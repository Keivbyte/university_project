#include "shared/math/utils.h"
#include "shared/math/vec.h"
#include "shared/math/mat.h"
#include "shared/math/geometry.h"
#include <cstdlib>
#include <iostream>
#include <limits>
#include <type_traits>

using namespace math;

namespace {
int checks = 0;
void check(bool passed, const char* expression, int line) {
    ++checks;
    if (!passed) {
        std::cerr << "FAIL line " << line << ": " << expression << '\n';
        std::exit(EXIT_FAILURE);
    }
}
}
#define CHECK(...) check((__VA_ARGS__), #__VA_ARGS__, __LINE__)

static_assert(Vec2{} == Vec2{0, 0});
static_assert(Vec3{} == Vec3{0, 0, 0});
static_assert(Vec4{} == Vec4{0, 0, 0, 0});
static_assert(sizeof(Vec4) == 4 * sizeof(float));
static_assert(sizeof(Mat4) == 16 * sizeof(float));
static_assert(std::is_standard_layout_v<Vec3> && std::is_trivially_copyable_v<Vec3>);
static_assert(cross(Vec3{1, 0, 0}, Vec3{0, 1, 0}) == Vec3{0, 0, 1});
static_assert(Mat4::identity() * Vec4{1, 2, 3, 1} == Vec4{1, 2, 3, 1});
static_assert(Mat4::translation({1, 2, 3}).data[12] == 1.0f);
static_assert(AABB{{-1, -2, -3}, {1, 2, 3}}.extents() == Vec3{1, 2, 3});

int main(int argc, char**) {
    // Separate process mode for verifying the debug assertion contract.
    if (argc > 1) {
        (void)normalize(Vec3{});
        return EXIT_SUCCESS;
    }
    CHECK(nearlyEqual(radians(180), PI));
    CHECK(nearlyEqual(degrees(HALF_PI), 90));
    CHECK(clamp(-1, 0, 1) == 0 && clamp(2, 0, 1) == 1 && clamp(0.5f, 0, 1) == 0.5f);
    CHECK(lerp(2, 4, 0.5f) == 3 && lerp(2, 4, 2) == 6);
    CHECK(!nearlyEqual(std::numeric_limits<float>::infinity(), 1.0f));
    CHECK(!nearlyEqual(std::numeric_limits<float>::quiet_NaN(), 1.0f));
    {
        constexpr Vec2 a{1, 2}, b{2, 2};
        CHECK(nearlyEqual(a + b, Vec2{3, 4}));
        CHECK(nearlyEqual(a + 2.0f, Vec2{3, 4}));
        CHECK(nearlyEqual(2.0f + b, Vec2{4, 4}));
        CHECK(nearlyEqual(a - b, Vec2{-1, 0}));
        CHECK(nearlyEqual(a - 2.0f, Vec2{-1, 0}));
        CHECK(nearlyEqual(2.0f - b, Vec2{0, 0}));
        CHECK(nearlyEqual(a * b, Vec2{2, 4}));
        CHECK(nearlyEqual(a * 2.0f, Vec2{2, 4}));
        CHECK(nearlyEqual(2.0f * b, Vec2{4, 4}));
        CHECK(nearlyEqual(a / b, Vec2{0.5, 1}));
        CHECK(nearlyEqual(a / 2.0f, Vec2{0.5, 1}));
        CHECK(nearlyEqual(2.0f / b, Vec2{1, 1}));
        CHECK(-a == Vec2{-1, -2});
        CHECK(lengthSq(a) == 5);
        CHECK(nearlyEqual(length(normalize(a)), 1.0f));
        CHECK(nearlyEqual(distance(a, a), 0.0f));
        CHECK(nearlyEqual(lerp(a, b, 0), a) && nearlyEqual(lerp(a, b, 1), b));
        Vec2 indexed = a;
        for (int i = 0; i < 2; ++i) { CHECK(indexed[i] == a[i]); indexed[i] = 9; }
        CHECK(indexed == Vec2{9, 9});
        const float huge = std::numeric_limits<float>::max();
        CHECK(nearlyEqual(length(normalize(Vec2{huge, huge})), 1.0f));
        CHECK(nearlyEqual(length(normalize(Vec2{2.0e-6f, 2.0e-6f})), 1.0f));
#ifdef NDEBUG
        CHECK(normalize(Vec2{}) == Vec2{});
        CHECK(normalize(Vec2{1.0e-8f, 1.0e-8f}) == Vec2{});
        const float inf = std::numeric_limits<float>::infinity();
        CHECK(normalize(Vec2{inf, inf}) == Vec2{});
        const float nan = std::numeric_limits<float>::quiet_NaN();
        CHECK(normalize(Vec2{nan, nan}) == Vec2{});
#endif
    }
    {
        constexpr Vec3 a{1, 2, 3}, b{2, 2, 2};
        CHECK(nearlyEqual(a + b, Vec3{3, 4, 5}));
        CHECK(nearlyEqual(a + 2.0f, Vec3{3, 4, 5}));
        CHECK(nearlyEqual(2.0f + b, Vec3{4, 4, 4}));
        CHECK(nearlyEqual(a - b, Vec3{-1, 0, 1}));
        CHECK(nearlyEqual(a - 2.0f, Vec3{-1, 0, 1}));
        CHECK(nearlyEqual(2.0f - b, Vec3{0, 0, 0}));
        CHECK(nearlyEqual(a * b, Vec3{2, 4, 6}));
        CHECK(nearlyEqual(a * 2.0f, Vec3{2, 4, 6}));
        CHECK(nearlyEqual(2.0f * b, Vec3{4, 4, 4}));
        CHECK(nearlyEqual(a / b, Vec3{0.5, 1, 1.5}));
        CHECK(nearlyEqual(a / 2.0f, Vec3{0.5, 1, 1.5}));
        CHECK(nearlyEqual(2.0f / b, Vec3{1, 1, 1}));
        CHECK(-a == Vec3{-1, -2, -3});
        CHECK(lengthSq(a) == 14);
        CHECK(nearlyEqual(length(normalize(a)), 1.0f));
        CHECK(nearlyEqual(distance(a, a), 0.0f));
        CHECK(nearlyEqual(lerp(a, b, 0), a) && nearlyEqual(lerp(a, b, 1), b));
        Vec3 indexed = a;
        for (int i = 0; i < 3; ++i) { CHECK(indexed[i] == a[i]); indexed[i] = 9; }
        CHECK(indexed == Vec3{9, 9, 9});
        const float huge = std::numeric_limits<float>::max();
        CHECK(nearlyEqual(length(normalize(Vec3{huge, huge, huge})), 1.0f));
        CHECK(nearlyEqual(length(normalize(Vec3{2.0e-6f, 2.0e-6f, 2.0e-6f})), 1.0f));
#ifdef NDEBUG
        CHECK(normalize(Vec3{}) == Vec3{});
        CHECK(normalize(Vec3{1.0e-8f, 1.0e-8f, 1.0e-8f}) == Vec3{});
        const float inf = std::numeric_limits<float>::infinity();
        CHECK(normalize(Vec3{inf, inf, inf}) == Vec3{});
        const float nan = std::numeric_limits<float>::quiet_NaN();
        CHECK(normalize(Vec3{nan, nan, nan}) == Vec3{});
#endif
    }
    {
        constexpr Vec4 a{1, 2, 3, 4}, b{2, 2, 2, 2};
        CHECK(nearlyEqual(a + b, Vec4{3, 4, 5, 6}));
        CHECK(nearlyEqual(a + 2.0f, Vec4{3, 4, 5, 6}));
        CHECK(nearlyEqual(2.0f + b, Vec4{4, 4, 4, 4}));
        CHECK(nearlyEqual(a - b, Vec4{-1, 0, 1, 2}));
        CHECK(nearlyEqual(a - 2.0f, Vec4{-1, 0, 1, 2}));
        CHECK(nearlyEqual(2.0f - b, Vec4{0, 0, 0, 0}));
        CHECK(nearlyEqual(a * b, Vec4{2, 4, 6, 8}));
        CHECK(nearlyEqual(a * 2.0f, Vec4{2, 4, 6, 8}));
        CHECK(nearlyEqual(2.0f * b, Vec4{4, 4, 4, 4}));
        CHECK(nearlyEqual(a / b, Vec4{0.5, 1, 1.5, 2}));
        CHECK(nearlyEqual(a / 2.0f, Vec4{0.5, 1, 1.5, 2}));
        CHECK(nearlyEqual(2.0f / b, Vec4{1, 1, 1, 1}));
        CHECK(-a == Vec4{-1, -2, -3, -4});
        CHECK(lengthSq(a) == 30);
        CHECK(nearlyEqual(length(normalize(a)), 1.0f));
        CHECK(nearlyEqual(distance(a, a), 0.0f));
        CHECK(nearlyEqual(lerp(a, b, 0), a) && nearlyEqual(lerp(a, b, 1), b));
        Vec4 indexed = a;
        for (int i = 0; i < 4; ++i) { CHECK(indexed[i] == a[i]); indexed[i] = 9; }
        CHECK(indexed == Vec4{9, 9, 9, 9});
        const float huge = std::numeric_limits<float>::max();
        CHECK(nearlyEqual(length(normalize(Vec4{huge, huge, huge, huge})), 1.0f));
        CHECK(nearlyEqual(length(normalize(Vec4{2.0e-6f, 2.0e-6f, 2.0e-6f, 2.0e-6f})), 1.0f));
#ifdef NDEBUG
        CHECK(normalize(Vec4{}) == Vec4{});
        CHECK(normalize(Vec4{1.0e-8f, 1.0e-8f, 1.0e-8f, 1.0e-8f}) == Vec4{});
        const float inf = std::numeric_limits<float>::infinity();
        CHECK(normalize(Vec4{inf, inf, inf, inf}) == Vec4{});
        const float nan = std::numeric_limits<float>::quiet_NaN();
        CHECK(normalize(Vec4{nan, nan, nan, nan}) == Vec4{});
#endif
    }
    CHECK(dot(Vec3{1, 0, 0}, Vec3{0, 1, 0}) == 0);
    CHECK(cross(Vec3{0, 1, 0}, Vec3{1, 0, 0}) == Vec3{0, 0, -1});
    CHECK(length(Vec3{1, 0, 0}) == 1);
    const Vec4 point{1, 1, 1, 1}, direction{1, 1, 1, 0};
    const Mat4 t = Mat4::translation({10, 0, 0}), s = Mat4::scale(2);
    CHECK(t * s * point == Vec4{12, 2, 2, 1});
    CHECK(s * t * point == Vec4{22, 2, 2, 1});
    CHECK(t * direction == direction);
    CHECK(Mat4::scale({2, 3, 4}) * point == Vec4{2, 3, 4, 1});
    CHECK(Mat4{} * point == Vec4{});
    const Mat4 rx = Mat4::rotationX(HALF_PI), ry = Mat4::rotationY(HALF_PI);
    const Mat4 rz = Mat4::rotationZ(HALF_PI);
    CHECK(nearlyEqual(rx * Vec4{0, 1, 0, 0}, Vec4{0, 0, 1, 0}));
    CHECK(nearlyEqual(ry * Vec4{0, 0, 1, 0}, Vec4{1, 0, 0, 0}));
    CHECK(nearlyEqual(rz * Vec4{1, 0, 0, 0}, Vec4{0, 1, 0, 0}));
    CHECK(nearlyEqual(transpose(rx) * rx, Mat4::identity()));
    CHECK(nearlyEqual(transpose(ry) * ry, Mat4::identity()));
    CHECK(nearlyEqual(transpose(rz) * rz, Mat4::identity()));
    CHECK(nearlyEqual(rz * Mat4::rotationZ(-HALF_PI), Mat4::identity()));
    Mat4 arbitrary;
    for (int i = 0; i < 16; ++i) arbitrary.data[i] = static_cast<float>(i + 1);
    CHECK(transpose(transpose(arbitrary)) == arbitrary);
    CHECK(transpose(arbitrary)(1, 2) == arbitrary(2, 1));
    CHECK(arbitrary * Mat4::identity() == arbitrary);
    CHECK(nearlyEqual((t * rz) * s, t * (rz * s)));
    CHECK(nearlyEqual((t * rz) * point, t * (rz * point)));
    CHECK(arbitrary * Vec4{1, 2, 3, 4} == Vec4{90, 100, 110, 120});
    CHECK(nearlyEqual(transpose(t * arbitrary), transpose(arbitrary) * transpose(t)));

    AABB box{{-1, -2, -3}, {1, 2, 3}};
    CHECK(box.contains(box.min) && box.contains(box.max) && box.contains({0, 0, 0}));
    CHECK(!box.contains({1.01f, 0, 0}));
    CHECK(box.center() == Vec3{} && box.extents() == Vec3{1, 2, 3});
    box.merge(AABB{{-4, 1, -1}, {2, 5, 8}});
    CHECK(box.min == Vec3{-4, -2, -3} && box.max == Vec3{2, 5, 8});
    CHECK(AABB{}.contains({0, 0, 0}));
    CHECK(nearlyEqual(length(Ray{}.direction), 1));
    const Ray ray{{1, 2, 3}, normalize(Vec3{1, 2, 3})};
    CHECK(ray.origin == Vec3{1, 2, 3} && nearlyEqual(length(ray.direction), 1));
    const Plane plane{{0, 1, 0}, -2};
    CHECK(plane.signedDistance({0, 2, 0}) == 0);
    CHECK(plane.signedDistance({0, 3, 0}) == 1);
    CHECK(plane.signedDistance({0, 1, 0}) == -1);
    std::cout << checks << " checks passed\n";
}
