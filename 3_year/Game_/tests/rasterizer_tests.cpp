#include "client/render/renderer.h"
#include "client/platform/framebuffer.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

namespace {
int checks = 0;
constexpr std::uint32_t BACKGROUND = 0xFF182030u;

void Check(bool passed, const char* expression, int line) {
    ++checks;
    if (!passed) {
        std::cerr << "FAIL line " << line << ": " << expression << '\n';
        std::exit(EXIT_FAILURE);
    }
}
#define CHECK(...) Check((__VA_ARGS__), #__VA_ARGS__, __LINE__)

std::vector<std::uint32_t> Snapshot(const Framebuffer& fb) {
    return {fb.Data(), fb.Data() + static_cast<std::size_t>(fb.Width()) *
                                 static_cast<std::size_t>(fb.Height())};
}

bool IsClear(const Framebuffer& fb) {
    const auto pixels = Snapshot(fb);
    return std::all_of(pixels.begin(), pixels.end(), [](auto c) { return c == BACKGROUND; });
}
} // namespace

static_assert(Renderer::EdgeFunction({0, 0}, {0, 4}, {4, 0}) == 16);
static_assert(Renderer::EdgeFunction({0, 0}, {4, 0}, {0, 4}) == -16);
static_assert(Renderer::EdgeFunction({0, 0}, {4, 0}, {2, 0}) == 0);

int main() {
    Framebuffer fb(8, 8);
    Renderer renderer(fb);

    // Exact center-sampled barycentrics at (0.5, 0.5): (0.75, 0.125, 0.125).
    // Distinct alpha bytes verify that RGB extraction doesn't mix channels/alpha.
    const std::array<ScreenVertex, 3> triangle{{
        {{0, 0}, 0x12FF0000u}, {{4, 0}, 0x3400FF00u}, {{0, 4}, 0x560000FFu}
    }};
    fb.Clear(BACKGROUND);
    renderer.DrawTriangle(triangle[0], triangle[1], triangle[2]);
    CHECK(fb.GetPixel(0, 0) == 0xFFBF2020u);
    CHECK(fb.GetPixel(1, 0) == 0xFF806020u);
    CHECK(fb.GetPixel(3, 0) == 0xFF00DF20u); // exactly on the hypotenuse
    CHECK(fb.GetPixel(4, 0) == BACKGROUND);
    // Analytic coverage mask: center x+y <= 4, i.e. pixel index x+y <= 3.
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 8; ++x)
            CHECK((fb.GetPixel(x, y) != BACKGROUND) == (x + y <= 3));
    const auto reference = Snapshot(fb);

    // All six permutations retain positions AND their attached colors.
    std::array<int, 3> order{0, 1, 2};
    do {
        fb.Clear(BACKGROUND);
        renderer.DrawTriangle(triangle[static_cast<std::size_t>(order[0])],
                              triangle[static_cast<std::size_t>(order[1])],
                              triangle[static_cast<std::size_t>(order[2])]);
        CHECK(Snapshot(fb) == reference);
    } while (std::next_permutation(order.begin(), order.end()));

    // Flat shading is the same path with identical colors; later draws win.
    renderer.DrawTriangle({{0, 0}, 0xFF123456u}, {{4, 0}, 0xFF123456u}, {{0, 4}, 0xFF123456u});
    CHECK(fb.GetPixel(0, 0) == 0xFF123456u);
    renderer.DrawTriangle(triangle[0], triangle[1], triangle[2]);
    CHECK(Snapshot(fb) == reference);

    // Collinear, repeated, near-zero area and nonfinite vertices leave pixels alone.
    fb.Clear(BACKGROUND);
    renderer.DrawTriangle({{0, 0}}, {{2, 2}}, {{4, 4}});
    renderer.DrawTriangle({{1, 1}}, {{1, 1}}, {{4, 4}});
    renderer.DrawTriangle({{0, 0}}, {{0.0001f, 0}}, {{0, 0.0001f}});
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();
    renderer.DrawTriangle({{nan, 0}}, {{4, 0}}, {{0, 4}});
    renderer.DrawTriangle({{0, 0}}, {{inf, 0}}, {{0, 4}});
    CHECK(IsClear(fb));

    // Fully offscreen on each side, including exact exclusive width/height bounds.
    renderer.DrawTriangle({{-4, 0}}, {{0, 0}}, {{-4, 4}});
    renderer.DrawTriangle({{8, 0}}, {{12, 0}}, {{8, 4}});
    renderer.DrawTriangle({{0, -4}}, {{4, -4}}, {{0, 0}});
    renderer.DrawTriangle({{0, 8}}, {{4, 8}}, {{0, 12}});
    CHECK(IsClear(fb));

    // Geometry in one pixel's corner must not cover that pixel's center.
    renderer.DrawTriangle({{0, 0}}, {{0.4f, 0}}, {{0, 0.4f}});
    CHECK(IsClear(fb));
    // Fractional vertices place the center exactly on a vertex/edge: inclusive now.
    renderer.DrawTriangle({{0.5f, 0.5f}}, {{1.5f, 0.5f}}, {{0.5f, 1.5f}});
    CHECK(fb.GetPixel(0, 0) == 0xFFFFFFFFu);
    CHECK(fb.GetPixel(1, 0) == 0xFFFFFFFFu);
    CHECK(fb.GetPixel(0, 1) == 0xFFFFFFFFu);
    CHECK(fb.GetPixel(1, 1) == BACKGROUND);

    // A triangle extending past all screen bounds covers even the final row/column.
    fb.Clear(BACKGROUND);
    renderer.DrawTriangle({{-100, -100}}, {{300, -100}}, {{-100, 300}});
    for (const auto pixel : Snapshot(fb)) CHECK(pixel == 0xFFFFFFFFu);

    // Very large, but finite area: bounds must be clamped before conversion to int.
    fb.Clear(BACKGROUND);
    renderer.DrawTriangle({{-1.0e10f, -1.0e10f}}, {{3.0e10f, -1.0e10f}}, {{-1.0e10f, 3.0e10f}});
    CHECK(fb.GetPixel(7, 7) == 0xFFFFFFFFu);
    fb.Clear(BACKGROUND);
    const float huge = std::numeric_limits<float>::max();
    renderer.DrawTriangle({{-huge, -huge}}, {{huge, -huge}}, {{0, huge}});
    CHECK(IsClear(fb)); // overflowing edge arithmetic is discarded

    // Two triangles tile the screen with no holes. Shared diagonal is inclusive.
    fb.Clear(BACKGROUND);
    renderer.DrawTriangle({{0, 0}}, {{8, 0}}, {{0, 8}});
    renderer.DrawTriangle({{8, 0}}, {{8, 8}}, {{0, 8}});
    for (const auto pixel : Snapshot(fb)) CHECK(pixel == 0xFFFFFFFFu);

    // Renderer must use current dimensions/data after framebuffer resize.
    fb.Resize(1, 1);
    fb.Clear(BACKGROUND);
    renderer.DrawTriangle({{-1, -1}}, {{3, -1}}, {{-1, 3}});
    CHECK(fb.GetPixel(0, 0) == 0xFFFFFFFFu);
    fb.Resize(0, 0);
    renderer.DrawTriangle(triangle[0], triangle[1], triangle[2]);
    CHECK(fb.Width() == 0 && fb.Height() == 0);

    std::cout << checks << " rasterizer checks passed\n";
}
