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

        // UV seams require four independent vertices per face (24 total).
        // Each quad is bottom-left, bottom-right, top-right, top-left, outward CCW.
        const std::array<MeshVertex, 24> cubeVertices{{
            {{-1,-1, 1}, 0xFFFFFFFFu, {0,1}}, {{ 1,-1, 1}, 0xFFFFFFFFu, {1,1}},
            {{ 1, 1, 1}, 0xFFFFFFFFu, {1,0}}, {{-1, 1, 1}, 0xFFFFFFFFu, {0,0}}, // +Z
            {{ 1,-1,-1}, 0xFFFFFFFFu, {0,1}}, {{-1,-1,-1}, 0xFFFFFFFFu, {1,1}},
            {{-1, 1,-1}, 0xFFFFFFFFu, {1,0}}, {{ 1, 1,-1}, 0xFFFFFFFFu, {0,0}}, // -Z
            {{-1,-1,-1}, 0xFFFFFFFFu, {0,1}}, {{-1,-1, 1}, 0xFFFFFFFFu, {1,1}},
            {{-1, 1, 1}, 0xFFFFFFFFu, {1,0}}, {{-1, 1,-1}, 0xFFFFFFFFu, {0,0}}, // -X
            {{ 1,-1, 1}, 0xFFFFFFFFu, {0,1}}, {{ 1,-1,-1}, 0xFFFFFFFFu, {1,1}},
            {{ 1, 1,-1}, 0xFFFFFFFFu, {1,0}}, {{ 1, 1, 1}, 0xFFFFFFFFu, {0,0}}, // +X
            {{-1, 1,-1}, 0xFFFFFFFFu, {0,1}}, {{-1, 1, 1}, 0xFFFFFFFFu, {1,1}},
            {{ 1, 1, 1}, 0xFFFFFFFFu, {1,0}}, {{ 1, 1,-1}, 0xFFFFFFFFu, {0,0}}, // +Y
            {{-1,-1,-1}, 0xFFFFFFFFu, {0,1}}, {{ 1,-1,-1}, 0xFFFFFFFFu, {1,1}},
            {{ 1,-1, 1}, 0xFFFFFFFFu, {1,0}}, {{-1,-1, 1}, 0xFFFFFFFFu, {0,0}}  // -Y
        }};
        const std::array<std::uint32_t, 36> cubeIndices{{
             0, 1, 2,  0, 2, 3,  4, 5, 6,  4, 6, 7,
             8, 9,10,  8,10,11, 12,13,14, 12,14,15,
            16,17,18, 16,18,19, 20,21,22, 20,22,23
        }};
        const Texture checker = Texture::GenerateCheckerboard(128);
        const Texture pattern = Texture::GenerateAsymmetricPattern(128);
        DrawParameters parameters{&checker, WrapMode::Clamp, true};
        bool showPattern = false;
        bool paused = false;
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
            const bool toggleInterpolation = input.WasPressed(KeyCode::T);
            const bool togglePattern = input.WasPressed(KeyCode::F);
            if (toggleInterpolation) parameters.perspective_correct = !parameters.perspective_correct;
            if (togglePattern) showPattern = !showPattern;
            if (input.WasPressed(KeyCode::Space)) paused = !paused;
            parameters.texture = showPattern ? &pattern : &checker;

            // Example reads (no gameplay wired up yet - just proving the API):
            //   bool  moveForward = input.IsDown(KeyCode::W);
            //   bool  jumped      = input.WasPressed(KeyCode::Space);
            //   bool  sprint      = input.IsDown(KeyCode::LShift);
            //   bool  fired       = input.WasPressed(KeyCode::MouseLeft);
            //   int   lookX       = input.MouseDeltaX();
            //   int   lookY       = input.MouseDeltaY();
            //   float zoom        = input.MouseWheel();
            // 5. Rotate in radians/second, then draw with MVP = Projection * View * Model.
            if (!paused) angle = std::fmod(angle + static_cast<float>(dt) * 0.8f, math::TWO_PI);
            const math::Mat4 model = math::Mat4::rotationY(angle) * math::Mat4::rotationX(0.5f);
            fb.Clear(0xFF182030u);
            renderer.ClearDepth();
            renderer.DrawMesh(cubeVertices, cubeIndices, model, view, projection, parameters);

            // 6. Present + FPS readout, twice a second.
            fpsDisplayTimer += dt;
            if (fpsDisplayTimer >= 0.5 || toggleInterpolation || togglePattern) {
                std::wstring title = L"Game | FPS: " + std::to_wstring(static_cast<int>(timer.FPS()))
                                   + (parameters.perspective_correct ? L" | Perspective-correct" : L" | Affine")
                                   + (showPattern ? L" | F pattern" : L" | Checkerboard")
                                   + L" | T: interpolation | F: texture | Space: pause";
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
