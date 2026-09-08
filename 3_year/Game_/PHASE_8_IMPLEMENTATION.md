# Этап 8 — полный листинг текстурирования

Полные добавленные/изменённые файлы приведены ниже. Это снимок этапа;
основная версия кода находится в src.

## Контракты

- Texture хранит ARGB uint32_t в памяти, без загрузчиков изображений.
- UV (0,0) — верхний левый угол, U вправо, V вниз.
- Repeat: coord - floor(coord); Clamp: ограничение до [0,1].
  Затем nearest-neighbor: round(coord * (dimension - 1)).
  При Clamp координата 1 выбирает последний тексель, при Repeat — первый.
  Это явно выбранное соглашение: UV-концы соответствуют центрам крайних текселей.
- Нефинитные UV в Sample возвращают диагностический пурпурный цвет.
- Генераторы создают шахматку до 8x8 клеток и асимметричную букву F
  с красной отметкой в верхнем левом углу.
- DrawParameters содержит texture, wrap_mode, perspective_correct.
  При texture=nullptr сохраняется интерполяция цветов вершин этапа 7.
- MeshVertex хранит исходные UV; DrawMesh после clip-проверок вычисляет
  uv_over_w = uv / clip.w и inv_w = 1 / clip.w для ScreenVertex.
- Affine: uv = sum(barycentric_i * uv_i).
- Perspective-correct: uv = sum(barycentric_i * uv_over_w_i) /
  sum(barycentric_i * inv_w_i). Нулевой/невалидный знаменатель отбрасывается.
- Текстура полностью заменяет цвет вершины. Тексель копируется как ARGB,
  без освещения, смешивания цветов или альфа-блендинга.
- Z-буфер не менялся: та же линейная интерполяция z_ndc, тот же strict LESS.
  Проверка валидности UV происходит до записи глубины.
- Куб содержит 24 вершины: 8 геометрических углов с дублированием на UV-швах.
  Каждая из шести граней получает свой полный диапазон UV [0,1].

## Управление сценой

- T — переключение Affine / Perspective-correct; текущий режим показан в заголовке.
- F — переключение шахматки и рисунка F.
- Space — пауза вращения для сравнения режимов на одном положении куба.
- Escape — выход.

Сцена использует Clamp. Repeat также реализован и покрыт тестами.
Генерация текстур выполняется один раз до игрового цикла.

## Проверка

MinGW GCC 14.2: сборка клиента успешна, все восемь CTest-целей проходят.
40 проверок текстурирования в каждом режиме Debug/Release, включая точные
ожидаемые UV/тексели, отрицательные UV и границы, передачу атрибутов через MVP,
копирование ARGB, защиту от нулевого inv_w и побитовое совпадение Z-буфера
между режимами. Существующие тесты математики, растеризации и пайплайна проходят.
Тестовые цели собираются со строгими предупреждениями и -Werror.
Сохранённые кадры показывают аффинный излом шахматки и правильную ориентацию F.

```sh
cmake --build build-math
ctest --test-dir build-math --output-on-failure
```

Запуск сцены: build-math/3_year.exe.

## Game_/src/client/render/texture.h

```cpp
#pragma once

#include <array>
#include <cstdint>
#include <vector>

enum class WrapMode { Repeat, Clamp };

class Texture {
public:
    // In-memory texels only. Positive dimensions and exactly width*height texels.
    Texture(int width, int height, std::vector<std::uint32_t> texels);
    int Width() const noexcept { return width_; }
    int Height() const noexcept { return height_; }

    // UV origin is TOP-LEFT; U right, V down. ARGB copied without conversion.
    // Wrap first, then nearest: round(uv * (dimension-1)). Clamp endpoints
    // select the first/last texel; Repeat maps integral UV (including 1) to 0.
    // Nonfinite UV returns diagnostic magenta, never an invalid array index.
    std::uint32_t Sample(float u, float v, WrapMode mode) const noexcept;

    static Texture GenerateCheckerboard(int size,
        std::array<std::uint32_t, 2> colors = {0xFFF0F0F0u, 0xFF202030u});
    static Texture GenerateAsymmetricPattern(int size);

private:
    int width_;
    int height_;
    std::vector<std::uint32_t> texels_;
};
```

## Game_/src/client/render/texture.cpp

```cpp
#include "texture.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace {
std::size_t TexelCount(int width, int height) {
    if (width <= 0 || height <= 0) throw std::invalid_argument("Texture dimensions must be positive");
    const auto w = static_cast<std::size_t>(width);
    const auto h = static_cast<std::size_t>(height);
    if (w > std::vector<std::uint32_t>{}.max_size() / h)
        throw std::length_error("Texture is too large");
    return w * h;
}

int NearestIndex(float coordinate, int dimension, WrapMode mode) noexcept {
    // Double avoids overflow or an out-of-range int conversion at large dimensions.
    double wrapped = static_cast<double>(coordinate);
    if (mode == WrapMode::Repeat) wrapped -= std::floor(wrapped);
    else wrapped = std::clamp(wrapped, 0.0, 1.0);
    return static_cast<int>(std::floor(wrapped * static_cast<double>(dimension - 1) + 0.5));
}
} // namespace

Texture::Texture(int width, int height, std::vector<std::uint32_t> texels)
    : width_(width), height_(height), texels_(std::move(texels)) {
    if (texels_.size() != TexelCount(width, height))
        throw std::invalid_argument("Texture texel count does not match dimensions");
}

std::uint32_t Texture::Sample(float u, float v, WrapMode mode) const noexcept {
    if (!std::isfinite(u) || !std::isfinite(v) || texels_.empty()) return 0xFFFF00FFu;
    const int x = NearestIndex(u, width_, mode);
    const int y = NearestIndex(v, height_, mode);
    return texels_[static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
                   static_cast<std::size_t>(x)];
}

Texture Texture::GenerateCheckerboard(int size, std::array<std::uint32_t, 2> colors) {
    std::vector<std::uint32_t> pixels(TexelCount(size, size));
    const auto dimension = static_cast<std::size_t>(size);
    const auto cells = static_cast<std::size_t>(std::min(size, 8));
    for (std::size_t y = 0; y < dimension; ++y)
        for (std::size_t x = 0; x < dimension; ++x)
            pixels[y * dimension + x] = colors[((x * cells / dimension) +
                                               (y * cells / dimension)) % 2];
    return Texture(size, size, std::move(pixels));
}

Texture Texture::GenerateAsymmetricPattern(int size) {
    std::vector<std::uint32_t> pixels(TexelCount(size, size), 0xFFF0E8D0u);
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const float u = (static_cast<float>(x) + 0.5f) / static_cast<float>(size);
            const float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(size);
            const bool stem = u >= 0.20f && u < 0.35f && v >= 0.15f && v < 0.85f;
            const bool top = u >= 0.20f && u < 0.80f && v >= 0.15f && v < 0.30f;
            const bool middle = u >= 0.20f && u < 0.65f && v >= 0.43f && v < 0.57f;
            auto& pixel = pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(size) +
                                 static_cast<std::size_t>(x)];
            if (stem || top || middle) pixel = 0xFF202030u;
            // Unique top-left marker makes both UV-axis inversions visible.
            if (u < 0.10f && v < 0.10f) pixel = 0xFFE04030u;
        }
    }
    return Texture(size, size, std::move(pixels));
}
```

## Game_/src/client/render/renderer.h

```cpp
#pragma once

#include <cstdint>
#include <span>
#include "shared/math/mat.h"
#include "depth_buffer.h"
#include "texture.h"

class Framebuffer;

// Coordinates are already in framebuffer pixels: +X right, +Y down.
struct ScreenVertex {
    math::Vec2 position{};
    std::uint32_t color = 0xFFFFFFFFu; // 0xAARRGGBB
    float depth = 0.0f; // post-divide NDC z, affine in screen space
    math::Vec2 uv{}; // raw UV for affine mode
    math::Vec2 uv_over_w{};
    float inv_w = 1.0f; // reciprocal clip w; NOT the depth buffer value
};

struct MeshVertex {
    math::Vec3 position{};
    std::uint32_t color = 0xFFFFFFFFu;
    math::Vec2 uv{};
};

struct DrawParameters {
    const Texture* texture = nullptr; // null preserves the vertex-color path
    WrapMode wrap_mode = WrapMode::Repeat;
    bool perspective_correct = true;
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
                  const math::Mat4& view, const math::Mat4& projection,
                  const DrawParameters& parameters = {});

    // E(P) = (Px-Ax)(By-Ay) - (Py-Ay)(Bx-Ax).
    static constexpr float EdgeFunction(const math::Vec2& a, const math::Vec2& b,
                                        const math::Vec2& p) noexcept {
        return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
    }

    // NDC depth and strict LESS are unchanged. A texture replaces vertex color,
    // copying its full ARGB texel; without a texture, RGB is affine and alpha=255.
    // Direct screen callers must provide uv_over_w=uv*inv_w for corrected mode.
    // Visually CCW is the front face in this Y-down buffer; CW is culled.
    // All edges are inclusive for now: shared-edge pixels can be drawn twice.
    void DrawTriangle(const ScreenVertex& v0, const ScreenVertex& v1,
                      const ScreenVertex& v2, const DrawParameters& parameters = {});

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
                        const math::Mat4& view, const math::Mat4& projection,
                        const DrawParameters& parameters) {
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
            const auto& vertex = vertices[indices[i + j]];
            const float inv_w = 1.0f / c.w;
            screen[j] = {{(ndc.x + 1.0f) * 0.5f * width,
                          (1.0f - ndc.y) * 0.5f * height},
                         vertex.color, ndc.z, vertex.uv, vertex.uv * inv_w, inv_w};
        }
        // DrawTriangle performs screen-space CCW culling before its AABB loop.
        DrawTriangle(screen[0], screen[1], screen[2], parameters);
    }
}

void Renderer::DrawTriangle(const ScreenVertex& v0, const ScreenVertex& v1,
                            const ScreenVertex& v2, const DrawParameters& parameters) {
    SyncDepthSize();
    const int width = framebuffer_.Width();
    const int height = framebuffer_.Height();
    if (width <= 0 || height <= 0) return;
    if (!IsFinite(v0.position) || !IsFinite(v1.position) || !IsFinite(v2.position)) return;
    if (parameters.texture) {
        for (const ScreenVertex* vertex : {&v0, &v1, &v2}) {
            if (!IsFinite(vertex->uv)) return;
            if (parameters.perspective_correct &&
                (!IsFinite(vertex->uv_over_w) || !std::isfinite(vertex->inv_w) || vertex->inv_w <= 0.0f))
                return;
        }
    }

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
            math::Vec2 uv;
            if (parameters.texture) {
                if (parameters.perspective_correct) {
                    const float inv_w = w0 * v0.inv_w + w1 * v1.inv_w + w2 * v2.inv_w;
                    if (!std::isfinite(inv_w) || inv_w <= 0.0f) continue;
                    const math::Vec2 uv_over_w = w0 * v0.uv_over_w + w1 * v1.uv_over_w + w2 * v2.uv_over_w;
                    uv = uv_over_w / inv_w;
                } else {
                    // Deliberately affine: raw UV, completely independent of clip w.
                    uv = w0 * v0.uv + w1 * v1.uv + w2 * v2.uv;
                }
                // Reject invalid attributes before depth write; no invisible occluders.
                if (!IsFinite(uv)) continue;
            }
            // z_ndc, unlike view-space z, is affine in screen barycentrics.
            // Do NOT divide this interpolation by interpolated reciprocal w.
            const float z = w0 * v0.depth + w1 * v1.depth + w2 * v2.depth;
            const std::size_t index = row + static_cast<std::size_t>(x);
            if (!depth_.TestAndWrite(index, z)) continue;
            const std::uint32_t color = parameters.texture
                ? parameters.texture->Sample(uv.x, uv.y, parameters.wrap_mode)
                : PackRGB(
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
        Game_/src/client/render/texture.h
        Game_/src/client/render/texture.cpp
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
            foreach(render_test IN ITEMS rasterizer pipeline texture)
            add_executable(${render_test}_tests_${mode}
                Game_/tests/${render_test}_tests.cpp
                Game_/src/client/render/renderer.cpp
                Game_/src/client/render/depth_buffer.cpp
                Game_/src/client/render/projection.cpp
                Game_/src/client/render/texture.cpp
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


