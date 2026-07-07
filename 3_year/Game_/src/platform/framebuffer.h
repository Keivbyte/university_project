#pragma once

#include <vector>
#include <cstdint>
#include <windows.h>

class Framebuffer {
public:
    /*
     * @func  Framebuffer
     * @desc  Constructor. Allocates memory for the framebuffer and initializes bitmap info.
     * @param width: the width of the framebuffer in pixels
     * @param height: the height of the framebuffer in pixels
     */
    Framebuffer(int width, int height);

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&&) = default;
    Framebuffer& operator=(Framebuffer&&) = default;

    /*
     * @func  Resize
     * @desc  Resizes the internal pixel buffer and bitmap info. Clears the buffer to 0.
     * @param width: the new width in pixels
     * @param height: the new height in pixels
     */
    void Resize(int width, int height);

    /*
     * @func  Data
     * @desc  Provides raw access to the underlying pixel data array.
     * @return Pointer to the first element of the pixel buffer.
     */
    uint32_t* Data() noexcept;

    /*
     * @func  Data
     * @desc  Provides read-only raw access to the underlying pixel data array.
     * @return Const pointer to the first element of the pixel buffer.
     */
    const uint32_t* Data() const noexcept;

    /*
     * @func  Clear
     * @desc  Fills the entire framebuffer with a single solid color.
     * @param argb: the color to fill the buffer with, in ARGB 32-bit format
     */
    void Clear(uint32_t argb);

    /*
     * @func  SetPixel
     * @desc  Sets the color of a specific pixel in the framebuffer. Checks for bounds.
     * @param x: the x coordinate of the pixel
     * @param y: the y coordinate of the pixel
     * @param argb: the color to set, in ARGB 32-bit format
     */
    void SetPixel(int x, int y, uint32_t argb);

    /*
     * @func  GetPixel
     * @desc  Retrieves the color of a specific pixel in the framebuffer. Checks for bounds.
     * @param x: the x coordinate of the pixel
     * @param y: the y coordinate of the pixel
     * @return The color of the requested pixel in ARGB format, or 0 if out of bounds.
     */
    uint32_t GetPixel(int x, int y) const;

    /*
     * @func  Present
     * @desc  Draws the framebuffer contents directly to a device context (window) using GDI.
     * @param dstDC: the target device context handle
     * @param dstX: destination X coordinate
     * @param dstY: destination Y coordinate
     * @param dstWidth: width of the destination area (for stretching)
     * @param dstHeight: height of the destination area (for stretching)
     */
    void Present(HDC dstDC, int dstX, int dstY, int dstWidth, int dstHeight) const {
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

    /*
     * @func  Width
     * @desc  Gets the current width of the framebuffer.
     * @return Framebuffer width in pixels.
     */
    int Width() const noexcept { return size_x_; }

    /*
     * @func  Height
     * @desc  Gets the current height of the framebuffer.
     * @return Framebuffer height in pixels.
     */
    int Height() const noexcept { return size_y_; }

private:
    /*
     * @func  InitBitMapInfo
     * @desc  Initializes the BITMAPINFO structure required by GDI for rendering.
     *        Sets up a 32-bit top-down DIB (Device-Independent Bitmap).
     */
    void InitBitMapInfo();

    int size_x_;
    int size_y_;
    BITMAPINFO bmi_;
    std::vector<uint32_t> argb_;
};