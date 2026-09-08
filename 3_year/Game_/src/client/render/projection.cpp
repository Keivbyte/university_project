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
