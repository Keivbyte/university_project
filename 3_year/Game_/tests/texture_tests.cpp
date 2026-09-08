#include "client/render/renderer.h"
#include "client/render/projection.h"
#include "client/platform/framebuffer.h"

#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
int checks = 0;
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
std::vector<float> Depths(const Renderer& renderer) {
    const auto& d = renderer.Depth();
    return {d.Data(), d.Data() + static_cast<std::size_t>(d.Width()) * static_cast<std::size_t>(d.Height())};
}
void Dump(const Framebuffer& fb, const std::string& path) {
    std::ofstream output(path, std::ios::binary);
    output << "P6\n" << fb.Width() << ' ' << fb.Height() << "\n255\n";
    for (const auto color : Pixels(fb)) {
        const char bytes[]{static_cast<char>((color >> 16u) & 255u),
                           static_cast<char>((color >> 8u) & 255u),
                           static_cast<char>(color & 255u)};
        output.write(bytes, 3);
    }
    CHECK(output.good());
}
} // namespace

int main(int argc, char** argv) {
    const Texture corners(2, 2, {0x12FF0000u, 0x3400FF00u, 0x560000FFu, 0x78FFFFFFu});
    CHECK(corners.Sample(0, 0, WrapMode::Clamp) == 0x12FF0000u);
    CHECK(corners.Sample(1, 0, WrapMode::Clamp) == 0x3400FF00u);
    CHECK(corners.Sample(0, 1, WrapMode::Clamp) == 0x560000FFu);
    CHECK(corners.Sample(1, 1, WrapMode::Clamp) == 0x78FFFFFFu);
    CHECK(corners.Sample(1, 1, WrapMode::Repeat) == 0x12FF0000u);
    CHECK(corners.Sample(-1, -2, WrapMode::Repeat) == 0x12FF0000u);
    CHECK(corners.Sample(-0.25f, -0.25f, WrapMode::Repeat) == 0x78FFFFFFu);
    CHECK(corners.Sample(1.75f, 2.75f, WrapMode::Repeat) == 0x78FFFFFFu);
    CHECK(corners.Sample(-20, 20, WrapMode::Clamp) == 0x560000FFu);
    CHECK(corners.Sample(0.49f, 0, WrapMode::Clamp) == 0x12FF0000u);
    CHECK(corners.Sample(0.5f, 0, WrapMode::Clamp) == 0x3400FF00u);
    const float huge = std::numeric_limits<float>::max();
    CHECK(corners.Sample(huge, -huge, WrapMode::Clamp) == 0x3400FF00u);
    CHECK(corners.Sample(huge, -huge, WrapMode::Repeat) == 0x12FF0000u);
    CHECK(corners.Sample(std::numeric_limits<float>::quiet_NaN(), 0, WrapMode::Repeat) == 0xFFFF00FFu);
    const Texture single(1, 1, {0x12345678u});
    CHECK(single.Sample(-8, 15, WrapMode::Repeat) == 0x12345678u);
    const Texture row(3, 1, {1, 2, 3});
    CHECK(row.Sample(0.5f, 1, WrapMode::Clamp) == 2);
    bool threw = false;
    try { (void)Texture(0, 2, {}); } catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);
    threw = false;
    try { (void)Texture(2, 2, {1}); } catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);
    threw = false;
    try { (void)Texture::GenerateCheckerboard(-1); } catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);
    const auto checker = Texture::GenerateCheckerboard(64, {0xFFEEEEEEu, 0xFF111111u});
    CHECK(checker.Sample(0, 0, WrapMode::Clamp) == 0xFFEEEEEEu);
    CHECK(checker.Sample(0.14f, 0, WrapMode::Clamp) == 0xFF111111u);
    CHECK(checker.Sample(0.14f, 0.14f, WrapMode::Clamp) == 0xFFEEEEEEu);
    CHECK(Texture::GenerateCheckerboard(1).Width() == 1);
    const auto pattern = Texture::GenerateAsymmetricPattern(64);
    CHECK(pattern.Sample(0.03f, 0.03f, WrapMode::Clamp) == 0xFFE04030u);
    CHECK(pattern.Sample(0.97f, 0.03f, WrapMode::Clamp) == 0xFFF0E8D0u);
    CHECK(pattern.Sample(0.25f, 0.75f, WrapMode::Clamp) == 0xFF202030u);
    CHECK(pattern.Sample(0.75f, 0.75f, WrapMode::Clamp) == 0xFFF0E8D0u);
    CHECK(pattern.Sample(0.75f, 0.20f, WrapMode::Clamp) == 0xFF202030u);

    // Encode integer texel coordinates directly into R and G; alpha must be copied too.
    std::vector<std::uint32_t> coordinates;
    for (std::uint32_t y = 0; y < 64; ++y)
        for (std::uint32_t x = 0; x < 64; ++x) coordinates.push_back(0x7F000000u | (x << 16u) | (y << 8u));
    const Texture lookup(64, 64, std::move(coordinates));
    Framebuffer fb(8, 8);
    Renderer renderer(fb);
    const auto clear = [&] { fb.Clear(0xFF182030u); renderer.ClearDepth(); };
    DrawParameters parameters{&lookup, WrapMode::Clamp, false};
    ScreenVertex a{{0, 0}, 0xFF000000u, 0.2f, {0, 0}, {0, 0}, 1};
    ScreenVertex b{{0, 4}, 0xFF000000u, 0.6f, {0, 1}, {0, 0.5f}, 0.5f};
    ScreenVertex c{{4, 0}, 0xFF000000u, 1, {1, 0}, {0.25f, 0}, 0.25f};
    clear();
    renderer.DrawTriangle(a, b, c, parameters);
    CHECK(fb.GetPixel(0, 0) == 0x7F080800u); // affine UV=(.125,.125), round(*63)=(8,8)
    const auto affineDepth = Depths(renderer);
    parameters.perspective_correct = true;
    clear();
    renderer.DrawTriangle(a, b, c, parameters);
    CHECK(fb.GetPixel(0, 0) == 0x7F020500u); // correct UV=(1/27,2/27), texel=(2,5)
    CHECK(Depths(renderer) == affineDepth); // entire depth buffer is bit-for-bit unchanged
    // Invalid reciprocal w cannot produce NaN pixels or write invisible depth.
    clear();
    a.inv_w = 0;
    renderer.DrawTriangle(a, b, c, parameters);
    CHECK(renderer.Depth().Data()[0] == 1 && fb.GetPixel(0, 0) == 0xFF182030u);
    parameters.perspective_correct = false;
    renderer.DrawTriangle(a, b, c, parameters); // affine ignores reciprocal w
    CHECK(fb.GetPixel(0, 0) == 0x7F080800u);
    a.inv_w = b.inv_w = c.inv_w = 1;
    a.uv_over_w = a.uv; b.uv_over_w = b.uv; c.uv_over_w = c.uv;
    clear();
    renderer.DrawTriangle(a, b, c, parameters);
    const auto equalW = Pixels(fb);
    parameters.perspective_correct = true;
    clear();
    renderer.DrawTriangle(a, b, c, parameters);
    CHECK(Pixels(fb) == equalW);

    // Full pipeline must prepare u/w,v/w,1/w rather than passing raw UV twice.
    const auto projection = Perspective(math::HALF_PI, 1, 0.1f, 100);
    const auto identity = math::Mat4::identity();
    const std::array<MeshVertex, 3> mesh{{
        {{-1,1,-1},0xFF000000u,{0,0}}, {{-2,0,-2},0xFF000000u,{0,1}},
        {{0,4,-4},0xFF000000u,{1,0}}
    }};
    const std::array<std::uint32_t, 3> indices{0,1,2};
    clear();
    renderer.DrawMesh(mesh, indices, identity, identity, projection, parameters);
    CHECK(fb.GetPixel(0, 0) == 0x7F020500u);
    const auto meshDepth = Depths(renderer);
    parameters.perspective_correct = false;
    clear();
    renderer.DrawMesh(mesh, indices, identity, identity, projection, parameters);
    CHECK(fb.GetPixel(0, 0) == 0x7F080800u);
    CHECK(Depths(renderer) == meshDepth);
    // A farther textured primitive cannot overwrite the existing foreground.
    const auto foreground = Pixels(fb);
    renderer.DrawMesh(mesh, indices, math::Mat4::scale(2), identity, projection, parameters);
    CHECK(Pixels(fb) == foreground);

    // Visual artifacts: a tilted textured quad, showing the affine diagonal bend.
    fb.Resize(320, 200);
    const std::array<MeshVertex, 4> quad{{
        {{-1,-1,0},0xFFFFFFFFu,{0,1}}, {{1,-1,0},0xFFFFFFFFu,{1,1}},
        {{1,1,0},0xFFFFFFFFu,{1,0}}, {{-1,1,0},0xFFFFFFFFu,{0,0}}
    }};
    const std::array<std::uint32_t, 6> quadIndices{0,1,2,0,2,3};
    const auto view = math::Mat4::translation({0,0,-3});
    const auto displayProjection = Perspective(math::radians(60), 1.6f, 0.1f, 100);
    const auto model = math::Mat4::rotationY(0.9f) * math::Mat4::rotationX(-0.15f);
    parameters.texture = &checker;
    clear();
    renderer.DrawMesh(quad, quadIndices, model, view, displayProjection, parameters);
    const auto affinePixels = Pixels(fb);
    const auto displayDepth = Depths(renderer);
    if (argc > 1) Dump(fb, std::string(argv[1]) + "_affine.ppm");
    parameters.perspective_correct = true;
    clear();
    renderer.DrawMesh(quad, quadIndices, model, view, displayProjection, parameters);
    CHECK(Pixels(fb) != affinePixels);
    CHECK(Depths(renderer) == displayDepth);
    if (argc > 1) Dump(fb, std::string(argv[1]) + "_perspective.ppm");
    parameters.texture = &pattern;
    clear();
    renderer.DrawMesh(quad, quadIndices, model, view, displayProjection, parameters);
    if (argc > 1) Dump(fb, std::string(argv[1]) + "_pattern.ppm");
    std::cout << checks << " texture checks passed\n";
}
