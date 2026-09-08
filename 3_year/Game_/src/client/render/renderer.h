#pragma once

#include <cstdint>
#include <span>
#include "shared/math/mat.h"
#include "depth_buffer.h"
#include "texture.h"

class Framebuffer;

// Coordinates are already in framebuffer pixels: +X right, +Y down.
struct ScreenVertex {
    math::Vec2 position{};
    std::uint32_t color = 0xFFFFFFFFu; // 0xAARRGGBB
    float depth = 0.0f; // post-divide NDC z, affine in screen space
    math::Vec2 uv{}; // raw UV for affine mode
    math::Vec2 uv_over_w{};
    float inv_w = 1.0f; // reciprocal clip w; NOT the depth buffer value
};

struct MeshVertex {
    math::Vec3 position{};
    std::uint32_t color = 0xFFFFFFFFu;
    math::Vec2 uv{};
};

struct DrawParameters {
    const Texture* texture = nullptr; // null preserves the vertex-color path
    WrapMode wrap_mode = WrapMode::Repeat;
    bool perspective_correct = true;
};

class Renderer {
public:
    // The framebuffer must outlive the renderer. Dimensions are read per draw.
    explicit Renderer(Framebuffer& framebuffer);

    void ClearDepth(float value = 1.0f);
    const DepthBuffer& Depth() const noexcept { return depth_; }

    // No camera abstraction: the caller supplies fixed view/projection matrices.
    // Rejects the whole triangle if any clip w<=0 or clip z<0 (near plane).
    // No polygon splitting; offscreen X/Y are handled by the rasterizer AABB.
    void DrawMesh(std::span<const MeshVertex> vertices,
                  std::span<const std::uint32_t> indices, const math::Mat4& model,
                  const math::Mat4& view, const math::Mat4& projection,
                  const DrawParameters& parameters = {});

    // E(P) = (Px-Ax)(By-Ay) - (Py-Ay)(Bx-Ax).
    static constexpr float EdgeFunction(const math::Vec2& a, const math::Vec2& b,
                                        const math::Vec2& p) noexcept {
        return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
    }

    // NDC depth and strict LESS are unchanged. A texture replaces vertex color,
    // copying its full ARGB texel; without a texture, RGB is affine and alpha=255.
    // Direct screen callers must provide uv_over_w=uv*inv_w for corrected mode.
    // Visually CCW is the front face in this Y-down buffer; CW is culled.
    // All edges are inclusive for now: shared-edge pixels can be drawn twice.
    void DrawTriangle(const ScreenVertex& v0, const ScreenVertex& v1,
                      const ScreenVertex& v2, const DrawParameters& parameters = {});

private:
    Framebuffer& framebuffer_;
    DepthBuffer depth_;
    void SyncDepthSize();
};
