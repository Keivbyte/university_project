#pragma once

#include <cstdint>
#include <span>
#include "shared/math/mat.h"
#include "depth_buffer.h"

class Framebuffer;

// Coordinates are already in framebuffer pixels: +X right, +Y down.
struct ScreenVertex {
    math::Vec2 position{};
    std::uint32_t color = 0xFFFFFFFFu; // 0xAARRGGBB
    float depth = 0.0f; // post-divide NDC z, affine in screen space
};

struct MeshVertex {
    math::Vec3 position{};
    std::uint32_t color = 0xFFFFFFFFu;
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
                  const math::Mat4& view, const math::Mat4& projection);

    // E(P) = (Px-Ax)(By-Ay) - (Py-Ay)(Bx-Ax).
    static constexpr float EdgeFunction(const math::Vec2& a, const math::Vec2& b,
                                        const math::Vec2& p) noexcept {
        return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
    }

    // Affine RGB and NDC depth, strict LESS, opaque output (alpha=255).
    // Visually CCW is the front face in this Y-down buffer; CW is culled.
    // All edges are inclusive for now: shared-edge pixels can be drawn twice.
    void DrawTriangle(const ScreenVertex& v0, const ScreenVertex& v1,
                      const ScreenVertex& v2);

private:
    Framebuffer& framebuffer_;
    DepthBuffer depth_;
    void SyncDepthSize();
};
