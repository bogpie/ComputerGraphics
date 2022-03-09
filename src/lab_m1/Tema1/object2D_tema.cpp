#include "object2D_tema.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"
#include "Tema1.h"


Mesh* object2D_tema::CreateSquareFromCenter(
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

		VertexFormat(center + glm::vec3(-length / 2 * 1.1, -length / 2 * 1.1, 0), glm::vec3(0)), // duplicated for border

		VertexFormat(center + glm::vec3(+length / 2, -length / 2, 0), color),
		VertexFormat(center + glm::vec3(+length / 2 * 1.1, -length / 2 * 1.1, 0), glm::vec3(0)),


		VertexFormat(center + glm::vec3(+length / 2,+length / 2, 0), color),
		VertexFormat(center + glm::vec3(+length / 2 * 1.1,+length / 2 * 1.1, 0), glm::vec3(0)),


		VertexFormat(center + glm::vec3(-length / 2, +length / 2, 0), color),
		VertexFormat(center + glm::vec3(-length / 2 * 1.1, +length / 2 * 1.1, 0), glm::vec3(0)),
	};

	Mesh* square = new Mesh(name);
	//std::vector<unsigned int> indices = { 0, 1, 2, 3 };
	std::vector<unsigned int> indices;

	if (!fill)
	{
		indices = { 1, 3, 5, 7, 1, 3 };
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else
	{
		// Draw 2 triangles. Add the remaining 2 indices

		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(4);
		
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(5);

		indices.push_back(6);
		indices.push_back(0);
		indices.push_back(4);

		indices.push_back(7);
		indices.push_back(1);
		indices.push_back(5);
	}

	square->InitFromData(vertices, indices);
	return square;
}

Mesh* object2D_tema::CreateProjectile(
	const std::string& name,
	glm::vec3 center)
{
	glm::vec3 color = glm::vec3(0, 0, 0);
	bool fill = true;

	float length = m1::projectileSide;
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(center + glm::vec3(-length / 2, -length / 2, 0), color),
		VertexFormat(center + glm::vec3(+length / 2, -length / 2, 0), color),
		VertexFormat(center + glm::vec3(+length / 2,+length / 2, 0), color),
		VertexFormat(center + glm::vec3(-length / 2, +length / 2, 0), color),
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



Mesh* object2D_tema::CreateEnemyBody(
	const std::string& name,
	glm::vec3 center)
{
	//glm::vec3 corner = leftBottomCorner;
	//glm::vec3 center = leftBottomCorner;
	bool fill = true;

	glm::vec3 color = glm::vec3{ 0.5, 0.05, 0.10 };

	float length = m1::squareSide;
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(center + glm::vec3(-length / 2, -length / 2, 0), color),
		VertexFormat(center + glm::vec3(+length / 2, -length / 2, 0), color),
		VertexFormat(center + glm::vec3(+length / 2,+length / 2, 0), color),
		VertexFormat(center + glm::vec3(-length / 2, +length / 2, 0), color),
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

Mesh* object2D_tema::CreateBorder(
	const std::string& name,
	glm::vec3 center,
	float worldSizeMultiplier,
	glm::vec3 color)
{
	bool fill = false;
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(center + glm::vec3(-m1::windowMax.x / 2 * worldSizeMultiplier, -m1::windowMax.y / 2 * worldSizeMultiplier, 0), color),
		VertexFormat(center + glm::vec3(+m1::windowMax.x / 2 * worldSizeMultiplier, -m1::windowMax.y / 2 * worldSizeMultiplier, 0), color),
		VertexFormat(center + glm::vec3(+m1::windowMax.x / 2 * worldSizeMultiplier, +m1::windowMax.y / 2 * worldSizeMultiplier, 0), color),
		VertexFormat(center + glm::vec3(-m1::windowMax.x / 2 * worldSizeMultiplier, +m1::windowMax.y / 2 * worldSizeMultiplier, 0), color),
	};

	Mesh* borderMesh = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2, 3 };

	if (!fill)
	{
		borderMesh->SetDrawMode(GL_LINE_LOOP);
	}
	else
	{
		// Draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	borderMesh->InitFromData(vertices, indices);
	return borderMesh;
}

Mesh* object2D_tema::CreateCircle(
	const std::string& name,
	glm::vec3 center,
	float radius,
	glm::vec3 color,
	bool fill)
{
	int noTriangles = 100;

	std::vector<VertexFormat> verticesCircle
	{
		// Add the center and the radius at 0 degrees
		VertexFormat(glm::vec3(0, 0, 0), color), // 0
		VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0,0,0)), // duplicated for border

		VertexFormat(glm::vec3(1, 0, 0), color),
		VertexFormat(glm::vec3(1, 0, 0) * 1.2f, glm::vec3(0, 0, 0)),

	};

	std::vector<unsigned int> indicesCircle;
	for (int idTriangle = 0; idTriangle < noTriangles; ++idTriangle)
	{
		float angle = 2 * M_PI / noTriangles * (idTriangle + 1.0);
		verticesCircle.push_back(VertexFormat
		(
			glm::vec3
			(
				radius * cos(angle),
				radius * sin(angle),
				0
			),
			color
		));

		verticesCircle.push_back(VertexFormat
		(
			glm::vec3
			(
				radius * cos(angle),
				radius * sin(angle),
				0
			) * 1.2f,
			glm::vec3(0, 0, 0)
		));

		indicesCircle.push_back(0);

		unsigned int secondIndice = idTriangle < noTriangles - 1 ? idTriangle + 2 : 1;
		unsigned int firstIndice = idTriangle + 1;


		indicesCircle.push_back(secondIndice * 2);
		indicesCircle.push_back(firstIndice * 2);

		indicesCircle.push_back(1);

		indicesCircle.push_back(secondIndice * 2 + 1);
		indicesCircle.push_back(firstIndice * 2 + 1);

	}

	Mesh* circle = new Mesh(name);
	circle->InitFromData(verticesCircle, indicesCircle);
	return circle;
}
