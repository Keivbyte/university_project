#include "platform/window.h"
#include "platform/framebuffer.h"
#include <iostream>

int main() {
    try {
        Window window(1280, 800, L"Game");
        Framebuffer fb(320, 200);
        
        fb.Clear(0xFFFF0000);

        fb.SetPixel(160, 100, 0xFF0000FF);
        fb.SetPixel(161, 100, 0xFF0000FF);
        fb.SetPixel(160, 101, 0xFF0000FF);
        fb.SetPixel(161, 101, 0xFF0000FF);

        while (!window.ShouldClose()) {
            window.PumpMessages();

            HDC hdc = GetDC(window.GetHandle());
            if (hdc) {
                fb.Present(hdc, 0, 0, window.GetWidth(), window.GetHeight());
                ReleaseDC(window.GetHandle(), hdc);
            }

            Sleep(16); 
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return -1;
    }

    return 0;
}