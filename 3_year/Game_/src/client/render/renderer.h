#pragma once

#include <cstdint>
#include "shared/math/vec.h"

class Framebuffer;

// Coordinates are already in framebuffer pixels: +X right, +Y down.
struct ScreenVertex {
    math::Vec2 position{};
    std::uint32_t color = 0xFFFFFFFFu; // 0xAARRGGBB
};

class Renderer {
public:
    // The framebuffer must outlive the renderer. Dimensions are read per draw.
    explicit Renderer(Framebuffer& framebuffer) noexcept : framebuffer_(framebuffer) {}

    // E(P) = (Px-Ax)(By-Ay) - (Py-Ay)(Bx-Ax).
    static constexpr float EdgeFunction(const math::Vec2& a, const math::Vec2& b,
                                        const math::Vec2& p) noexcept {
        return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
    }

    // Affine RGB interpolation, opaque output (alpha=255), no blending/depth.
    // Both windings render; visually CCW is the front face in this Y-down buffer.
    // All edges are inclusive for now: shared-edge pixels can be drawn twice.
    void DrawTriangle(const ScreenVertex& v0, const ScreenVertex& v1,
                      const ScreenVertex& v2);

private:
    Framebuffer& framebuffer_;
};
