#pragma once
#include <cassert>
#include <cmath>
#include "vec.h"

namespace math {
// Column-major: data[column * 4 + row]. Column vectors: M * v.
// T * R * S applies S, then R, then T. Angles are radians.
// Right-handed: +X right, +Y up, default camera forward -Z.
// Positive rotations follow the right-hand rule.
// Points have w=1, directions w=0. Default matrix is ZERO.
// View/projection factories and inversion are deferred, with no stubs.
struct Mat4 {
    float data[16]{};
    constexpr float& operator()(int row, int column) {
        assert(row >= 0 && row < 4 && column >= 0 && column < 4);
        return data[column * 4 + row];
    }
    constexpr const float& operator()(int row, int column) const {
        assert(row >= 0 && row < 4 && column >= 0 && column < 4);
        return data[column * 4 + row];
    }
    constexpr bool operator==(const Mat4& other) const = default;
    static constexpr Mat4 identity() {
        Mat4 result;
        for (int i = 0; i < 4; ++i) result(i, i) = 1.0f;
        return result;
    }
    static constexpr Mat4 translation(const Vec3& offset) {
        Mat4 result = identity();
        for (int i = 0; i < 3; ++i) result(i, 3) = offset[i];
        return result;
    }
    static constexpr Mat4 scale(const Vec3& factors) {
        Mat4 result = identity();
        for (int i = 0; i < 3; ++i) result(i, i) = factors[i];
        return result;
    }
    static constexpr Mat4 scale(float factor) { return scale(Vec3{factor, factor, factor}); }
    static inline Mat4 rotationX(float angle) {
        Mat4 result = identity();
        const float c = std::cos(angle), s = std::sin(angle);
        result(1, 1) = c; result(1, 2) = -s;
        result(2, 1) = s; result(2, 2) = c;
        return result;
    }
    static inline Mat4 rotationY(float angle) {
        Mat4 result = identity();
        const float c = std::cos(angle), s = std::sin(angle);
        result(0, 0) = c; result(0, 2) = s;
        result(2, 0) = -s; result(2, 2) = c;
        return result;
    }
    static inline Mat4 rotationZ(float angle) {
        Mat4 result = identity();
        const float c = std::cos(angle), s = std::sin(angle);
        result(0, 0) = c; result(0, 1) = -s;
        result(1, 0) = s; result(1, 1) = c;
        return result;
    }
};
constexpr Vec4 operator*(const Mat4& matrix, const Vec4& vector) {
    Vec4 result;
    for (int row = 0; row < 4; ++row)
        for (int column = 0; column < 4; ++column)
            result[row] += matrix(row, column) * vector[column];
    return result;
}
constexpr Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 result;
    for (int row = 0; row < 4; ++row)
        for (int column = 0; column < 4; ++column)
            for (int k = 0; k < 4; ++k)
                result(row, column) += a(row, k) * b(k, column);
    return result;
}
constexpr Mat4 transpose(const Mat4& matrix) {
    Mat4 result;
    for (int row = 0; row < 4; ++row)
        for (int column = 0; column < 4; ++column)
            result(row, column) = matrix(column, row);
    return result;
}
inline bool nearlyEqual(const Mat4& a, const Mat4& b, float absTolerance = 1.0e-6f,
                        float relTolerance = 1.0e-5f) {
    for (int i = 0; i < 16; ++i)
        if (!nearlyEqual(a.data[i], b.data[i], absTolerance, relTolerance)) return false;
    return true;
}
} // namespace math
