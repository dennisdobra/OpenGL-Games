#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2dimensions::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2dimensions::CreateRectangle(
    const std::string& name,
    glm::vec3 color,
    bool fill)
{

    glm::vec3 bottomLeft = glm::vec3(-2, 0, 0);
    glm::vec3 bottomRight = glm::vec3(2, 0, 0);
    glm::vec3 topRight = glm::vec3(2, 40, 0);
    glm::vec3 topLeft = glm::vec3(-2, 40, 0);

    std::vector<VertexFormat> vertices = {
        VertexFormat(bottomLeft, color),
        VertexFormat(bottomRight, color),
        VertexFormat(topRight, color),
        VertexFormat(topLeft, color)
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

Mesh* object2dimensions::CreateTank(
    const std::string& name,
    glm::vec3 color1,
    glm::vec3 color2)
{
    // Create the bottom trapezoid (larger base facing up)
    glm::vec3 bottomLeft = glm::vec3(-20, 0, 0);
    glm::vec3 bottomRight = glm::vec3(20, 0, 0);
    glm::vec3 topRight = glm::vec3(25, 4, 0);
    glm::vec3 topLeft = glm::vec3(-25, 4, 0);

    //// Create upper trapezoid (larger base facing down)
    glm::vec3 bottomLeft2 = glm::vec3(-32, 4, 0);
    glm::vec3 bottomRight2 = glm::vec3(32, 4, 0);
    glm::vec3 topRight2 = glm::vec3(20, 17, 0);
    glm::vec3 topLeft2 = glm::vec3(-20, 17, 0);

    std::vector<VertexFormat> vertices = {
        VertexFormat(bottomLeft, color1),
        VertexFormat(bottomRight, color1),
        VertexFormat(topRight, color1),
        VertexFormat(topLeft, color1),
        VertexFormat(bottomLeft2, color2),
        VertexFormat(bottomRight2, color2),
        VertexFormat(topRight2, color2),
        VertexFormat(topLeft2, color2),
    };

    glm::vec3 center = glm::vec3(0, 17, 0);
    vertices.push_back(VertexFormat(center, color2));

    std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };

    int nr_points = 50;
    int radius = 12;
    for (int i = 0; i < nr_points; i++) {
        // Calculate the current and next point around the semicircle
        float angle1 = (i * AI_MATH_PI) / nr_points;
        float angle2 = ((i + 1) * AI_MATH_PI) / nr_points;

        glm::vec3 point1 = glm::vec3(center.x + radius * cos(angle1), center.y + radius * sin(angle1), 0);
        glm::vec3 point2 = glm::vec3(center.x + radius * cos(angle2), center.y + radius * sin(angle2), 0);

        // Add points to vertices
        vertices.push_back(VertexFormat(point1, color2));
        vertices.push_back(VertexFormat(point2, color2));

        // Get indices for the two new points
        int index1 = vertices.size() - 2;
        int index2 = vertices.size() - 1;

        // Add center and the two points as a triangle
        indices.push_back(8);
        indices.push_back(index1);
        indices.push_back(index2);
    }

    Mesh* tank = new Mesh(name);

    tank->InitFromData(vertices, indices);
    return tank;
}

Mesh* object2dimensions::CreateCircle(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color)
{
    int numPoints = 100;

    std::vector<VertexFormat> vertices;
    vertices.push_back(VertexFormat(center, color));

    // create the points around the circle
    for (int i = 0; i < numPoints; i++) {
        float angle = (i * 2.0f * AI_MATH_PI) / numPoints;

        glm::vec3 point = glm::vec3(
            center.x + radius * cos(angle),
            center.y + radius * sin(angle),
            center.z
        );

        vertices.push_back(VertexFormat(point, color));
    }

    // create the triangles
    std::vector<unsigned int> indices;
    for (int i = 1; i <= numPoints; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i % numPoints + 1);
    }

    Mesh* moon = new Mesh(name);
    moon->InitFromData(vertices, indices);
    return moon;
}
