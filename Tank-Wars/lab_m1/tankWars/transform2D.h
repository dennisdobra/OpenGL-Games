#pragma once

#include "utils/glm_utils.h"

namespace transform2D
{
    inline glm::mat3 Translate(float val1, float val2)
    {
        return glm::mat3(
            1, 0, 0,
            0, 1, 0,
            val1, val2, 1
        );
    }

    inline glm::mat3 Scale(float val1, float val2)
    {
        return glm::mat3(
            val1, 0, 0,
            0, val2, 0,
            0, 0, 1
        );
    }

    inline glm::mat3 Shear(float val)
    {
        return glm::mat3(
            1, val, 0,
            0, 1, 0,
            0, 0, 1
        );
    }

    inline glm::mat3 Rotate(float radians)
    {
        return glm::mat3(
            cos(radians), sin(radians), 0,
            -sin(radians), cos(radians), 0,
            0, 0, 1
        );
    }
} // namespace transform2D