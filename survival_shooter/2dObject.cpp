#include "2dObject.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::create_square(
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
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::create_rectangle(
    const std::string& name,
    glm::vec3 position,
    float height,
    float width,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 pos = position;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(pos, color),
        VertexFormat(pos + glm::vec3(width, 0, 0), color),
        VertexFormat(pos + glm::vec3(width, height, 0), color),
        VertexFormat(pos + glm::vec3(0, height, 0), color)
    };


    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

Mesh* object2D::create_circle(float radiusX, float radiusY, const char* name, glm::vec3 color, bool fill)

{
    int triangleCount = 200;
    float deg = 2.f * M_PI;
    float angle;

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    vertices.push_back(VertexFormat(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
    indices.push_back(0.f);

    for (int i = 0; i < triangleCount; i++)
    {
        angle = i * deg / triangleCount;
        vertices.push_back(VertexFormat(glm::vec3(radiusX * cos(angle), radiusY * sin(angle), 0), color));
        indices.push_back(i);
    }
    indices.push_back(triangleCount);
    indices.push_back(1);

    Mesh* circle = new Mesh(name);
    if (!fill) {
        circle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        circle->SetDrawMode(GL_TRIANGLE_FAN);
    }

    circle->InitFromData(vertices, indices);
    return circle;
}

