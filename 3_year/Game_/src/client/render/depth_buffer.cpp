#include "depth_buffer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

DepthBuffer::DepthBuffer(int width, int height) { Resize(width, height); }

void DepthBuffer::Resize(int width, int height) {
    if (width < 0 || height < 0) throw std::invalid_argument("Negative depth buffer size");
    const auto w = static_cast<std::size_t>(width);
    const auto h = static_cast<std::size_t>(height);
    if (h != 0 && w > values_.max_size() / h)
        throw std::length_error("Depth buffer too large");
    // Commit dimensions only after successful allocation.
    values_.assign(w * h, 1.0f);
    width_ = width;
    height_ = height;
}

void DepthBuffer::Clear(float value) {
    if (!std::isfinite(value) || value < 0.0f || value > 1.0f)
        throw std::invalid_argument("Depth clear value must be in [0,1]");
    std::fill(values_.begin(), values_.end(), value);
}

bool DepthBuffer::TestAndWrite(std::size_t index, float depth) noexcept {
    assert(index < values_.size());
    if (index >= values_.size() || !std::isfinite(depth) || depth < 0.0f || depth > 1.0f)
        return false;
    if (!(depth < values_[index])) return false; // strict LESS, equal depth keeps first pixel
    values_[index] = depth;
    return true;
}
