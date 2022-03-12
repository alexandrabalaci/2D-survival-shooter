#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D
{
    Mesh* create_square(const std::string &name, glm::vec3 position, float length, glm::vec3 color, bool fill = false);
    Mesh* create_rectangle(const std::string& name, glm::vec3 position, float height, float width, glm::vec3 color, bool fill = false);
    Mesh* create_circle(float radiusX, float radiusY, const char* name, glm::vec3 color, bool fill);

}
