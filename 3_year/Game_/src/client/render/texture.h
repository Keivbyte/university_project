#pragma once

#include <array>
#include <cstdint>
#include <vector>

enum class WrapMode { Repeat, Clamp };

class Texture {
public:
    // In-memory texels only. Positive dimensions and exactly width*height texels.
    Texture(int width, int height, std::vector<std::uint32_t> texels);
    int Width() const noexcept { return width_; }
    int Height() const noexcept { return height_; }

    // UV origin is TOP-LEFT; U right, V down. ARGB copied without conversion.
    // Wrap first, then nearest: round(uv * (dimension-1)). Clamp endpoints
    // select the first/last texel; Repeat maps integral UV (including 1) to 0.
    // Nonfinite UV returns diagnostic magenta, never an invalid array index.
    std::uint32_t Sample(float u, float v, WrapMode mode) const noexcept;

    static Texture GenerateCheckerboard(int size,
        std::array<std::uint32_t, 2> colors = {0xFFF0F0F0u, 0xFF202030u});
    static Texture GenerateAsymmetricPattern(int size);

private:
    int width_;
    int height_;
    std::vector<std::uint32_t> texels_;
};
