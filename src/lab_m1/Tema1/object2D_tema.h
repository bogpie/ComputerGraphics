#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D_tema
{

	// Create square with given center, length and color
	Mesh* CreateSquareFromCenter(const std::string& name, glm::vec3 center, float length, glm::vec3 color, bool fill = true);
	
	Mesh* CreateProjectile(
		const std::string& name,
		glm::vec3 center);

	Mesh* CreateBorder(
		const std::string& name,
		glm::vec3 center,
		float worldSizeMultiplier,
		glm::vec3 color);

	Mesh* CreateCircle(
		const std::string& name,
		glm::vec3 center,
		float radius,
		glm::vec3 color,
		bool fill);


	Mesh* CreateEnemyBody(
		const std::string& name,
		glm::vec3 center);
}
