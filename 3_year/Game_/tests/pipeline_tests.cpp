#include "client/render/renderer.h"
#include "client/render/projection.h"
#include "client/platform/framebuffer.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
int checks = 0;
constexpr std::uint32_t BG = 0xFF182030u;
void Check(bool value, const char* expression, int line) {
    ++checks;
    if (!value) {
        std::cerr << "FAIL " << line << ": " << expression << '\n';
        std::exit(EXIT_FAILURE);
    }
}
#define CHECK(...) Check((__VA_ARGS__), #__VA_ARGS__, __LINE__)
std::vector<std::uint32_t> Pixels(const Framebuffer& fb) {
    return {fb.Data(), fb.Data() + static_cast<std::size_t>(fb.Width()) *
                                 static_cast<std::size_t>(fb.Height())};
}
bool Empty(const Framebuffer& fb) {
    const auto pixels = Pixels(fb);
    return std::all_of(pixels.begin(), pixels.end(), [](auto c) { return c == BG; });
}
void DumpPPM(const Framebuffer& fb, const char* path) {
    std::ofstream output(path, std::ios::binary);
    output << "P6\n" << fb.Width() << ' ' << fb.Height() << "\n255\n";
    for (const auto color : Pixels(fb)) {
        const char rgb[]{static_cast<char>((color >> 16u) & 255u),
                         static_cast<char>((color >> 8u) & 255u),
                         static_cast<char>(color & 255u)};
        output.write(rgb, 3);
    }
    CHECK(output.good());
}
} // namespace

int main(int argc, char** argv) {
    const auto identity = math::Mat4::identity();
    const auto projection = Perspective(math::HALF_PI, 1.0f, 0.1f, 100.0f);
    const auto nearClip = projection * math::Vec4{0, 0, -0.1f, 1};
    const auto farClip = projection * math::Vec4{0, 0, -100, 1};
    CHECK(math::nearlyEqual(nearClip.z / nearClip.w, 0));
    CHECK(math::nearlyEqual(farClip.z / farClip.w, 1));
    CHECK(math::nearlyEqual((projection * math::Vec4{1, 0, -1, 1}).x, 1));
    CHECK(math::nearlyEqual(Perspective(math::HALF_PI, 2, 0.1f, 100)(0, 0), 0.5f));
    bool threw = false;
    try { (void)Perspective(math::HALF_PI, 0, 0.1f, 100); }
    catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);

    Framebuffer fb(8, 8);
    Renderer renderer(fb);
    const auto clear = [&] { fb.Clear(BG); renderer.ClearDepth(); };
    const auto flat = [&](float z, std::uint32_t color) {
        renderer.DrawTriangle({{0, 0}, color, z}, {{0, 8}, color, z}, {{8, 0}, color, z});
    };
    clear();
    CHECK(renderer.Depth().Data()[0] == 1);
    flat(0.8f, 0xFFFF0000u);
    flat(0.2f, 0xFF00FF00u);
    const auto nearFirst = Pixels(fb);
    CHECK(fb.GetPixel(1, 1) == 0xFF00FF00u);
    CHECK(math::nearlyEqual(renderer.Depth().Data()[9], 0.2f));
    clear();
    flat(0.2f, 0xFF00FF00u);
    flat(0.8f, 0xFFFF0000u);
    CHECK(Pixels(fb) == nearFirst);
    flat(0.2f, 0xFF0000FFu);
    CHECK(Pixels(fb) == nearFirst); // strict LESS preserves the first equal-depth surface
    clear();
    flat(1, 0xFFFFFFFFu);
    flat(-0.1f, 0xFFFFFFFFu);
    flat(1.1f, 0xFFFFFFFFu);
    flat(std::numeric_limits<float>::quiet_NaN(), 0xFFFFFFFFu);
    CHECK(Empty(fb));
    flat(0, 0xFFFFFFFFu);
    CHECK(fb.GetPixel(0, 0) == 0xFFFFFFFFu);
    clear();
    renderer.ClearDepth(0.3f);
    flat(0.4f, 0xFFFFFFFFu);
    CHECK(Empty(fb));

    // Analytic screen barycentrics at (0.5,0.5): .75, .125, .125.
    clear();
    renderer.DrawTriangle({{0, 0}, 0xFFFF0000u, 0.2f},
                          {{0, 4}, 0xFF0000FFu, 0.6f},
                          {{4, 0}, 0xFF00FF00u, 1.0f});
    CHECK(math::nearlyEqual(renderer.Depth().Data()[0], 0.35f));
    CHECK(fb.GetPixel(0, 0) == 0xFFBF2020u);
    clear();
    renderer.DrawTriangle({{0, 0}}, {{4, 0}}, {{0, 4}}); // screen CW
    CHECK(Empty(fb));

    // NDC Y-up maps to the TOP half of the framebuffer; CCW survives inversion.
    const std::array<MeshVertex, 3> triangle{{
        {{-0.75f, 0.25f, 0.5f}}, {{0.75f, 0.25f, 0.5f}}, {{0, 0.75f, 0.5f}}
    }};
    const std::array<std::uint32_t, 3> indices{0, 1, 2}, reversed{0, 2, 1};
    clear();
    renderer.DrawMesh(triangle, indices, identity, identity, identity);
    CHECK(fb.GetPixel(3, 2) == 0xFFFFFFFFu);
    CHECK(fb.GetPixel(3, 5) == BG);
    clear();
    renderer.DrawMesh(triangle, reversed, identity, identity, identity);
    CHECK(Empty(fb));

    // Noncommuting model/view verifies P * V * M via independently known screen result.
    clear();
    renderer.DrawMesh(triangle, indices, math::Mat4::translation({0.25f, 0, 0}),
                      math::Mat4::scale({0.5f, 1, 1}), identity);
    const auto transformed = Pixels(fb);
    clear();
    renderer.DrawTriangle({{3, 3}, 0xFFFFFFFFu, 0.5f},
                          {{6, 3}, 0xFFFFFFFFu, 0.5f},
                          {{4.5f, 1}, 0xFFFFFFFFu, 0.5f});
    CHECK(Pixels(fb) == transformed);

    // Varying clip w: projected positions are the same, but per-vertex z/w differs.
    auto perspectiveTriangle = triangle;
    const float distances[]{1, 2, 4};
    for (std::size_t i = 0; i < 3; ++i) {
        perspectiveTriangle[i].position.x *= distances[i];
        perspectiveTriangle[i].position.y *= distances[i];
        perspectiveTriangle[i].position.z = -distances[i];
    }
    clear();
    renderer.DrawMesh(perspectiveTriangle, indices, identity, identity, projection);
    CHECK(fb.GetPixel(3, 2) == 0xFFFFFFFFu);
    // Screen weights at (3.5,2.5): 11/24, 7/24, 1/4.
    const auto projectZ = [&](float d) {
        const auto c = projection * math::Vec4{0, 0, -d, 1};
        return c.z / c.w;
    };
    const float expectedDepth = (11.0f / 24.0f) * projectZ(1) +
                                (7.0f / 24.0f) * projectZ(2) + 0.25f * projectZ(4);
    CHECK(math::nearlyEqual(renderer.Depth().Data()[19], expectedDepth));
    // Any ONE bad vertex must discard the entire triangle, before division.
    for (const float badZ : {0.0f, 1.0f, -0.05f}) {
        auto crossing = perspectiveTriangle;
        crossing[0].position.z = badZ;
        clear();
        renderer.DrawMesh(crossing, indices, identity, identity, projection);
        CHECK(Empty(fb));
    }
    auto beyondFar = perspectiveTriangle;
    for (auto& vertex : beyondFar) vertex.position.z = -200;
    clear();
    renderer.DrawMesh(beyondFar, indices, identity, identity, projection);
    CHECK(Empty(fb));
    threw = false;
    try { renderer.DrawMesh(triangle, std::array<std::uint32_t, 3>{0, 1, 9}, identity, identity, identity); }
    catch (const std::out_of_range&) { threw = true; }
    CHECK(threw);
    threw = false;
    try { renderer.DrawMesh(triangle, std::array<std::uint32_t, 2>{0, 1}, identity, identity, identity); }
    catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);
    fb.Resize(3, 5);
    renderer.ClearDepth();
    CHECK(renderer.Depth().Width() == 3 && renderer.Depth().Height() == 5);
    for (int i = 0; i < 15; ++i) CHECK(renderer.Depth().Data()[i] == 1);

    // Complete cube: all frames must contain pixels, and triangle submission order
    // must not change the image. Also validates the demo's outward face winding.
    const std::array<MeshVertex, 8> cube{{
        {{-1,-1,-1},0xFFFF4040u}, {{1,-1,-1},0xFF40FF40u},
        {{1,1,-1},0xFF4040FFu}, {{-1,1,-1},0xFFFFFF40u},
        {{-1,-1,1},0xFFFF40FFu}, {{1,-1,1},0xFF40FFFFu},
        {{1,1,1},0xFFFFFFFFu}, {{-1,1,1},0xFFFFA040u}
    }};
    const std::array<std::uint32_t, 36> cubeIndices{
        4,5,6,4,6,7, 1,0,3,1,3,2, 0,4,7,0,7,3,
        5,1,2,5,2,6, 3,7,6,3,6,2, 0,1,5,0,5,4
    };
    auto backToFront = cubeIndices;
    for (std::size_t i = 0; i < 12; ++i)
        for (std::size_t j = 0; j < 3; ++j)
            backToFront[i * 3 + j] = cubeIndices[(11 - i) * 3 + j];
    fb.Resize(320, 200);
    const auto cubeProjection = Perspective(math::radians(70), 1.6f, 0.1f, 100);
    const auto view = math::Mat4::translation({0, 0, -5});
    for (const float angle : {0.0f, 0.7f, 1.8f, 3.5f}) {
        const auto model = math::Mat4::rotationY(angle) * math::Mat4::rotationX(0.5f);
        clear();
        renderer.DrawMesh(cube, cubeIndices, model, view, cubeProjection);
        CHECK(!Empty(fb));
        const auto reference = Pixels(fb);
        clear();
        renderer.DrawMesh(cube, backToFront, model, view, cubeProjection);
        CHECK(reference == Pixels(fb));
        if (argc > 1 && angle == 0.7f) DumpPPM(fb, argv[1]);
    }
    std::cout << checks << " pipeline checks passed\n";
}
