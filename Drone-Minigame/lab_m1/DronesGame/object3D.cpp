#include "object3D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

Mesh* object3dimensions::CreateParallelipiped(const std::string& name, glm::vec3 center, float width, float height, float depth, glm::vec3 color) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(center + glm::vec3(-width / 2, -height / 2,  depth / 2), color), // bottom left front
        VertexFormat(center + glm::vec3(width / 2, -height / 2,  depth / 2), color),  // bottom right front
        VertexFormat(center + glm::vec3(width / 2,  height / 2,  depth / 2), color),  // top right front
        VertexFormat(center + glm::vec3(-width / 2,  height / 2,  depth / 2), color), // top left front
        VertexFormat(center + glm::vec3(-width / 2, -height / 2, -depth / 2), color), // bottom left back
        VertexFormat(center + glm::vec3(width / 2, -height / 2, -depth / 2), color),  // bottom right back
        VertexFormat(center + glm::vec3(width / 2,  height / 2, -depth / 2), color),  // top right back
        VertexFormat(center + glm::vec3(-width / 2,  height / 2, -depth / 2), color)  // top left back
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,  0, 2, 3,  // Front face
        4, 5, 6,  4, 6, 7,  // Back face
        0, 3, 7,  0, 7, 4,  // Left face
        1, 5, 6,  1, 6, 2,  // Right face
        3, 2, 6,  3, 6, 7,  // Top face
        0, 1, 5,  0, 5, 4   // Bottom face
    };

    Mesh* parallelepiped = new Mesh(name);
    parallelepiped->InitFromData(vertices, indices);
    return parallelepiped;
}

Mesh* object3dimensions::CreateSphere(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, int sectors, int stacks) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Generate vertices for the sphere
    for (int stack = 0; stack <= stacks; ++stack) {
        float stackAngle = M_PI / 2 - stack * (M_PI / stacks); // Angle from the equator
        float xy = radius * cos(stackAngle);                   // Radius at current stack
        float z = radius * sin(stackAngle);                    // Height at current stack

        for (int sector = 0; sector <= sectors; ++sector) {
            float sectorAngle = sector * (2 * M_PI / sectors); // Angle around the circle

            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);

            vertices.emplace_back(center + glm::vec3(x, y, z), color);
        }
    }

    // Generate indices for the sphere
    for (int stack = 0; stack < stacks; ++stack) {
        int stackStart = stack * (sectors + 1);
        int nextStackStart = (stack + 1) * (sectors + 1);

        for (int sector = 0; sector < sectors; ++sector) {
            int current = stackStart + sector;
            int next = stackStart + sector + 1;
            int below = nextStackStart + sector;
            int belowNext = nextStackStart + sector + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(below);

            indices.push_back(next);
            indices.push_back(belowNext);
            indices.push_back(below);
        }
    }

    Mesh* sphere = new Mesh(name);
    sphere->InitFromData(vertices, indices);
    return sphere;
}

Mesh* object3dimensions::CreateTreeMesh(const std::string& name, glm::vec3 baseCenter, float trunkRadius, float trunkHeight, float foliageBaseRadius, float foliageHeight, glm::vec3 trunkColor, glm::vec3 foliageColor, int sectors)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Trunk (Cylinder)
    float halfHeight = trunkHeight / 2.0f;
    for (int i = 0; i <= sectors; ++i) {
        float angle = i * (2 * M_PI / sectors);
        float x = trunkRadius * cos(angle);
        float z = trunkRadius * sin(angle);

        // Bottom and top vertices
        vertices.emplace_back(baseCenter + glm::vec3(x, -halfHeight, z), trunkColor); // Bottom circle
        vertices.emplace_back(baseCenter + glm::vec3(x, halfHeight, z), trunkColor);  // Top circle
    }

    // Generate trunk indices
    int bottomCenterIndex = vertices.size();
    vertices.emplace_back(baseCenter + glm::vec3(0, -halfHeight, 0), trunkColor); // Bottom center
    int topCenterIndex = vertices.size();
    vertices.emplace_back(baseCenter + glm::vec3(0, halfHeight, 0), trunkColor);  // Top center

    for (int i = 0; i < sectors; ++i) {
        // Sides
        indices.push_back(2 * i);
        indices.push_back(2 * i + 1);
        indices.push_back((2 * (i + 1)) % (2 * sectors));

        indices.push_back(2 * i + 1);
        indices.push_back((2 * (i + 1) + 1) % (2 * sectors));
        indices.push_back((2 * (i + 1)) % (2 * sectors));

        // Bottom cap
        indices.push_back(bottomCenterIndex);
        indices.push_back(2 * i);
        indices.push_back((2 * (i + 1)) % (2 * sectors));

        // Top cap
        indices.push_back(topCenterIndex);
        indices.push_back(2 * i + 1);
        indices.push_back((2 * (i + 1) + 1) % (2 * sectors));
    }

    // Foliage (Cones)
    float coneSpacing = foliageHeight * 0.6f; // Adjust spacing for realism
    for (int i = 0; i < 3; ++i) {
        glm::vec3 coneBaseCenter = baseCenter + glm::vec3(0, halfHeight + i * coneSpacing, 0);
        glm::vec3 coneApex = coneBaseCenter + glm::vec3(0, foliageHeight, 0);

        int coneBaseStartIndex = vertices.size();
        for (int j = 0; j <= sectors; ++j) {
            float angle = j * (2 * M_PI / sectors);
            float x = foliageBaseRadius * cos(angle);
            float z = foliageBaseRadius * sin(angle);
            vertices.emplace_back(coneBaseCenter + glm::vec3(x, 0, z), foliageColor);
        }

        // Add cone apex vertex
        int coneApexIndex = vertices.size();
        vertices.emplace_back(coneApex, foliageColor);

        // Cone indices
        for (int j = 0; j < sectors; ++j) {
            // Side triangles
            indices.push_back(coneBaseStartIndex + j);
            indices.push_back(coneBaseStartIndex + j + 1);
            indices.push_back(coneApexIndex);
        }

        foliageBaseRadius *= 0.75f; // Gradually reduce the radius for realism
        foliageHeight *= 0.8f;     // Reduce the height of the next cone
    }

    // Create the tree mesh
    Mesh* treeMesh = new Mesh(name);
    treeMesh->InitFromData(vertices, indices);
    return treeMesh;
}

Mesh* object3dimensions::CreateCylinder(const std::string& name, glm::vec3 center, float radius, float height, glm::vec3 color, int sectors) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Top and bottom center points
    glm::vec3 topCenter = center + glm::vec3(0, height / 2, 0);
    glm::vec3 bottomCenter = center - glm::vec3(0, height / 2, 0);

    vertices.emplace_back(topCenter, color);    // Top center vertex
    vertices.emplace_back(bottomCenter, color); // Bottom center vertex

    // Generate vertices around the top and bottom circles
    for (int i = 0; i <= sectors; ++i) {
        float angle = i * (2.0f * M_PI / sectors);
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.emplace_back(topCenter + glm::vec3(x, 0, z), color);    // Top circle
        vertices.emplace_back(bottomCenter + glm::vec3(x, 0, z), color); // Bottom circle
    }

    // Generate indices for the sides
    for (int i = 2; i < 2 + sectors * 2; i += 2) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 3);

        indices.push_back(i);
        indices.push_back(i + 3);
        indices.push_back(i + 2);
    }

    // Generate indices for top and bottom caps
    for (int i = 2; i < 2 + sectors * 2; i += 2) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 2);

        indices.push_back(1);
        indices.push_back(i + 3);
        indices.push_back(i + 1);
    }

    Mesh* cylinder = new Mesh(name);
    cylinder->InitFromData(vertices, indices);
    return cylinder;
}

Mesh* object3dimensions::CreateCone(const std::string& name, glm::vec3 center, float baseRadius, float height, glm::vec3 color, int sectors) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 tip = center + glm::vec3(0, height / 2, 0);
    glm::vec3 baseCenter = center - glm::vec3(0, height / 2, 0);

    vertices.emplace_back(tip, color);          // Tip vertex
    vertices.emplace_back(baseCenter, color);   // Base center vertex

    for (int i = 0; i <= sectors; ++i) {
        float angle = i * (2.0f * M_PI / sectors);
        float x = baseRadius * cos(angle);
        float z = baseRadius * sin(angle);

        vertices.emplace_back(baseCenter + glm::vec3(x, 0, z), color); // Base circle
    }

    for (int i = 2; i < 2 + sectors; ++i) {
        // Side triangles
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);

        // Base triangles
        indices.push_back(1);
        indices.push_back(i + 1);
        indices.push_back(i);
    }

    Mesh* cone = new Mesh(name);
    cone->InitFromData(vertices, indices);
    return cone;
}

Mesh* object3dimensions::CreatePropeller(const std::string& name, glm::vec3 center, float baseWidth, float height, float thickness, glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Vertices for the front face (triangle 1)
    glm::vec3 v1 = center + glm::vec3(-baseWidth / 2, 0, thickness / 2); // Bottom-left
    glm::vec3 v2 = center + glm::vec3(baseWidth / 2, 0, thickness / 2);  // Bottom-right
    glm::vec3 v3 = center + glm::vec3(0, height, thickness / 2);         // Top

    // Vertices for the back face (triangle 2)
    glm::vec3 v4 = center + glm::vec3(-baseWidth / 2, 0, -thickness / 2); // Bottom-left
    glm::vec3 v5 = center + glm::vec3(baseWidth / 2, 0, -thickness / 2);  // Bottom-right
    glm::vec3 v6 = center + glm::vec3(0, height, -thickness / 2);         // Top

    // Add vertices to the list
    vertices.push_back(VertexFormat(v1, color));
    vertices.push_back(VertexFormat(v2, color));
    vertices.push_back(VertexFormat(v3, color));
    vertices.push_back(VertexFormat(v4, color));
    vertices.push_back(VertexFormat(v5, color));
    vertices.push_back(VertexFormat(v6, color));

    // Define indices for the triangular faces
    // Front face
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    // Back face
    indices.push_back(3);
    indices.push_back(5);
    indices.push_back(4);

    // Define indices for the rectangular side faces
    // Side 1
    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(2);

    indices.push_back(3);
    indices.push_back(5);
    indices.push_back(2);

    // Side 2
    indices.push_back(1);
    indices.push_back(4);
    indices.push_back(5);

    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(2);

    // Side 3 (bottom base)
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(4);

    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(3);

    // Create the mesh
    Mesh* propeller = new Mesh(name);
    propeller->InitFromData(vertices, indices);
    return propeller;
}
