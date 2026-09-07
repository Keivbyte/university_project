#pragma once
#include <cmath>

namespace math {
inline constexpr float PI = 3.14159265358979323846f;
inline constexpr float TWO_PI = 2.0f * PI;
inline constexpr float HALF_PI = 0.5f * PI;
inline constexpr float NORMALIZATION_EPSILON = 1.0e-6f;

constexpr float radians(float degrees) { return degrees * (PI / 180.0f); }
constexpr float degrees(float angle) { return angle * (180.0f / PI); }
constexpr float min(float a, float b) { return a < b ? a : b; }
constexpr float max(float a, float b) { return a > b ? a : b; }
// Precondition: low <= high. lerp permits extrapolation.
constexpr float clamp(float value, float low, float high) { return min(max(value, low), high); }
constexpr float lerp(float a, float b, float t) { return (1.0f - t) * a + t * b; }
inline bool nearlyEqual(float a, float b, float absTolerance = 1.0e-6f,
                        float relTolerance = 1.0e-5f) {
    if (a == b) return true;
    if (!std::isfinite(a) || !std::isfinite(b)) return false;
    return std::fabs(a - b) <= max(absTolerance,
        relTolerance * max(std::fabs(a), std::fabs(b)));
}
} // namespace math
