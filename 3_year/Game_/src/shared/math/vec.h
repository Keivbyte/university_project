#pragma once
#include <cassert>
#include <cmath>
#include "utils.h"

namespace math {
// Zero initialized float value types. Indexing requires an in-range index.
// Division requires nonzero divisors. Equality is exact, not approximate.

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
    constexpr Vec2() = default;
    constexpr Vec2(float xValue, float yValue)
        : x(xValue), y(yValue) {}
    constexpr float& operator[](int index) {
        assert(index >= 0 && index < 2);
        if (index == 0) return x;
        return y;
    }
    constexpr const float& operator[](int index) const {
        assert(index >= 0 && index < 2);
        if (index == 0) return x;
        return y;
    }
    constexpr Vec2 operator-() const { return {-x, -y}; }
    constexpr bool operator==(const Vec2& other) const = default;
    constexpr Vec2& operator+=(const Vec2& rhs) {
        x += rhs.x; y += rhs.y;
        return *this;
    }
    constexpr Vec2& operator+=(float rhs) {
        x += rhs; y += rhs;
        return *this;
    }
    constexpr Vec2& operator-=(const Vec2& rhs) {
        x -= rhs.x; y -= rhs.y;
        return *this;
    }
    constexpr Vec2& operator-=(float rhs) {
        x -= rhs; y -= rhs;
        return *this;
    }
    constexpr Vec2& operator*=(const Vec2& rhs) {
        x *= rhs.x; y *= rhs.y;
        return *this;
    }
    constexpr Vec2& operator*=(float rhs) {
        x *= rhs; y *= rhs;
        return *this;
    }
    constexpr Vec2& operator/=(const Vec2& rhs) {
        x /= rhs.x; y /= rhs.y;
        return *this;
    }
    constexpr Vec2& operator/=(float rhs) {
        x /= rhs; y /= rhs;
        return *this;
    }
};
constexpr Vec2 operator+(Vec2 lhs, const Vec2& rhs) { return lhs += rhs; }
constexpr Vec2 operator+(Vec2 lhs, float rhs) { return lhs += rhs; }
constexpr Vec2 operator+(float lhs, const Vec2& rhs) { return {lhs + rhs.x, lhs + rhs.y}; }
constexpr Vec2 operator-(Vec2 lhs, const Vec2& rhs) { return lhs -= rhs; }
constexpr Vec2 operator-(Vec2 lhs, float rhs) { return lhs -= rhs; }
constexpr Vec2 operator-(float lhs, const Vec2& rhs) { return {lhs - rhs.x, lhs - rhs.y}; }
constexpr Vec2 operator*(Vec2 lhs, const Vec2& rhs) { return lhs *= rhs; }
constexpr Vec2 operator*(Vec2 lhs, float rhs) { return lhs *= rhs; }
constexpr Vec2 operator*(float lhs, const Vec2& rhs) { return {lhs * rhs.x, lhs * rhs.y}; }
constexpr Vec2 operator/(Vec2 lhs, const Vec2& rhs) { return lhs /= rhs; }
constexpr Vec2 operator/(Vec2 lhs, float rhs) { return lhs /= rhs; }
constexpr Vec2 operator/(float lhs, const Vec2& rhs) { return {lhs / rhs.x, lhs / rhs.y}; }
constexpr float dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
constexpr float lengthSq(const Vec2& value) { return dot(value, value); }
inline float length(const Vec2& value) { return std::hypot(value.x, value.y); }
// Rescaling avoids intermediate overflow on large finite vectors.
// Invalid/near-zero input asserts in debug and returns zero with NDEBUG.
inline Vec2 normalize(const Vec2& value) {
    const bool finite = std::isfinite(value.x) && std::isfinite(value.y);
    assert(finite && "normalize requires finite components");
    if (!finite) return {};
    const float largest = max(std::fabs(value.x), std::fabs(value.y));
    if (largest == 0.0f) {
        assert(false && "cannot normalize a zero vector");
        return {};
    }
    const Vec2 scaled = value / largest;
    const float scaledLength = length(scaled);
    if (largest <= NORMALIZATION_EPSILON / scaledLength) {
        assert(false && "vector is too short to normalize");
        return {};
    }
    return scaled / scaledLength;
}
inline float distance(const Vec2& a, const Vec2& b) { return length(a - b); }
constexpr Vec2 lerp(const Vec2& a, const Vec2& b, float t) { return (1.0f - t) * a + t * b; }
inline bool nearlyEqual(const Vec2& a, const Vec2& b, float absTolerance = 1.0e-6f,
                        float relTolerance = 1.0e-5f) {
    return nearlyEqual(a.x, b.x, absTolerance, relTolerance) &&
           nearlyEqual(a.y, b.y, absTolerance, relTolerance);
}

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    constexpr Vec3() = default;
    constexpr Vec3(float xValue, float yValue, float zValue)
        : x(xValue), y(yValue), z(zValue) {}
    constexpr float& operator[](int index) {
        assert(index >= 0 && index < 3);
        if (index == 0) return x;
        if (index == 1) return y;
        return z;
    }
    constexpr const float& operator[](int index) const {
        assert(index >= 0 && index < 3);
        if (index == 0) return x;
        if (index == 1) return y;
        return z;
    }
    constexpr Vec3 operator-() const { return {-x, -y, -z}; }
    constexpr bool operator==(const Vec3& other) const = default;
    constexpr Vec3& operator+=(const Vec3& rhs) {
        x += rhs.x; y += rhs.y; z += rhs.z;
        return *this;
    }
    constexpr Vec3& operator+=(float rhs) {
        x += rhs; y += rhs; z += rhs;
        return *this;
    }
    constexpr Vec3& operator-=(const Vec3& rhs) {
        x -= rhs.x; y -= rhs.y; z -= rhs.z;
        return *this;
    }
    constexpr Vec3& operator-=(float rhs) {
        x -= rhs; y -= rhs; z -= rhs;
        return *this;
    }
    constexpr Vec3& operator*=(const Vec3& rhs) {
        x *= rhs.x; y *= rhs.y; z *= rhs.z;
        return *this;
    }
    constexpr Vec3& operator*=(float rhs) {
        x *= rhs; y *= rhs; z *= rhs;
        return *this;
    }
    constexpr Vec3& operator/=(const Vec3& rhs) {
        x /= rhs.x; y /= rhs.y; z /= rhs.z;
        return *this;
    }
    constexpr Vec3& operator/=(float rhs) {
        x /= rhs; y /= rhs; z /= rhs;
        return *this;
    }
};
constexpr Vec3 operator+(Vec3 lhs, const Vec3& rhs) { return lhs += rhs; }
constexpr Vec3 operator+(Vec3 lhs, float rhs) { return lhs += rhs; }
constexpr Vec3 operator+(float lhs, const Vec3& rhs) { return {lhs + rhs.x, lhs + rhs.y, lhs + rhs.z}; }
constexpr Vec3 operator-(Vec3 lhs, const Vec3& rhs) { return lhs -= rhs; }
constexpr Vec3 operator-(Vec3 lhs, float rhs) { return lhs -= rhs; }
constexpr Vec3 operator-(float lhs, const Vec3& rhs) { return {lhs - rhs.x, lhs - rhs.y, lhs - rhs.z}; }
constexpr Vec3 operator*(Vec3 lhs, const Vec3& rhs) { return lhs *= rhs; }
constexpr Vec3 operator*(Vec3 lhs, float rhs) { return lhs *= rhs; }
constexpr Vec3 operator*(float lhs, const Vec3& rhs) { return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z}; }
constexpr Vec3 operator/(Vec3 lhs, const Vec3& rhs) { return lhs /= rhs; }
constexpr Vec3 operator/(Vec3 lhs, float rhs) { return lhs /= rhs; }
constexpr Vec3 operator/(float lhs, const Vec3& rhs) { return {lhs / rhs.x, lhs / rhs.y, lhs / rhs.z}; }
constexpr float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
constexpr float lengthSq(const Vec3& value) { return dot(value, value); }
inline float length(const Vec3& value) { return std::hypot(value.x, value.y, value.z); }
// Rescaling avoids intermediate overflow on large finite vectors.
// Invalid/near-zero input asserts in debug and returns zero with NDEBUG.
inline Vec3 normalize(const Vec3& value) {
    const bool finite = std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
    assert(finite && "normalize requires finite components");
    if (!finite) return {};
    const float largest = max(max(std::fabs(value.x), std::fabs(value.y)), std::fabs(value.z));
    if (largest == 0.0f) {
        assert(false && "cannot normalize a zero vector");
        return {};
    }
    const Vec3 scaled = value / largest;
    const float scaledLength = length(scaled);
    if (largest <= NORMALIZATION_EPSILON / scaledLength) {
        assert(false && "vector is too short to normalize");
        return {};
    }
    return scaled / scaledLength;
}
inline float distance(const Vec3& a, const Vec3& b) { return length(a - b); }
constexpr Vec3 lerp(const Vec3& a, const Vec3& b, float t) { return (1.0f - t) * a + t * b; }
inline bool nearlyEqual(const Vec3& a, const Vec3& b, float absTolerance = 1.0e-6f,
                        float relTolerance = 1.0e-5f) {
    return nearlyEqual(a.x, b.x, absTolerance, relTolerance) &&
           nearlyEqual(a.y, b.y, absTolerance, relTolerance) &&
           nearlyEqual(a.z, b.z, absTolerance, relTolerance);
}

struct Vec4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;
    constexpr Vec4() = default;
    constexpr Vec4(float xValue, float yValue, float zValue, float wValue)
        : x(xValue), y(yValue), z(zValue), w(wValue) {}
    constexpr float& operator[](int index) {
        assert(index >= 0 && index < 4);
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;
        return w;
    }
    constexpr const float& operator[](int index) const {
        assert(index >= 0 && index < 4);
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;
        return w;
    }
    constexpr Vec4 operator-() const { return {-x, -y, -z, -w}; }
    constexpr bool operator==(const Vec4& other) const = default;
    constexpr Vec4& operator+=(const Vec4& rhs) {
        x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
        return *this;
    }
    constexpr Vec4& operator+=(float rhs) {
        x += rhs; y += rhs; z += rhs; w += rhs;
        return *this;
    }
    constexpr Vec4& operator-=(const Vec4& rhs) {
        x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w;
        return *this;
    }
    constexpr Vec4& operator-=(float rhs) {
        x -= rhs; y -= rhs; z -= rhs; w -= rhs;
        return *this;
    }
    constexpr Vec4& operator*=(const Vec4& rhs) {
        x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w;
        return *this;
    }
    constexpr Vec4& operator*=(float rhs) {
        x *= rhs; y *= rhs; z *= rhs; w *= rhs;
        return *this;
    }
    constexpr Vec4& operator/=(const Vec4& rhs) {
        x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w;
        return *this;
    }
    constexpr Vec4& operator/=(float rhs) {
        x /= rhs; y /= rhs; z /= rhs; w /= rhs;
        return *this;
    }
};
constexpr Vec4 operator+(Vec4 lhs, const Vec4& rhs) { return lhs += rhs; }
constexpr Vec4 operator+(Vec4 lhs, float rhs) { return lhs += rhs; }
constexpr Vec4 operator+(float lhs, const Vec4& rhs) { return {lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w}; }
constexpr Vec4 operator-(Vec4 lhs, const Vec4& rhs) { return lhs -= rhs; }
constexpr Vec4 operator-(Vec4 lhs, float rhs) { return lhs -= rhs; }
constexpr Vec4 operator-(float lhs, const Vec4& rhs) { return {lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w}; }
constexpr Vec4 operator*(Vec4 lhs, const Vec4& rhs) { return lhs *= rhs; }
constexpr Vec4 operator*(Vec4 lhs, float rhs) { return lhs *= rhs; }
constexpr Vec4 operator*(float lhs, const Vec4& rhs) { return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w}; }
constexpr Vec4 operator/(Vec4 lhs, const Vec4& rhs) { return lhs /= rhs; }
constexpr Vec4 operator/(Vec4 lhs, float rhs) { return lhs /= rhs; }
constexpr Vec4 operator/(float lhs, const Vec4& rhs) { return {lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w}; }
constexpr float dot(const Vec4& a, const Vec4& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
constexpr float lengthSq(const Vec4& value) { return dot(value, value); }
inline float length(const Vec4& value) { return std::hypot(std::hypot(value.x, value.y), std::hypot(value.z, value.w)); }
// Rescaling avoids intermediate overflow on large finite vectors.
// Invalid/near-zero input asserts in debug and returns zero with NDEBUG.
inline Vec4 normalize(const Vec4& value) {
    const bool finite = std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z) && std::isfinite(value.w);
    assert(finite && "normalize requires finite components");
    if (!finite) return {};
    const float largest = max(max(max(std::fabs(value.x), std::fabs(value.y)), std::fabs(value.z)), std::fabs(value.w));
    if (largest == 0.0f) {
        assert(false && "cannot normalize a zero vector");
        return {};
    }
    const Vec4 scaled = value / largest;
    const float scaledLength = length(scaled);
    if (largest <= NORMALIZATION_EPSILON / scaledLength) {
        assert(false && "vector is too short to normalize");
        return {};
    }
    return scaled / scaledLength;
}
inline float distance(const Vec4& a, const Vec4& b) { return length(a - b); }
constexpr Vec4 lerp(const Vec4& a, const Vec4& b, float t) { return (1.0f - t) * a + t * b; }
inline bool nearlyEqual(const Vec4& a, const Vec4& b, float absTolerance = 1.0e-6f,
                        float relTolerance = 1.0e-5f) {
    return nearlyEqual(a.x, b.x, absTolerance, relTolerance) &&
           nearlyEqual(a.y, b.y, absTolerance, relTolerance) &&
           nearlyEqual(a.z, b.z, absTolerance, relTolerance) &&
           nearlyEqual(a.w, b.w, absTolerance, relTolerance);
}

constexpr Vec3 cross(const Vec3& a, const Vec3& b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}
} // namespace math
