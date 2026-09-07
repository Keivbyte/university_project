#include "platform/window.h"
#include "platform/framebuffer.h"
#include "platform/input.h"
#include "shared/time/timer.h"
#include <iostream>
#include <string>

int main() {
    try {
        Window window(1280, 800, L"Game");
        Framebuffer fb(320, 200);
        Timer timer;

        Input input;
        window.SetInput(&input);   // Window::WndProc now forwards into OnMessage

        fb.Clear(0xFFFF0000);
        fb.SetPixel(160, 100, 0xFF0000FF);

        double fpsDisplayTimer = 0.0;

        while (!window.ShouldClose()) {
            // --- Mandatory per-frame order: BeginFrame -> Pump -> Update -------
            // 1. Roll the input snapshot and clear frame accumulators.
            input.BeginFrame();

            // 2. Drain the OS queue; each message lands in Input::OnMessage.
            window.PumpMessages();

            // 3. Time step.
            double dt = timer.Tick();

            // 4. Update: game code POLLS the input table, it does not subscribe.
            if (input.WasPressed(KeyCode::Escape)) {
                break;
            }

            // Example reads (no gameplay wired up yet - just proving the API):
            //   bool  moveForward = input.IsDown(KeyCode::W);
            //   bool  jumped      = input.WasPressed(KeyCode::Space);
            //   bool  sprint      = input.IsDown(KeyCode::LShift);
            //   bool  fired       = input.WasPressed(KeyCode::MouseLeft);
            //   int   lookX       = input.MouseDeltaX();
            //   int   lookY       = input.MouseDeltaY();
            //   float zoom        = input.MouseWheel();
            (void)dt;

            // 5. Present + FPS readout, twice a second.
            fpsDisplayTimer += dt;
            if (fpsDisplayTimer >= 0.5) {
                std::wstring title = L"Game | FPS: " + std::to_wstring(static_cast<int>(timer.FPS()))
                                   + L" | mouse " + std::to_wstring(input.MouseX())
                                   + L"," + std::to_wstring(input.MouseY());
                SetWindowTextW(window.GetHandle(), title.c_str());
                fpsDisplayTimer = 0.0;
            }

            HDC hdc = GetDC(window.GetHandle());
            if (hdc) {
                fb.Present(hdc, 0, 0, window.GetWidth(), window.GetHeight());
                ReleaseDC(window.GetHandle(), hdc);
            }
        }

        window.SetInput(nullptr);   // detach before Input goes out of scope
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return -1;
    }

    return 0;
}
