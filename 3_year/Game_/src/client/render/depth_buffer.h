#pragma once

#include <cstddef>
#include <vector>

// Stores post-divide NDC z in [0,1], NOT reciprocal clip w.
class DepthBuffer {
public:
    DepthBuffer(int width, int height);
    void Resize(int width, int height);
    void Clear(float value = 1.0f);
    int Width() const noexcept { return width_; }
    int Height() const noexcept { return height_; }
    const float* Data() const noexcept { return values_.data(); }
    bool TestAndWrite(std::size_t index, float depth) noexcept;

private:
    int width_ = 0;
    int height_ = 0;
    std::vector<float> values_;
};
