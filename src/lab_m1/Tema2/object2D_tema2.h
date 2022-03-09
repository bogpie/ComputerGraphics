#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D_tema2
{

	// Create square with given center, length and color
	Mesh* CreateSquareFromCenter(const std::string& name, glm::vec3 center, float length, glm::vec3 color, bool fill = true);
	Mesh* CreateCubeOne(const std::string& name, glm::vec3 color);
}
