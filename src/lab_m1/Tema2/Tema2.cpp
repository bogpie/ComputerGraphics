#include "lab_m1/Tema2/Tema2.h"
#include <components/text_renderer.h>

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;
using namespace chrono;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Tema2::Tema2()
{}


Tema2::~Tema2()
{}


Tema2::Perspective Tema2::getDefaultPerspective()
{
	// projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
	Perspective defaultPerspective;
	defaultPerspective.fov = RADIANS(60);
	defaultPerspective.aspect = window->props.aspectRatio;
	defaultPerspective.zNear = 0.01f;
	defaultPerspective.zFar = 200.0f;
	return defaultPerspective;
}

void Tema2::Init()
{
	gameStartTime = high_resolution_clock::now();

	camera = new implemented::Camera();

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("teapot");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("plane50");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("quad");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		// Will be scaled
		Mesh* healthMesh = object2D_tema2::CreateSquareFromCenter("healthMesh", glm::vec3(0, 0, 0), 1, glm::vec3(1, 0, 0), true);
		AddMeshToList(healthMesh);
	}

	{
		Mesh* healthWireframeMesh = object2D_tema2::CreateSquareFromCenter("healthWireframeMesh", glm::vec3(0, 0, 0), 1, glm::vec3(1, 0, 0), false);
		AddMeshToList(healthWireframeMesh);
	}

	{
		Mesh* wallMesh = object2D_tema2::CreateCubeOne("wallMesh", glm::vec3(1, 0, 1));
		AddMeshToList(wallMesh);
	}

	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader* shader = new Shader("LabShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// DONE(student): After you implement the changing of the projection
	// parameters, remove hardcodings of these parameters
	//projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
	perspective = getDefaultPerspective();
	projectionMatrix = glm::perspective(perspective.fov, perspective.aspect, perspective.zNear, perspective.zFar);

	maze.generate();
	maze.printMatrix();

	switchToThirdPersonCamera();
}

void Tema2::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
	if (isGameOver)
	{
		cout << "Game over!\n";
		exit(0);
	}

	glViewport(0, 0, window->GetResolution().x, window->GetResolution().y * 7 / 8);
	ortho = defaultOrtho;
	perspective = getDefaultPerspective();
	projectionMatrix = glm::perspective(perspective.fov, perspective.aspect, perspective.zNear, perspective.zFar);
	drawFloor();
	drawPlayer();
	drawProjectiles();
	drawMaze();

	glViewport(0, window->GetResolution().y * 7 / 8, window->GetResolution().x, window->GetResolution().y / 8);
	ortho = defaultOrtho;
	projectionMatrix = glm::ortho(ortho.left, ortho.right, ortho.bottom, ortho.top, ortho.zNear, ortho.zFar);

	drawBar(true);
	drawBar(false);
}



void Tema2::FrameEnd()
{
	// DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// Render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	// translate the camera only if MOUSE_RIGHT button is pressed
	float rightDeviation = 0;
	float forwardDeviation = 0;


	Player newPlayer = Player(maze.player);

	// Prevent camera bug when rapidly pressing opposing directions 
	if (window->KeyHold(GLFW_KEY_W) && !window->KeyHold(GLFW_KEY_S))
	{
		forwardDeviation = deltaTime * 2;
		newPlayer.position.z += -forwardDeviation * cos(angle.y);
		newPlayer.position.x += -forwardDeviation * sin(angle.y);

	}
	if (window->KeyHold(GLFW_KEY_S) && !window->KeyHold(GLFW_KEY_W))
	{
		forwardDeviation = -deltaTime * 2;
		newPlayer.position.z += -forwardDeviation * cos(angle.y);
		newPlayer.position.x += -forwardDeviation * sin(angle.y);
	}
	if (window->KeyHold(GLFW_KEY_D) && !window->KeyHold(GLFW_KEY_A))
	{
		rightDeviation = deltaTime * 2;
		newPlayer.position.z += -rightDeviation * sin(angle.y);
		newPlayer.position.x += rightDeviation * cos(angle.y);
	}
	if (window->KeyHold(GLFW_KEY_A) && !window->KeyHold(GLFW_KEY_D))
	{
		rightDeviation = -deltaTime * 2;
		newPlayer.position.z += -rightDeviation * sin(angle.y);
		newPlayer.position.x += rightDeviation * cos(angle.y);
	}


	if (canMove(newPlayer))
	{
		maze.player = newPlayer;
		camera->MoveRight(rightDeviation);
		camera->MoveForward(forwardDeviation);
	}

}


void Tema2::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)
	{
		isThirdPerson = !isThirdPerson;
		if (isThirdPerson)
		{
			switchToThirdPersonCamera();
		}
		else
		{
			switchToFirstPersonCamera();
		}
	}
}

void Tema2::OnKeyRelease(int key, int mods)
{
	// Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Add mouse move event
	float sensivityOX = 0.001f;
	float sensivityOY = 0.001f;
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) || window->MouseHold(GLFW_MOUSE_BUTTON_MIDDLE))
	{
		glm::vec2 deviation;
		deviation.x = -sensivityOX * deltaY;
		deviation.y = -sensivityOY * deltaX;


		// Prevent rotating the camera upwards and downwards too much
		if (abs((int)(DEGREES(angle.x + deviation.x)) % 360) > 40)
		{
			return;
		}

		angle.x += deviation.x;
		angle.y += deviation.y;

		// cout << (int)(DEGREES(angle.x)) % 360 << " " << (int)(DEGREES(angle.y)) % 360 << "\n";

		if (isThirdPerson)
		{
			camera->TranslateRight(-0.5);
			camera->RotateThirdPerson_OX(deviation.x);
			camera->RotateThirdPerson_OY(deviation.y);
			camera->TranslateRight(+0.5);
		}
		else
		{
			camera->RotateFirstPerson_OX(deviation.x);
			camera->RotateFirstPerson_OY(deviation.y);
		}
	}


}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button press event
	if (button != GLFW_MOUSE_BUTTON_2)
	{
		return;
	}

	shootProjectile();
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{}


void Tema2::OnWindowResize(int width, int height)
{}

void Tema2::drawEnemies()
{
	std::vector<Enemy>::iterator enemy = maze.enemies.begin();
	while (enemy != maze.enemies.end())
	{
		duration<float> seconds = high_resolution_clock::now() - enemy->timeDirectionChange;

		// reused the variable for time between erase
		if (seconds.count() > enemy->intervalDirectionChange && enemy->isShot)
		{
			enemy = maze.enemies.erase(enemy);
			continue;
		}

		string meshString = "sphere";
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, enemy->position);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(enemy->radius / BIG_SPHERE_RADIUS));
		RenderSimpleMesh(meshes[meshString], shaders["LabShader"], modelMatrix, enemy->color, enemy->isShot);

		Enemy newEnemy = Enemy(*enemy);

		newEnemy.position += newEnemy.direction * newEnemy.speed;
		if (!isInside(newEnemy.getSphere(), newEnemy.boundingBox))
		{
			// bounce back
			enemy->direction = -enemy->direction;
		}
		else
		{
			*enemy = newEnemy;
			duration<float> seconds = high_resolution_clock::now() - enemy->timeDirectionChange;
			if (seconds.count() >= enemy->intervalDirectionChange)
			{
				enemy->generateDirection();
			}
		}
		++enemy;
	}
}

void Tema2::drawMaze()
{
	auto matrix = maze.matrix;
	string meshString = "";
	glm::mat4 modelMatrix = glm::mat4(1);

	for (const Box& wall : maze.walls)
	{
		meshString = "box";
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, wall.center);
		RenderMesh(meshes[meshString], shaders["VertexNormal"], modelMatrix);
	}

	drawEnemies();

	for (int idLine = 0; idLine < matrix.size(); ++idLine)
	{
		for (int idCol = 0; idCol < matrix[idLine].size(); ++idCol)
		{
			int cell = matrix[idLine][idCol];
			if (cell == maze.Road || cell == maze.PlayerStartingCell || cell == maze.Wall || cell == maze.EnemyCell)
			{
				continue;
			}

			glm::mat4 modelMatrix = glm::mat4(1);
			double x = 0, z = 0;

			modelMatrix = glm::translate(modelMatrix, glm::vec3(x, 0.5, z));
		}
	}
}


// https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
bool Tema2::checkCollision(Sphere sphere, Box box)
{
	float x = max(box.min.x, min(sphere.center.x, box.max.x));
	float y = max(box.min.y, min(sphere.center.y, box.max.y));
	float z = max(box.min.z, min(sphere.center.z, box.max.z));

	float distance = sqrt(
		(x - sphere.center.x) * (x - sphere.center.x) +
		(y - sphere.center.y) * (y - sphere.center.y) +
		(z - sphere.center.z) * (z - sphere.center.z));

	return distance < sphere.radius;
}

bool Tema2::checkCollision(Sphere sphere, Sphere other)
{
	float distance = sqrt(
		(sphere.center.x - other.center.x) * (sphere.center.x - other.center.x) +
		(sphere.center.y - other.center.y) * (sphere.center.y - other.center.y) +
		(sphere.center.z - other.center.z) * (sphere.center.z - other.center.z));

	return distance < (sphere.radius + other.radius);
}

bool Tema2::isInside(Sphere sphere, Box box)
{
	return
		sphere.center.x - sphere.radius * 2 > box.min.x
		&& sphere.center.y - sphere.radius * 2 > box.min.y
		&& sphere.center.z - sphere.radius * 2 > box.min.z
		&& sphere.center.x + sphere.radius * 2 < box.max.x
		&& sphere.center.y + sphere.radius * 2 < box.max.y
		&& sphere.center.z + sphere.radius * 2 < box.max.z;
}

bool Tema2::canMove(Player player)
{
	for (const Box& wall : maze.walls)
	{
		if (checkCollision(player.getSphere(), wall))
		{
			return false;
		}
	}
	return true;
}

void Tema2::drawBar(bool isHealth)
{
	float x = maze.player.position.x;
	float z = maze.player.position.z;

	glm::mat4 modelMatrix = glm::mat4(1);
	auto healthWireframe = modelMatrix;
	auto targetPosition = camera->GetTargetPosition();

	healthWireframe = glm::translate(healthWireframe, glm::vec3(targetPosition.x, targetPosition.y, targetPosition.z));
	healthWireframe = glm::rotate(healthWireframe, angle.y, glm::vec3(0, 1, 0));
	healthWireframe = glm::rotate(healthWireframe, angle.x - 0.2f * isThirdPerson, glm::vec3(1, 0, 0));

	string shaderType;
	float percentage = 0;
	if (isHealth)
	{
		healthWireframe = glm::translate(healthWireframe, glm::vec3(-5, 0, 0));
		shaderType = "VertexColor";
		percentage = maze.player.getHealthPercentage();
	}
	else
	{
		healthWireframe = glm::translate(healthWireframe, glm::vec3(+5, 0, 0));
		shaderType = "Simple";

		duration<float> seconds = high_resolution_clock::now() - gameStartTime;
		percentage = 1.0 - seconds.count() / gameTotalLength;
		if (percentage < 0) percentage = 0;
	}

	RenderMesh(meshes["healthWireframeMesh"], shaders[shaderType], healthWireframe);

	glm::mat4  health = healthWireframe;
	health = glm::translate(health, glm::vec3(-(1 - percentage) / 2, 0, 0));
	health = glm::scale(health, glm::vec3(-percentage, 1, 1));
	RenderMesh(meshes["healthMesh"], shaders[shaderType], health);
	if (percentage == 0)
	{
		isGameOver = true;
	}

}

void Tema2::drawPlayer()
{
	string meshString = "sphere";
	float x = maze.player.position.x;
	float z = maze.player.position.z;

	glm::mat4 modelMatrix = glm::mat4(1);

	if (!isInside(maze.player.getSphere(), maze.boundingBox))
	{
		cout << "You won!\n";
		isGameOver = true;
		return;
	}

	// Player is invisible if in first person, in order to not see the body when looking down / behind
	if (isThirdPerson)
	{
		// Global player transformations
		modelMatrix = glm::translate(modelMatrix, glm::vec3(x, 0.5, z));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5, 0.5, 0.5));
		modelMatrix = glm::rotate(modelMatrix, angle.y, glm::vec3(0, 1, 0));

		auto torsoMatrix = modelMatrix;
		torsoMatrix = glm::scale(modelMatrix, glm::vec3(0.35, 1, 0.35));
		RenderMesh(meshes["box"], shaders["VertexNormal"], torsoMatrix);

		auto legModelMatrix = modelMatrix;
		legModelMatrix = glm::translate(legModelMatrix, glm::vec3(0.15, -0.75, 0));
		legModelMatrix = glm::scale(legModelMatrix, glm::vec3(0.25, 0.5, 0.5));
		RenderMesh(meshes["box"], shaders["VertexNormal"], legModelMatrix);

		legModelMatrix = modelMatrix;
		legModelMatrix = glm::translate(legModelMatrix, glm::vec3(-0.15, -0.75, 0));
		legModelMatrix = glm::scale(legModelMatrix, glm::vec3(0.25, 0.5, 0.5));
		RenderMesh(meshes["box"], shaders["VertexNormal"], legModelMatrix);


		auto headModelMatrix = modelMatrix;
		headModelMatrix = glm::translate(headModelMatrix, glm::vec3(0, 0.60, 0));
		headModelMatrix = glm::scale(headModelMatrix, glm::vec3(0.30, 0.30, 0.30));
		RenderMesh(meshes["box"], shaders["VertexNormal"], headModelMatrix);

		auto handModelMatrix = modelMatrix;
		handModelMatrix = glm::translate(handModelMatrix, glm::vec3(-0.25, 0, 0));
		handModelMatrix = glm::scale(handModelMatrix, glm::vec3(0.25, 0.75, 0.25));
		RenderMesh(meshes["box"], shaders["VertexNormal"], handModelMatrix);

		handModelMatrix = modelMatrix;
		handModelMatrix = glm::translate(handModelMatrix, glm::vec3(0.25, 0, 0));
		handModelMatrix = glm::scale(handModelMatrix, glm::vec3(0.25, 0.75, 0.25));
		RenderMesh(meshes["box"], shaders["VertexNormal"], handModelMatrix);
	}
	std::vector<Enemy>::iterator enemy = maze.enemies.begin();
	while (enemy != maze.enemies.end())
	{
		if (checkCollision(maze.player.getSphere(), enemy->getSphere()))
		{
			enemy = maze.enemies.erase(enemy);
			maze.player.life -= 20;
			cout << "Health is now " << maze.player.life << "\n";

			if (maze.player.life == 0)
			{
				isGameOver = true;
			}
			break;
		}
		++enemy;
	}


}

void Tema2::switchToThirdPersonCamera()
{
	// Camera is intentionally shifted to right in order to see better forward
	camera->Set
	(glm::vec3(maze.player.position.x, 1.5f, maze.player.position.z + 2),
		glm::vec3(maze.player.position.x, 0, maze.player.position.z - 5.5f),
		glm::vec3(0, 1, 0));

	// Resetting angles for camera and player
	angle.x = 0;
	angle.y = 0;
	camera->MoveRight(0.5);
}

glm::vec3 Tema2::getFirstPersonPosition()
{
	return glm::vec3(maze.player.position.x, 0.5f, maze.player.position.z);
}

void Tema2::switchToFirstPersonCamera()
{
	camera->Set
	(getFirstPersonPosition(),
		glm::vec3(maze.player.position.x, 0.5f, maze.player.position.z - 5.5f),
		glm::vec3(0, 1, 0));

	angle.x = 0;
	angle.y = 0;
}

void Tema2::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	GetSceneCamera()->Update();
}

// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema2::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	return glm::transpose(glm::mat3(
		smin, 0.0f, tx,
		0.0f, smin, ty,
		0.0f, 0.0f, 1.0f));
}

void Tema2::drawFloor()
{
	// Plane has side of 50 so we first scale it to a 1x1 tile, and then enlarge it to fit the maze 
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3((0.5 + maze.matrix.size()) / 2, 0, (0.5 + maze.matrix[0].size()) / 2));
	modelMatrix = glm::scale(modelMatrix, 1.5f * glm::vec3(0.5 + maze.matrix.size(), 1, 0.5 + maze.matrix[0].size()));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.02, 1, 0.02));
	RenderMesh(meshes["plane50"], shaders["VertexNormal"], modelMatrix);
}

void Tema2::shootProjectile()
{
	if (isThirdPerson) return;

	Projectile projectile = Projectile();
	projectile.timeShot = high_resolution_clock::now();
	projectile.position = getFirstPersonPosition();
	auto target = camera->GetTargetPosition();
	projectile.direction = glm::vec3(target - projectile.position) / 100.0f;

	// Move the projectile a bit further from the body
	projectile.position += projectile.direction * projectile.speed;

	projectiles.emplace_back(projectile);
}

void Tema2::drawProjectiles()
{
	glm::mat4 modelMatrix = glm::mat4(1);

	std::vector<Projectile>::iterator projectile = projectiles.begin();

	while (projectile != projectiles.end())
	{
		std::vector<Enemy>::iterator enemy = maze.enemies.begin();
		bool collision = false;
		while (enemy != maze.enemies.end())
		{
			if (checkCollision(projectile->getSphere(), enemy->getSphere()))
			{
				enemy->isShot = true;
				enemy->timeShot = high_resolution_clock::now();
				// enemy = maze.enemies.erase(enemy);
				projectile = projectiles.erase(projectile);
				collision = true;

				break;
			}
			++enemy;
		}
		if (collision == true)
		{
			continue;
		}


		duration<float> seconds = high_resolution_clock::now() - projectile->timeShot;
		if (seconds.count() >= projectile->lifespanInSeconds)
		{
			projectile = projectiles.erase(projectile);
			continue;
		}

		bool eraseProjectile = false;
		for (const Box& wall : maze.walls)
		{
			if (checkCollision(projectile->getSphere(), wall))
			{
				projectile = projectiles.erase(projectile);
				eraseProjectile = true;
				break;
			}
		}
		if (eraseProjectile)
		{
			continue;
		}

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, projectile->position);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(projectile->radius / BIG_SPHERE_RADIUS));
		RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
		projectile->position += projectile->direction * projectile->speed;
		++projectile;
	}
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, int isExploding)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// DONE(student): Get shader location for uniform mat4 "Model"
	int location = glGetUniformLocation(shader->program, "Model");

	// DONE(student): Set shader uniform "Model" to modelMatrix
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// DONE(student): Get shader location for uniform mat4 "View"
	location = glGetUniformLocation(shader->program, "View");

	// DONE(student): Set shader uniform "View" to viewMatrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// DONE(student): Get shader location for uniform mat4 "Projection"
	location = glGetUniformLocation(shader->program, "Projection");

	// DONE(student): Set shader uniform "Projection" to projectionMatrix
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	location = glGetUniformLocation(shader->program, "enemyColor");
	glUniform3fv(location, 1, glm::value_ptr(color));

	location = glGetUniformLocation(shader->GetProgramID(), "time");
	glUniform1f(location, Engine::GetElapsedTime());
	
	location = glGetUniformLocation(shader->GetProgramID(), "isExploding");
	glUniform1i(location, isExploding);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);

}