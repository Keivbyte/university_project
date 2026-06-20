#pragma once

#include <vector>
#include <cstdint>
#include <windows.h>

class Framebuffer {
public:
    Framebuffer(int width, int height);

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&&) = default;
    Framebuffer& operator=(Framebuffer&&) = default;

    void Resize(int width, int height);
    uint32_t* Data() noexcept;
    void Clear(uint32_t argb);

    void SetPixel(int x, int y, uint32_t argb);
    uint32_t GetPixel(int x, int y) const;

    inline void Present(HDC dstDC, int dstX, int dstY, int dstWidth, int dstHeight) const {
        if (dstDC == nullptr || dstWidth <= 0 || dstHeight <= 0) return;

        SetStretchBltMode(dstDC, COLORONCOLOR);

        StretchDIBits(
            dstDC,
            dstX, dstY,
            dstWidth, dstHeight,
            0, 0,
            size_x_, size_y_,
            argb_.data(),
            &bmi_,
            DIB_RGB_COLORS,
            SRCCOPY
        );
    }

    int Width() const noexcept { return size_x_; }
    int Height() const noexcept { return size_y_; }

private:
    void InitBitMapInfo();

    int size_x_;
    int size_y_;
    BITMAPINFO bmi_;
    std::vector<uint32_t> argb_;
};