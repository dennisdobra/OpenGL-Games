#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

namespace object3dimensions
{
    Mesh* CreateCube(const std::string& name, glm::vec3 center, float side, glm::vec3 color);
    Mesh* CreateParallelipiped(const std::string& name, glm::vec3 center, float width, float height, float depth, glm::vec3 color);
    Mesh* CreateSphere(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, int sectors = 36, int stacks = 18);

    Mesh* CreateCone(const std::string& name, glm::vec3 center, float baseRadius, float height, glm::vec3 color, int sectors);
    Mesh* CreateCylinder(const std::string& name, glm::vec3 center, float radius, float height, glm::vec3 color, int sectors);
    Mesh* CreatePropeller(
        const std::string& name,
        glm::vec3 center,
        float baseWidth,
        float height,
        float thickness,
        glm::vec3 color);


    Mesh* CreateTreeMesh(
        const std::string& name,
        glm::vec3 baseCenter,
        float trunkRadius,
        float trunkHeight,
        float foliageBaseRadius,
        float foliageHeight,
        glm::vec3 trunkColor,
        glm::vec3 foliageColor,
        int sectors);
}