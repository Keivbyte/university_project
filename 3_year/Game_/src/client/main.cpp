#include "platform/window.h"
#include "platform/framebuffer.h"
#include "platform/input.h"
#include "render/renderer.h"
#include "render/projection.h"
#include "shared/time/timer.h"
#include <iostream>
#include <string>
#include <array>
#include <cmath>

int main() {
    try {
        Window window(1280, 800, L"Game");
        Framebuffer fb(320, 200);
        Renderer renderer(fb);
        Timer timer;

        // Eight shared vertices, twelve outward-facing CCW triangles.
        const std::array<MeshVertex, 8> cubeVertices{{
            {{-1, -1, -1}, 0xFFFF4040u}, {{1, -1, -1}, 0xFF40FF40u},
            {{1, 1, -1}, 0xFF4040FFu}, {{-1, 1, -1}, 0xFFFFFF40u},
            {{-1, -1, 1}, 0xFFFF40FFu}, {{1, -1, 1}, 0xFF40FFFFu},
            {{1, 1, 1}, 0xFFFFFFFFu}, {{-1, 1, 1}, 0xFFFFA040u}
        }};
        const std::array<std::uint32_t, 36> cubeIndices{{
            4, 5, 6, 4, 6, 7, // +Z
            1, 0, 3, 1, 3, 2, // -Z
            0, 4, 7, 0, 7, 3, // -X
            5, 1, 2, 5, 2, 6, // +X
            3, 7, 6, 3, 6, 2, // +Y
            0, 1, 5, 0, 5, 4  // -Y
        }};
        const math::Mat4 view = math::Mat4::translation({0, 0, -5});
        // The 320x200 target and 1280x800 window have the same aspect ratio.
        const float aspect = static_cast<float>(fb.Width()) / static_cast<float>(fb.Height());
        const math::Mat4 projection = Perspective(math::radians(70.0f), aspect, 0.1f, 100.0f);
        float angle = 0.0f;

        Input input;
        window.SetInput(&input);   // Window::WndProc now forwards into OnMessage

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
            // 5. Rotate in radians/second, then draw with MVP = Projection * View * Model.
            angle = std::fmod(angle + static_cast<float>(dt) * 0.8f, math::TWO_PI);
            const math::Mat4 model = math::Mat4::rotationY(angle) * math::Mat4::rotationX(0.5f);
            fb.Clear(0xFF182030u);
            renderer.ClearDepth();
            renderer.DrawMesh(cubeVertices, cubeIndices, model, view, projection);

            // 6. Present + FPS readout, twice a second.
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
