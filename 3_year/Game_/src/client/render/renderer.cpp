#include "renderer.h"
#include "client/platform/framebuffer.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace {
// EdgeFunction gives signed double-area, in squared screen pixels.
constexpr float AREA_EPSILON = 1.0e-6f;

struct RGB {
    float r;
    float g;
    float b;
};

constexpr RGB UnpackRGB(std::uint32_t color) noexcept {
    return {static_cast<float>((color >> 16u) & 0xFFu),
            static_cast<float>((color >> 8u) & 0xFFu),
            static_cast<float>(color & 0xFFu)};
}

std::uint32_t PackRGB(float r, float g, float b) noexcept {
    // Clamp rounding noise, then round each channel to the nearest byte.
    const auto channel = [](float value) {
        return static_cast<std::uint32_t>(std::clamp(value, 0.0f, 255.0f) + 0.5f);
    };
    return 0xFF000000u | (channel(r) << 16u) | (channel(g) << 8u) | channel(b);
}

bool IsFinite(const math::Vec2& p) noexcept {
    return std::isfinite(p.x) && std::isfinite(p.y);
}

// Coverage policy is separate from interpolation. A future top-left rule
// belongs here; its edge bias must NOT be applied to the barycentric values.
bool IsCovered(float e0, float e1, float e2, float signedArea) noexcept {
    if (signedArea > 0.0f) return e0 >= 0.0f && e1 >= 0.0f && e2 >= 0.0f;
    return e0 <= 0.0f && e1 <= 0.0f && e2 <= 0.0f;
}
} // namespace

Renderer::Renderer(Framebuffer& framebuffer)
    : framebuffer_(framebuffer), depth_(framebuffer.Width(), framebuffer.Height()) {}

void Renderer::SyncDepthSize() {
    if (depth_.Width() != framebuffer_.Width() || depth_.Height() != framebuffer_.Height())
        depth_.Resize(framebuffer_.Width(), framebuffer_.Height());
}

void Renderer::ClearDepth(float value) {
    SyncDepthSize();
    depth_.Clear(value);
}

void Renderer::DrawMesh(std::span<const MeshVertex> vertices,
                        std::span<const std::uint32_t> indices, const math::Mat4& model,
                        const math::Mat4& view, const math::Mat4& projection,
                        const DrawParameters& parameters) {
    if (indices.size() % 3 != 0) throw std::invalid_argument("Triangle index count must be a multiple of 3");
    for (const auto index : indices)
        if (index >= vertices.size()) throw std::out_of_range("Mesh vertex index out of range");
    SyncDepthSize();
    const float width = static_cast<float>(framebuffer_.Width());
    const float height = static_cast<float>(framebuffer_.Height());
    if (width <= 0.0f || height <= 0.0f) return;
    const math::Mat4 mvp = projection * view * model;

    for (std::size_t i = 0; i < indices.size(); i += 3) {
        math::Vec4 clip[3];
        bool rejected = false;
        for (std::size_t j = 0; j < 3; ++j) {
            const auto& p = vertices[indices[i + j]].position;
            clip[j] = mvp * math::Vec4{p.x, p.y, p.z, 1.0f};
            const auto& c = clip[j];
            // w<=0 rejects at/behind the eye; z<0 rejects the actual near plane
            // for the chosen [0,1] depth convention. Do this BEFORE division.
            if (!std::isfinite(c.x) || !std::isfinite(c.y) ||
                !std::isfinite(c.z) || !std::isfinite(c.w) || c.w <= 0.0f || c.z < 0.0f)
                rejected = true;
        }
        if (rejected) continue;

        ScreenVertex screen[3];
        for (std::size_t j = 0; j < 3; ++j) {
            const auto& c = clip[j];
            const math::Vec3 ndc{c.x / c.w, c.y / c.w, c.z / c.w};
            const auto& vertex = vertices[indices[i + j]];
            const float inv_w = 1.0f / c.w;
            screen[j] = {{(ndc.x + 1.0f) * 0.5f * width,
                          (1.0f - ndc.y) * 0.5f * height},
                         vertex.color, ndc.z, vertex.uv, vertex.uv * inv_w, inv_w};
        }
        // DrawTriangle performs screen-space CCW culling before its AABB loop.
        DrawTriangle(screen[0], screen[1], screen[2], parameters);
    }
}

void Renderer::DrawTriangle(const ScreenVertex& v0, const ScreenVertex& v1,
                            const ScreenVertex& v2, const DrawParameters& parameters) {
    SyncDepthSize();
    const int width = framebuffer_.Width();
    const int height = framebuffer_.Height();
    if (width <= 0 || height <= 0) return;
    if (!IsFinite(v0.position) || !IsFinite(v1.position) || !IsFinite(v2.position)) return;
    if (parameters.texture) {
        for (const ScreenVertex* vertex : {&v0, &v1, &v2}) {
            if (!IsFinite(vertex->uv)) return;
            if (parameters.perspective_correct &&
                (!IsFinite(vertex->uv_over_w) || !std::isfinite(vertex->inv_w) || vertex->inv_w <= 0.0f))
                return;
        }
    }

    const float signedArea = EdgeFunction(v0.position, v1.position, v2.position);
    if (!std::isfinite(signedArea) || std::fabs(signedArea) <= AREA_EPSILON) return;

    // With the prescribed edge function and Y-down framebuffer, visually CCW
    // triangles have POSITIVE area after the viewport Y inversion.
    const bool isFrontFace = signedArea > 0.0f;
    if (!isFrontFace) return;

    // AABB setup: clamp BEFORE integer conversion and iteration. Bounds are
    // conservative for center sampling; the maximum bounds are exclusive.
    // Double here preserves integer screen limits during safe float-to-int conversion.
    const double minX = std::min({v0.position.x, v1.position.x, v2.position.x});
    const double minY = std::min({v0.position.y, v1.position.y, v2.position.y});
    const double maxX = std::max({v0.position.x, v1.position.x, v2.position.x});
    const double maxY = std::max({v0.position.y, v1.position.y, v2.position.y});
    const int xBegin = static_cast<int>(std::floor(std::clamp(minX, 0.0, static_cast<double>(width))));
    const int yBegin = static_cast<int>(std::floor(std::clamp(minY, 0.0, static_cast<double>(height))));
    const int xEnd = static_cast<int>(std::ceil(std::clamp(maxX, 0.0, static_cast<double>(width))));
    const int yEnd = static_cast<int>(std::ceil(std::clamp(maxY, 0.0, static_cast<double>(height))));
    if (xBegin >= xEnd || yBegin >= yEnd) return;

    const RGB c0 = UnpackRGB(v0.color);
    const RGB c1 = UnpackRGB(v1.color);
    const RGB c2 = UnpackRGB(v2.color);
    std::uint32_t* const pixels = framebuffer_.Data();

    // Independent pixel evaluations; future tiling/SIMD can retain this setup.
    for (int y = yBegin; y < yEnd; ++y) {
        const std::size_t row = static_cast<std::size_t>(y) * static_cast<std::size_t>(width);
        for (int x = xBegin; x < xEnd; ++x) {
            const math::Vec2 p{static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f};
            const float e0 = EdgeFunction(v1.position, v2.position, p);
            const float e1 = EdgeFunction(v2.position, v0.position, p);
            const float e2 = EdgeFunction(v0.position, v1.position, p);
            if (!std::isfinite(e0) || !std::isfinite(e1) || !std::isfinite(e2)) continue;
            if (!IsCovered(e0, e1, e2, signedArea)) continue;

            // Divide by SIGNED area: reversing winding flips numerator and denominator.
            const float w0 = e0 / signedArea;
            const float w1 = e1 / signedArea;
            const float w2 = e2 / signedArea;
            if (!std::isfinite(w0) || !std::isfinite(w1) || !std::isfinite(w2)) continue;
            math::Vec2 uv;
            if (parameters.texture) {
                if (parameters.perspective_correct) {
                    const float inv_w = w0 * v0.inv_w + w1 * v1.inv_w + w2 * v2.inv_w;
                    if (!std::isfinite(inv_w) || inv_w <= 0.0f) continue;
                    const math::Vec2 uv_over_w = w0 * v0.uv_over_w + w1 * v1.uv_over_w + w2 * v2.uv_over_w;
                    uv = uv_over_w / inv_w;
                } else {
                    // Deliberately affine: raw UV, completely independent of clip w.
                    uv = w0 * v0.uv + w1 * v1.uv + w2 * v2.uv;
                }
                // Reject invalid attributes before depth write; no invisible occluders.
                if (!IsFinite(uv)) continue;
            }
            // z_ndc, unlike view-space z, is affine in screen barycentrics.
            // Do NOT divide this interpolation by interpolated reciprocal w.
            const float z = w0 * v0.depth + w1 * v1.depth + w2 * v2.depth;
            const std::size_t index = row + static_cast<std::size_t>(x);
            if (!depth_.TestAndWrite(index, z)) continue;
            const std::uint32_t color = parameters.texture
                ? parameters.texture->Sample(uv.x, uv.y, parameters.wrap_mode)
                : PackRGB(
                w0 * c0.r + w1 * c1.r + w2 * c2.r,
                w0 * c0.g + w1 * c1.g + w2 * c2.g,
                w0 * c0.b + w1 * c1.b + w2 * c2.b);
            pixels[index] = color;
        }
    }
}
