#pragma once
#include <cassert>
#include "vec.h"

namespace math {
// Invariants of public fields must also be preserved by the caller on mutation.
// Ray direction is finite and unit length. Default direction is -Z.
struct Ray {
    Vec3 origin{};
    Vec3 direction{0.0f, 0.0f, -1.0f};
    constexpr Ray() = default;
    Ray(const Vec3& start, const Vec3& unitDirection)
        : origin(start), direction(unitDirection) {
        assert(nearlyEqual(lengthSq(direction), 1.0f));
    }
};
// Finite, ordered bounds. Default is a point at origin, NOT an empty box.
struct AABB {
    Vec3 min{};
    Vec3 max{};
    constexpr AABB() = default;
    constexpr AABB(const Vec3& minimum, const Vec3& maximum) : min(minimum), max(maximum) {
        assert(min.x <= max.x && min.y <= max.y && min.z <= max.z);
    }
    constexpr bool contains(const Vec3& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }
    constexpr void merge(const AABB& other) {
        min = {math::min(min.x, other.min.x), math::min(min.y, other.min.y),
               math::min(min.z, other.min.z)};
        max = {math::max(max.x, other.max.x), math::max(max.y, other.max.y),
               math::max(max.z, other.max.z)};
    }
    constexpr Vec3 center() const { return min * 0.5f + max * 0.5f; }
    constexpr Vec3 extents() const { return max * 0.5f - min * 0.5f; }
};
// dot(normal, point) + distance = 0; normal is finite and unit length.
// Default plane is y=0. Normalize BOTH normal and distance together.
struct Plane {
    Vec3 normal{0.0f, 1.0f, 0.0f};
    float distance = 0.0f;
    constexpr Plane() = default;
    Plane(const Vec3& unitNormal, float offset) : normal(unitNormal), distance(offset) {
        assert(nearlyEqual(lengthSq(normal), 1.0f));
        assert(std::isfinite(distance));
    }
    constexpr float signedDistance(const Vec3& point) const {
        return dot(normal, point) + distance;
    }
};
} // namespace math
