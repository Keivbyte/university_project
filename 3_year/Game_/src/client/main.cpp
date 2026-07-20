#include "platform/window.h"
#include "platform/framebuffer.h"
#include "shared/time/timer.h"
#include <iostream>
#include <string>

int main() {
    try {
        Window window(1280, 800, L"Game");
        Framebuffer fb(320, 200);
        Timer timer;

        fb.Clear(0xFFFF0000);
        fb.SetPixel(160, 100, 0xFF0000FF);

        double fpsDisplayTimer = 0.0;

        while (!window.ShouldClose()) {
            window.PumpMessages();

            // 1. Считаем время
            double dt = timer.Tick();

            // 2. Обновляем логику (пока пустая, но dt уже готов!)
            // Update(dt);

            // 3. Выводим FPS в заголовок окна 2 раза в секунду
            fpsDisplayTimer += dt;
            if (fpsDisplayTimer >= 0.5) {
                std::wstring title = L"Game | FPS: " + std::to_wstring(static_cast<int>(timer.FPS()));
                SetWindowTextW(window.GetHandle(), title.c_str());
                fpsDisplayTimer = 0.0;
            }

            // 4. Рендер (Clear, отрисовка, Present)
            // fb.Clear(0xFF000000);

            HDC hdc = GetDC(window.GetHandle());
            if (hdc) {
                fb.Present(hdc, 0, 0, window.GetWidth(), window.GetHeight());
                ReleaseDC(window.GetHandle(), hdc);
            }

        }
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return -1;
    }

    return 0;
}