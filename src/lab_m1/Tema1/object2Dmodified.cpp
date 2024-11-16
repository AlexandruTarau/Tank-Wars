#include "object2Dmodified.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

Mesh* object2Dmodified::CreateSquare(
    const std::string& name,
    glm::vec3 leftTopCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftTopCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, -length, 0), color),
        VertexFormat(corner + glm::vec3(0, -length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices;

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
        indices = { 0, 1, 2, 3 };
    }
    else {
        indices = { 0, 1, 2, 0, 2, 3 };
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2Dmodified::CreateTank(
    const std::string& name,
    glm::vec3 bottomMiddleCenter,
    float length,
    float height,
    float radius,
    glm::vec3 bottomColor,
    glm::vec3 topColor,
    bool fill)
{
    glm::vec3 center = bottomMiddleCenter;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(center + glm::vec3(-length / 2.f, 0, 0), bottomColor),
        VertexFormat(center + glm::vec3(length / 2.f, 0, 0), bottomColor),
        VertexFormat(center + glm::vec3(length / 2.f + height / 5.f, height / 5.f, 0), bottomColor),
        VertexFormat(center + glm::vec3(-length / 2.f - height / 5.f, height / 5.f, 0), bottomColor),
        VertexFormat(center + glm::vec3(-length / 2.f - 3 * height / 5.f, height / 5.f, 0), topColor),
        VertexFormat(center + glm::vec3(length / 2.f + 3 * height / 5.f, height / 5.f, 0), topColor),
        VertexFormat(center + glm::vec3(-length / 2.f - height / 5.f, 3 * height / 5.f, 0), topColor),
        VertexFormat(center + glm::vec3(length / 2.f + height / 5.f, 3 * height / 5.f, 0), topColor)
    };

    Mesh* tank = new Mesh(name);
    std::vector<unsigned int> indices;

    if (!fill) {
        tank->SetDrawMode(GL_LINE_LOOP);
        indices = { 0, 1, 2, 3, 4, 5, 6, 7 };
    }
    else {
        indices = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 5, 7, 6 };
    }

    // Circle center
    vertices.push_back(VertexFormat(center + glm::vec3(0, 3.f * height / 5.f, 0), topColor));

    float numTriangles = 64;
    float angleStep = glm::pi<float>() / numTriangles;

    // Generate the half circle
    for (int i = 0; i < numTriangles; i++) {
        float angle = i * angleStep;
        float x = center.x + radius * cos(angle);
        float y = center.y + 3.f * height / 5.f + radius * sin(angle);

        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), topColor));
        if (i > 0) {
            indices.push_back(0);
            indices.push_back(i - 1);
            indices.push_back(i);
        }
    }

    tank->InitFromData(vertices, indices);
    return tank;
}

Mesh* object2Dmodified::CreateCircle(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices;

    Mesh* circle = new Mesh(name);
    std::vector<unsigned int> indices;

    if (!fill) {
        circle->SetDrawMode(GL_LINE_LOOP);
    }

    float numTriangles = 64;
    float angleStep = 2 * glm::pi<float>() / numTriangles;

    // Generate the circle
    for (int i = 0; i < numTriangles; i++) {
        float angle = i * angleStep;
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);

        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
        if (i > 0) {
            indices.push_back(0);
            indices.push_back(i - 1);
            indices.push_back(i);
        }
    }

    circle->InitFromData(vertices, indices);
    return circle;
}

Mesh* object2Dmodified::CreateLine(const std::string& name, glm::vec3 color)
{
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f), color),
        VertexFormat(glm::vec3(1.0f, 0.0f, 0.0f), color)
    };

    Mesh* line = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1 };

    line->SetDrawMode(GL_LINES);
    line->InitFromData(vertices, indices);

    return line;
}