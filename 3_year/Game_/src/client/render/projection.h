#pragma once

#include "shared/math/mat.h"

// Right-handed perspective, column vectors, camera looks along -Z.
// Vertical FOV in radians. NDC depth is [0,1]: near -> 0, far -> 1.
math::Mat4 Perspective(float verticalFov, float aspect, float nearPlane, float farPlane);
