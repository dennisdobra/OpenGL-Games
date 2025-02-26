#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2dimensions
{
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);

    Mesh* CreateTank(const std::string& name, glm::vec3 color1, glm::vec3 color2);

    Mesh* CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color);

    Mesh* CreateRectangle(const std::string& name, glm::vec3 color, bool fill);
}
