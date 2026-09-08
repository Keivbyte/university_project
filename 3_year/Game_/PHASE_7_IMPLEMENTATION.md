# Этап 7 — полный листинг реализации

Ниже приведены полные заголовки, cpp-файлы, тестовая сцена и интеграция CMake.
Исходники в src являются основной версией; этот документ — снимок реализации этапа 7.

Принятые соглашения:
- MVP = Projection * View * Model, векторы-столбцы, правая система, камера смотрит в -Z.
- Перспектива переводит near/far в NDC z=0/1. В буфере хранится z_ndc, а не 1/w:
  это следует последнему уточнению задания. Очистка 1, сравнение строго <.
- До деления треугольник целиком отбрасывается при любом w<=0 или z_clip<0.
  Первое условие защищает от положения за камерой, второе проверяет near-плоскость.
  Нарезки треугольников нет; возможное исчезновение пересекающей near грани ожидаемо.
- Экранный Y инвертируется. Заданная edge function после инверсии даёт положительную
  площадь для видимого CCW. CW отбрасывается до обхода AABB.
- Глубина и RGB интерполируются экранными барицентриками. Выходная альфа 255.
  Глубина вне [0,1] не записывается; при равной глубине остаётся первый пиксель.
- При изменении размеров framebuffer DepthBuffer автоматически меняет размер
  при следующем ClearDepth/DrawTriangle/DrawMesh. Оба буфера очищаются каждый кадр.
- Нет текстур, света, управления камерой, загрузчиков геометрии, SIMD или потоков.

Проверено MinGW GCC 14.2: клиент собирается; все 6 CTest-целей проходят.
На каждый режим: 215 проверок растеризации и 52 проверки 3D-пайплайна;
математика: 107 Debug / 119 Release. Тестовые цели используют строгие предупреждения
и -Werror. Куб проверен в четырёх положениях; изменение порядка подачи треугольников
не меняет изображение. Кадр из реального framebuffer сохранён для визуальной проверки.

Команды из корня проекта (CMake и компилятор должны быть в PATH):

```sh
cmake --build build-math
ctest --test-dir build-math --output-on-failure
```

Для запуска сцены: build-math/3_year.exe. Escape закрывает окно.

## Game_/src/client/render/depth_buffer.h

```cpp
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
```

## Game_/src/client/render/depth_buffer.cpp

```cpp
#include "depth_buffer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

DepthBuffer::DepthBuffer(int width, int height) { Resize(width, height); }

void DepthBuffer::Resize(int width, int height) {
    if (width < 0 || height < 0) throw std::invalid_argument("Negative depth buffer size");
    const auto w = static_cast<std::size_t>(width);
    const auto h = static_cast<std::size_t>(height);
    if (h != 0 && w > values_.max_size() / h)
        throw std::length_error("Depth buffer too large");
    // Commit dimensions only after successful allocation.
    values_.assign(w * h, 1.0f);
    width_ = width;
    height_ = height;
}

void DepthBuffer::Clear(float value) {
    if (!std::isfinite(value) || value < 0.0f || value > 1.0f)
        throw std::invalid_argument("Depth clear value must be in [0,1]");
    std::fill(values_.begin(), values_.end(), value);
}

bool DepthBuffer::TestAndWrite(std::size_t index, float depth) noexcept {
    assert(index < values_.size());
    if (index >= values_.size() || !std::isfinite(depth) || depth < 0.0f || depth > 1.0f)
        return false;
    if (!(depth < values_[index])) return false; // strict LESS, equal depth keeps first pixel
    values_[index] = depth;
    return true;
}
```

## Game_/src/client/render/projection.h

```cpp
#pragma once

#include "shared/math/mat.h"

// Right-handed perspective, column vectors, camera looks along -Z.
// Vertical FOV in radians. NDC depth is [0,1]: near -> 0, far -> 1.
math::Mat4 Perspective(float verticalFov, float aspect, float nearPlane, float farPlane);
```

## Game_/src/client/render/projection.cpp

```cpp
#include "projection.h"

#include <cmath>
#include <stdexcept>

math::Mat4 Perspective(float verticalFov, float aspect, float nearPlane, float farPlane) {
    if (!std::isfinite(verticalFov) || !std::isfinite(aspect) ||
        !std::isfinite(nearPlane) || !std::isfinite(farPlane) ||
        verticalFov <= 0.0f || verticalFov >= math::PI || aspect <= 0.0f ||
        nearPlane <= 0.0f || farPlane <= nearPlane)
        throw std::invalid_argument("Invalid perspective parameters");

    const float focalScale = 1.0f / std::tan(verticalFov * 0.5f);
    math::Mat4 result;
    result(0, 0) = focalScale / aspect;
    result(1, 1) = focalScale;
    result(2, 2) = farPlane / (nearPlane - farPlane);
    result(2, 3) = nearPlane * result(2, 2);
    result(3, 2) = -1.0f; // clip.w = -view.z
    return result;
}
```

## Game_/src/client/render/renderer.h

```cpp
#pragma once

#include <cstdint>
#include <span>
#include "shared/math/mat.h"
#include "depth_buffer.h"

class Framebuffer;

// Coordinates are already in framebuffer pixels: +X right, +Y down.
struct ScreenVertex {
    math::Vec2 position{};
    std::uint32_t color = 0xFFFFFFFFu; // 0xAARRGGBB
    float depth = 0.0f; // post-divide NDC z, affine in screen space
};

struct MeshVertex {
    math::Vec3 position{};
    std::uint32_t color = 0xFFFFFFFFu;
};

class Renderer {
public:
    // The framebuffer must outlive the renderer. Dimensions are read per draw.
    explicit Renderer(Framebuffer& framebuffer);

    void ClearDepth(float value = 1.0f);
    const DepthBuffer& Depth() const noexcept { return depth_; }

    // No camera abstraction: the caller supplies fixed view/projection matrices.
    // Rejects the whole triangle if any clip w<=0 or clip z<0 (near plane).
    // No polygon splitting; offscreen X/Y are handled by the rasterizer AABB.
    void DrawMesh(std::span<const MeshVertex> vertices,
                  std::span<const std::uint32_t> indices, const math::Mat4& model,
                  const math::Mat4& view, const math::Mat4& projection);

    // E(P) = (Px-Ax)(By-Ay) - (Py-Ay)(Bx-Ax).
    static constexpr float EdgeFunction(const math::Vec2& a, const math::Vec2& b,
                                        const math::Vec2& p) noexcept {
        return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
    }

    // Affine RGB and NDC depth, strict LESS, opaque output (alpha=255).
    // Visually CCW is the front face in this Y-down buffer; CW is culled.
    // All edges are inclusive for now: shared-edge pixels can be drawn twice.
    void DrawTriangle(const ScreenVertex& v0, const ScreenVertex& v1,
                      const ScreenVertex& v2);

private:
    Framebuffer& framebuffer_;
    DepthBuffer depth_;
    void SyncDepthSize();
};
```

## Game_/src/client/render/renderer.cpp

```cpp
#include "renderer.h"
#include "client/platform/framebuffer.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace {
// EdgeFunction gives signed double-area, in squared screen pixels.
constexpr float AREA_EPSILON = 1.0e-6f;

struct RGB {
    float r;
    float g;
    float b;
};

constexpr RGB UnpackRGB(std::uint32_t color) noexcept {
    return {static_cast<float>((color >> 16u) & 0xFFu),
            static_cast<float>((color >> 8u) & 0xFFu),
            static_cast<float>(color & 0xFFu)};
}

std::uint32_t PackRGB(float r, float g, float b) noexcept {
    // Clamp rounding noise, then round each channel to the nearest byte.
    const auto channel = [](float value) {
        return static_cast<std::uint32_t>(std::clamp(value, 0.0f, 255.0f) + 0.5f);
    };
    return 0xFF000000u | (channel(r) << 16u) | (channel(g) << 8u) | channel(b);
}

bool IsFinite(const math::Vec2& p) noexcept {
    return std::isfinite(p.x) && std::isfinite(p.y);
}

// Coverage policy is separate from interpolation. A future top-left rule
// belongs here; its edge bias must NOT be applied to the barycentric values.
bool IsCovered(float e0, float e1, float e2, float signedArea) noexcept {
    if (signedArea > 0.0f) return e0 >= 0.0f && e1 >= 0.0f && e2 >= 0.0f;
    return e0 <= 0.0f && e1 <= 0.0f && e2 <= 0.0f;
}
} // namespace

Renderer::Renderer(Framebuffer& framebuffer)
    : framebuffer_(framebuffer), depth_(framebuffer.Width(), framebuffer.Height()) {}

void Renderer::SyncDepthSize() {
    if (depth_.Width() != framebuffer_.Width() || depth_.Height() != framebuffer_.Height())
        depth_.Resize(framebuffer_.Width(), framebuffer_.Height());
}

void Renderer::ClearDepth(float value) {
    SyncDepthSize();
    depth_.Clear(value);
}

void Renderer::DrawMesh(std::span<const MeshVertex> vertices,
                        std::span<const std::uint32_t> indices, const math::Mat4& model,
                        const math::Mat4& view, const math::Mat4& projection) {
    if (indices.size() % 3 != 0) throw std::invalid_argument("Triangle index count must be a multiple of 3");
    for (const auto index : indices)
        if (index >= vertices.size()) throw std::out_of_range("Mesh vertex index out of range");
    SyncDepthSize();
    const float width = static_cast<float>(framebuffer_.Width());
    const float height = static_cast<float>(framebuffer_.Height());
    if (width <= 0.0f || height <= 0.0f) return;
    const math::Mat4 mvp = projection * view * model;

    for (std::size_t i = 0; i < indices.size(); i += 3) {
        math::Vec4 clip[3];
        bool rejected = false;
        for (std::size_t j = 0; j < 3; ++j) {
            const auto& p = vertices[indices[i + j]].position;
            clip[j] = mvp * math::Vec4{p.x, p.y, p.z, 1.0f};
            const auto& c = clip[j];
            // w<=0 rejects at/behind the eye; z<0 rejects the actual near plane
            // for the chosen [0,1] depth convention. Do this BEFORE division.
            if (!std::isfinite(c.x) || !std::isfinite(c.y) ||
                !std::isfinite(c.z) || !std::isfinite(c.w) || c.w <= 0.0f || c.z < 0.0f)
                rejected = true;
        }
        if (rejected) continue;

        ScreenVertex screen[3];
        for (std::size_t j = 0; j < 3; ++j) {
            const auto& c = clip[j];
            const math::Vec3 ndc{c.x / c.w, c.y / c.w, c.z / c.w};
            screen[j] = {{(ndc.x + 1.0f) * 0.5f * width,
                          (1.0f - ndc.y) * 0.5f * height},
                         vertices[indices[i + j]].color, ndc.z};
        }
        // DrawTriangle performs screen-space CCW culling before its AABB loop.
        DrawTriangle(screen[0], screen[1], screen[2]);
    }
}

void Renderer::DrawTriangle(const ScreenVertex& v0, const ScreenVertex& v1,
                            const ScreenVertex& v2) {
    SyncDepthSize();
    const int width = framebuffer_.Width();
    const int height = framebuffer_.Height();
    if (width <= 0 || height <= 0) return;
    if (!IsFinite(v0.position) || !IsFinite(v1.position) || !IsFinite(v2.position)) return;

    const float signedArea = EdgeFunction(v0.position, v1.position, v2.position);
    if (!std::isfinite(signedArea) || std::fabs(signedArea) <= AREA_EPSILON) return;

    // With the prescribed edge function and Y-down framebuffer, visually CCW
    // triangles have POSITIVE area after the viewport Y inversion.
    const bool isFrontFace = signedArea > 0.0f;
    if (!isFrontFace) return;

    // AABB setup: clamp BEFORE integer conversion and iteration. Bounds are
    // conservative for center sampling; the maximum bounds are exclusive.
    // Double here preserves integer screen limits during safe float-to-int conversion.
    const double minX = std::min({v0.position.x, v1.position.x, v2.position.x});
    const double minY = std::min({v0.position.y, v1.position.y, v2.position.y});
    const double maxX = std::max({v0.position.x, v1.position.x, v2.position.x});
    const double maxY = std::max({v0.position.y, v1.position.y, v2.position.y});
    const int xBegin = static_cast<int>(std::floor(std::clamp(minX, 0.0, static_cast<double>(width))));
    const int yBegin = static_cast<int>(std::floor(std::clamp(minY, 0.0, static_cast<double>(height))));
    const int xEnd = static_cast<int>(std::ceil(std::clamp(maxX, 0.0, static_cast<double>(width))));
    const int yEnd = static_cast<int>(std::ceil(std::clamp(maxY, 0.0, static_cast<double>(height))));
    if (xBegin >= xEnd || yBegin >= yEnd) return;

    const RGB c0 = UnpackRGB(v0.color);
    const RGB c1 = UnpackRGB(v1.color);
    const RGB c2 = UnpackRGB(v2.color);
    std::uint32_t* const pixels = framebuffer_.Data();

    // Independent pixel evaluations; future tiling/SIMD can retain this setup.
    for (int y = yBegin; y < yEnd; ++y) {
        const std::size_t row = static_cast<std::size_t>(y) * static_cast<std::size_t>(width);
        for (int x = xBegin; x < xEnd; ++x) {
            const math::Vec2 p{static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f};
            const float e0 = EdgeFunction(v1.position, v2.position, p);
            const float e1 = EdgeFunction(v2.position, v0.position, p);
            const float e2 = EdgeFunction(v0.position, v1.position, p);
            if (!std::isfinite(e0) || !std::isfinite(e1) || !std::isfinite(e2)) continue;
            if (!IsCovered(e0, e1, e2, signedArea)) continue;

            // Divide by SIGNED area: reversing winding flips numerator and denominator.
            const float w0 = e0 / signedArea;
            const float w1 = e1 / signedArea;
            const float w2 = e2 / signedArea;
            if (!std::isfinite(w0) || !std::isfinite(w1) || !std::isfinite(w2)) continue;
            // z_ndc, unlike view-space z, is affine in screen barycentrics.
            // Do NOT divide this interpolation by interpolated reciprocal w.
            const float z = w0 * v0.depth + w1 * v1.depth + w2 * v2.depth;
            const std::size_t index = row + static_cast<std::size_t>(x);
            if (!depth_.TestAndWrite(index, z)) continue;
            const std::uint32_t color = PackRGB(
                w0 * c0.r + w1 * c1.r + w2 * c2.r,
                w0 * c0.g + w1 * c1.g + w2 * c2.g,
                w0 * c0.b + w1 * c1.b + w2 * c2.b);
            pixels[index] = color;
        }
    }
}
```

## Game_/src/client/main.cpp

```cpp
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
```

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
project(3_year)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(game_math INTERFACE)
target_include_directories(game_math INTERFACE Game_/src)
target_compile_features(game_math INTERFACE cxx_std_20)

if(WIN32)

add_executable(3_year
        Game_/src/client/main.cpp
        Game_/src/client/render/renderer.cpp
        Game_/src/client/render/renderer.h
        Game_/src/client/render/depth_buffer.cpp
        Game_/src/client/render/depth_buffer.h
        Game_/src/client/render/projection.cpp
        Game_/src/client/render/projection.h
        Game_/src/client/platform/window.cpp
        Game_/src/client/platform/window.h
        Game_/src/client/platform/framebuffer.h
        Game_/src/client/platform/framebuffer.cpp
        Game_/src/client/platform/input.h
        Game_/src/client/platform/input.cpp
        Game_/src/shared/time/timer.h
        Game_/src/shared/time/timer.cpp
        Game_/src/shared/net/socket.h
        Game_/src/shared/net/protocol.h
        Game_/src/shared/serialization/byte_stream.h
        Game_/src/server/app/server_main.cpp
)

target_include_directories(3_year PRIVATE Game_/src)
target_compile_definitions(3_year PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(3_year PRIVATE game_math)
endif()

include(CTest)
if(BUILD_TESTING)
    foreach(mode IN ITEMS debug release)
        add_executable(math_tests_${mode} Game_/tests/math_tests.cpp)
        target_link_libraries(math_tests_${mode} PRIVATE game_math)
        if(mode STREQUAL "release")
            target_compile_definitions(math_tests_${mode} PRIVATE NDEBUG)
        endif()
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(math_tests_${mode} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            if(mode STREQUAL "debug")
                target_compile_options(math_tests_${mode} PRIVATE -UNDEBUG)
            else()
                target_compile_options(math_tests_${mode} PRIVATE -O2)
            endif()
        elseif(MSVC)
            target_compile_options(math_tests_${mode} PRIVATE /W4 /WX)
            if(mode STREQUAL "debug")
                target_compile_options(math_tests_${mode} PRIVATE /UNDEBUG)
            endif()
        endif()
        add_test(NAME math_${mode} COMMAND math_tests_${mode})
        if(WIN32)
            foreach(render_test IN ITEMS rasterizer pipeline)
            add_executable(${render_test}_tests_${mode}
                Game_/tests/${render_test}_tests.cpp
                Game_/src/client/render/renderer.cpp
                Game_/src/client/render/depth_buffer.cpp
                Game_/src/client/render/projection.cpp
                Game_/src/client/platform/framebuffer.cpp)
            target_link_libraries(${render_test}_tests_${mode} PRIVATE game_math)
            target_compile_definitions(${render_test}_tests_${mode} PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
            if(mode STREQUAL "release")
                target_compile_definitions(${render_test}_tests_${mode} PRIVATE NDEBUG)
            endif()
            # Match the math test targets' strict warnings and assertion mode.
            get_target_property(test_options math_tests_${mode} COMPILE_OPTIONS)
            if(test_options)
                target_compile_options(${render_test}_tests_${mode} PRIVATE ${test_options})
            endif()
            add_test(NAME ${render_test}_${mode} COMMAND ${render_test}_tests_${mode})
            endforeach()
        endif()
    endforeach()
endif()
```


