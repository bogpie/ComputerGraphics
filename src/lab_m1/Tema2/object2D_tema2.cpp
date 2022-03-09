#include "object2D_tema2.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D_tema2::CreateSquareFromCenter(
	const std::string& name,
	glm::vec3 center,
	float length,
	glm::vec3 color,
	bool fill)
{
	//glm::vec3 corner = leftBottomCorner;
	//glm::vec3 center = leftBottomCorner;


	std::vector<VertexFormat> vertices =
	{
		VertexFormat(center + glm::vec3(-length / 2, -length / 2, 0), color),

		// VertexFormat(center + glm::vec3(-length / 2 * 1.1, -length / 2 * 1.1, 0), glm::vec3(0)), // duplicated for border

		VertexFormat(center + glm::vec3(+length / 2, -length / 2, 0), color),
		// VertexFormat(center + glm::vec3(+length / 2 * 1.1, -length / 2 * 1.1, 0), glm::vec3(0)),


		VertexFormat(center + glm::vec3(+length / 2,+length / 2, 0), color),
		// VertexFormat(center + glm::vec3(+length / 2 * 1.1,+length / 2 * 1.1, 0), glm::vec3(0)),


		VertexFormat(center + glm::vec3(-length / 2, +length / 2, 0), color),
		// VertexFormat(center + glm::vec3(-length / 2 * 1.1, +length / 2 * 1.1, 0), glm::vec3(0)),
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2, 3 };

	if (!fill)
	{
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else
	{
		// Draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}
	square->InitFromData(vertices, indices);
	return square;
}

Mesh* object2D_tema2::CreateCubeOne(
	const std::string& name, glm::vec3 color)
{
	// Create a mesh box using custom data
	{
		std::vector<VertexFormat> vertices
		{
			// TODO(student): Complete the vertices data for the cube mesh
			VertexFormat(glm::vec3(0, 0,  1), color),
			VertexFormat(glm::vec3(1, 0,  1), color),
			VertexFormat(glm::vec3(0, 1,  1), color),
			VertexFormat(glm::vec3(1, 1,  1), color),
			VertexFormat(glm::vec3(0, 0,  0), color),
			VertexFormat(glm::vec3(1, 0,  0), color),
			VertexFormat(glm::vec3(0, 1,  0), color),
			VertexFormat(glm::vec3(1, 1,  0), color)

		};

		std::vector<unsigned int> indices =
		{
			0, 1, 2,
			1, 3, 2,
			2, 3, 7,
			2, 7, 6,
			1, 7, 3,
			1, 5, 7,
			6, 7, 4,
			7, 5, 4,
			0, 4, 1,
			1, 4, 5,
			2, 6, 4,
			0, 2, 4
		};
		Mesh* cube = new Mesh(name);
		cube->InitFromData(vertices, indices);
		return cube;
	}
}
