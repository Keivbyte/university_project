#include "framebuffer.h"
#include <algorithm>
#include <cassert>

Framebuffer::Framebuffer(int width, int height)
    : size_x_(width), size_y_(height), bmi_{}, argb_(width * height)
{
    InitBitMapInfo();
}

void Framebuffer::Resize(int width, int height) {
    size_x_ = width;
    size_y_ = height;
    argb_.resize(width * height);
    InitBitMapInfo();
}

uint32_t* Framebuffer::Data() noexcept {
    return argb_.data();
}

void Framebuffer::Clear(uint32_t argb) {
    std::fill(argb_.begin(), argb_.end(), argb);
}

void Framebuffer::SetPixel(int x, int y, uint32_t argb) {
    assert(x >= 0 && x < size_x_ && y >= 0 && y < size_y_ && "SetPixel: Out of bounds!");
    if (x < 0 || x >= size_x_ || y < 0 || y >= size_y_) return;
    argb_[y * size_x_ + x] = argb;
}

uint32_t Framebuffer::GetPixel(int x, int y) const {
    assert(x >= 0 && x < size_x_ && y >= 0 && y < size_y_ && "GetPixel: Out of bounds!");
    if (x < 0 || x >= size_x_ || y < 0 || y >= size_y_) return 0;
    return argb_[y * size_x_ + x];
}

void Framebuffer::InitBitMapInfo() {
    ZeroMemory(&bmi_, sizeof(BITMAPINFO));

    bmi_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi_.bmiHeader.biWidth = size_x_;
    bmi_.bmiHeader.biHeight = -size_y_; // top-down
    bmi_.bmiHeader.biPlanes = 1;
    bmi_.bmiHeader.biBitCount = 32;
    bmi_.bmiHeader.biCompression = BI_RGB;
}