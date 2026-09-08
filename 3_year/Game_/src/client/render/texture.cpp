#include "texture.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace {
std::size_t TexelCount(int width, int height) {
    if (width <= 0 || height <= 0) throw std::invalid_argument("Texture dimensions must be positive");
    const auto w = static_cast<std::size_t>(width);
    const auto h = static_cast<std::size_t>(height);
    if (w > std::vector<std::uint32_t>{}.max_size() / h)
        throw std::length_error("Texture is too large");
    return w * h;
}

int NearestIndex(float coordinate, int dimension, WrapMode mode) noexcept {
    // Double avoids overflow or an out-of-range int conversion at large dimensions.
    double wrapped = static_cast<double>(coordinate);
    if (mode == WrapMode::Repeat) wrapped -= std::floor(wrapped);
    else wrapped = std::clamp(wrapped, 0.0, 1.0);
    return static_cast<int>(std::floor(wrapped * static_cast<double>(dimension - 1) + 0.5));
}
} // namespace

Texture::Texture(int width, int height, std::vector<std::uint32_t> texels)
    : width_(width), height_(height), texels_(std::move(texels)) {
    if (texels_.size() != TexelCount(width, height))
        throw std::invalid_argument("Texture texel count does not match dimensions");
}

std::uint32_t Texture::Sample(float u, float v, WrapMode mode) const noexcept {
    if (!std::isfinite(u) || !std::isfinite(v) || texels_.empty()) return 0xFFFF00FFu;
    const int x = NearestIndex(u, width_, mode);
    const int y = NearestIndex(v, height_, mode);
    return texels_[static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
                   static_cast<std::size_t>(x)];
}

Texture Texture::GenerateCheckerboard(int size, std::array<std::uint32_t, 2> colors) {
    std::vector<std::uint32_t> pixels(TexelCount(size, size));
    const auto dimension = static_cast<std::size_t>(size);
    const auto cells = static_cast<std::size_t>(std::min(size, 8));
    for (std::size_t y = 0; y < dimension; ++y)
        for (std::size_t x = 0; x < dimension; ++x)
            pixels[y * dimension + x] = colors[((x * cells / dimension) +
                                               (y * cells / dimension)) % 2];
    return Texture(size, size, std::move(pixels));
}

Texture Texture::GenerateAsymmetricPattern(int size) {
    std::vector<std::uint32_t> pixels(TexelCount(size, size), 0xFFF0E8D0u);
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const float u = (static_cast<float>(x) + 0.5f) / static_cast<float>(size);
            const float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(size);
            const bool stem = u >= 0.20f && u < 0.35f && v >= 0.15f && v < 0.85f;
            const bool top = u >= 0.20f && u < 0.80f && v >= 0.15f && v < 0.30f;
            const bool middle = u >= 0.20f && u < 0.65f && v >= 0.43f && v < 0.57f;
            auto& pixel = pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(size) +
                                 static_cast<std::size_t>(x)];
            if (stem || top || middle) pixel = 0xFF202030u;
            // Unique top-left marker makes both UV-axis inversions visible.
            if (u < 0.10f && v < 0.10f) pixel = 0xFFE04030u;
        }
    }
    return Texture(size, size, std::move(pixels));
}
