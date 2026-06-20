#pragma once
#include <vector>
#include <cstdint>
#include <windows.h>
#include <algorithm>

class Framebuffer {
public:
    Framebuffer(int width, int height) : size_x_(width), size_y_(height) {
        argb_.resize(width * height);
        InitBitMapInfo();
    }

    void Resize(int width, int height) {
        size_x_ = width;
        size_y_ = height;
        argb_.resize(width * height);
    }

    uint32_t* Data() noexcept {
        return argb_.data();
    }

    void Clear(uint32_t argb) {
        std::fill(argb_.begin(), argb_.end(), argb);
    }

    void setPixel(int x, int y, uint32_t argb) {
        if (x < 0 || x >= size_x_ || y < 0 || y >= size_y_) return;
        argb_[y*size_x_ + x] = argb;
    }

    uint32_t getPixel(int x, int y) const {
        if (x < 0 || x >= size_x_ || y < 0 || y >= size_y_) return 0;
        return argb_[y * size_x_ + x];
    }

    void Present(HDC dstDC, int dstX, int dstY, int dstWidth, int dstHeight) const {
        if (dstDC == nullptr || dstWidth <= 0 || dstHeight <= 0) return;

        StretchDIBits(
            dstDC,
            dstX, dstY,
            dstWidth, dstHeight,
            0, 0,
            size_x_, size_y_,
            &argb_[0],
            &bmi_,
            DIB_RGB_COLORS,
            SRCCOPY
        );
    }

    int Width() const noexcept { return size_x_; }
    int Height() const noexcept { return size_y_; }

private:

    void InitBitMapInfo() {
        bmi_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi_.bmiHeader.biWidth = size_x_;
        bmi_.bmiHeader.biHeight = -size_y_; // top-down
        bmi_.bmiHeader.biPlanes = 1;
        bmi_.bmiHeader.biBitCount = 32;
        bmi_.bmiHeader.biCompression = BI_RGB;
    }

    std::vector<uint32_t> argb_;
    int size_x_, size_y_;
    BITMAPINFO bmi_;
};

