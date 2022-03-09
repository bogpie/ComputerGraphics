#include "lab_m1/lab4/lab4.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/lab4/transform3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab4::Lab4()
{}


Lab4::~Lab4()
{}


void Lab4::Init()
{
	polygonMode = GL_FILL;

	Mesh* mesh = new Mesh("box");
	mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
	meshes[mesh->GetMeshID()] = mesh;

	// Initialize tx, ty and tz (the translation steps)
	translate.x = 0;
	translate.y = 0;
	translate.z = 0;

	// Initialize sx, sy and sz (the scale factors)
	scale = glm::vec3{ 1,1,1 };

	// Initialize angular steps
	angularStep = glm::vec3{ 0,0,0 };


	sunRotation = earthRotation = moonRotation = 0;
	earthRevolution = moonRevolution = 0;
	sunTranslate = glm::vec3{ 0,0,0 };
}

void Lab4::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Sets the screen area where to draw
	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}


void Lab4::Update(float deltaTimeSeconds)
{
	glLineWidth(3);
	glPointSize(5);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(-2.5f, 0.5f, -1.5f);
	modelMatrix *= transform3D::Translate(translate.x, translate.y, translate.z);
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(0.0f, 0.5f, -1.5f);
	modelMatrix *= transform3D::Scale(scale.x, scale.y, scale.z);
	RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(2.5f, 0.5f, -1.5f);
	modelMatrix *= transform3D::RotateOX(angularStep.x);
	modelMatrix *= transform3D::RotateOY(angularStep.y);
	modelMatrix *= transform3D::RotateOZ(angularStep.z);
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

	// TODO(student): Bonus - Solar System

	// Sun
	modelMatrix = glm::mat4(1);
	sunRotation += deltaTimeSeconds / 4;
	modelMatrix *= transform3D::Translate(0, 5, 0);
	modelMatrix *= transform3D::RotateOY(sunRotation);
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);


	// Earth
	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::RotateOY(sunRotation);
	
	earthRotation += deltaTimeSeconds / 2;
	modelMatrix *= transform3D::Translate(-2, 5, 0);
	modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
	modelMatrix *= transform3D::RotateOY(earthRotation);
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

	
	// Moon
	modelMatrix = glm::mat4(1);

	modelMatrix *= transform3D::RotateOY(sunRotation);
	modelMatrix *= transform3D::RotateOY(earthRotation);

	moonRotation += deltaTimeSeconds;
	modelMatrix *= transform3D::Translate(-4, 5, 0);
	modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
	modelMatrix *= transform3D::RotateOY(moonRotation);
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);


}



void Lab4::FrameEnd()
{
	DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab4::OnInputUpdate(float deltaTime, int mods)
{
	// DONE(student): Add transformation logic

	// DONE(student): Add translation logic
	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		if (window->KeyHold(GLFW_KEY_W))
		{
			translate.z -= deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_A))
		{
			translate.x -= deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_S))
		{
			translate.z += deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_D))
		{
			translate.x += deltaTime;
		}
	}

	// DONE(student): Add pulsation logic
	if (window->KeyHold(GLFW_KEY_F))
	{
		translate.y -= deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_R))
	{
		translate.y += deltaTime;
	}
	if (window->KeyHold(GLFW_KEY_1))
	{
		scale -= deltaTime;
	}
	if (window->KeyHold(GLFW_KEY_2))
	{
		scale += deltaTime;
	}

	// DONE(student): Add rotation logic
	if (window->KeyHold(GLFW_KEY_3))
	{
		angularStep.x -= deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_4))
	{
		angularStep.x += deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_5))
	{
		angularStep.y -= deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_6))
	{
		angularStep.y += deltaTime;
	}
	if (window->KeyHold(GLFW_KEY_7))
	{
		angularStep.z -= deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_8))
	{
		angularStep.z += deltaTime;
	}
}


void Lab4::OnKeyPress(int key, int mods)
{
	// Add key press event
	if (key == GLFW_KEY_SPACE)
	{
		switch (polygonMode)
		{
		case GL_POINT:
			polygonMode = GL_FILL;
			break;
		case GL_LINE:
			polygonMode = GL_POINT;
			break;
		default:
			polygonMode = GL_LINE;
			break;
		}
	}
}


void Lab4::OnKeyRelease(int key, int mods)
{
	// Add key release event
}


void Lab4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Add mouse move event
}


void Lab4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button press event
}


void Lab4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}


void Lab4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{}


void Lab4::OnWindowResize(int width, int height)
{}
