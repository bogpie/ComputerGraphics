#include "lab_m1/Tema1/Tema1.h"
#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D_tema.h"

#include <vector>
#include <iostream>


using namespace std;
using namespace chrono;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{}


Tema1::~Tema1()
{}


void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
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

// 2D visualization matrix
glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	tx = viewSpace.x - sx * logicSpace.x;
	ty = viewSpace.y - sy * logicSpace.y;

	return glm::transpose(glm::mat3(
		sx, 0.0f, tx,
		0.0f, sy, ty,
		0.0f, 0.0f, 1.0f));
}


// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
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


void Tema1::DrawEnemy(glm::mat3 modelMatrix, std::vector<Enemy>::iterator& enemy)
{
	glm::mat3 bodyModelMatrix = modelMatrix;
	glm::mat3 handModelMatrix = modelMatrix;

	std::vector<Projectile>::iterator projectile = projectiles.begin();
	bool deleteEnemy = false;
	while (projectile != projectiles.end())
	{
		if (CheckCollision(projectile->getRectangle(), enemy->getRectangle()) && !gameOver)
		{
			enemy = enemies.erase(enemy);
			projectile = projectiles.erase(projectile);
			deleteEnemy = true;
			score += 10;
			cout << "Score is now " << score << "!\n\n";
			break;
		}
		++projectile;
	}


	if (deleteEnemy)
	{
		return;
	}

	if (CheckCollision(player, enemy->getRectangle()) && !gameOver)
	{
		deleteEnemy = true;
		enemy = enemies.erase(enemy);
		healthValue = healthValue != 0 ? healthValue - 10 : 0;
		cout << "Your health dropped to " << healthValue << "\n\n";
		if (healthValue == 0)
		{
			gameOver = true;
			cout << "You died!\n";
			timeDeath = high_resolution_clock::now();
		}
	}

	if (deleteEnemy)
	{
		return;
	}
	++enemy;

	glm::mat3 leftHandModelMatrix = handModelMatrix * transform2D::Translate(-squareSide / 2, -squareSide / 2);
	leftHandModelMatrix *= transform2D::Scale(0.3, 0.3);
	RenderMesh2D(meshes["enemyMesh"], shaders["VertexColor"], leftHandModelMatrix);

	glm::mat3 rightHandModelMatrix = handModelMatrix * transform2D::Translate(-squareSide / 2, +squareSide / 2);
	rightHandModelMatrix *= transform2D::Scale(0.3, 0.3);
	RenderMesh2D(meshes["enemyMesh"], shaders["VertexColor"], rightHandModelMatrix);

	RenderMesh2D(meshes["enemyMesh"], shaders["VertexColor"], bodyModelMatrix);
}

void Tema1::GenerateEnemy()
{
	glm::vec2 absDeviation, deviation, sign;
	float speed;

	float minimumDeviation = squareSide * 3;
	float maximumDeviation = border.size.x / 2;
	absDeviation.x = RandomFloat(minimumDeviation, maximumDeviation);
	sign.x = rand() % 2 == 0 ? -1 : 1;


	minimumDeviation = squareSide * 3;
	maximumDeviation = border.size.y / 2;
	absDeviation.y = RandomFloat(minimumDeviation, maximumDeviation);
	sign.y = rand() % 2 == 0 ? -1 : 1;

	deviation = absDeviation * sign;
	speed = RandomFloat(minSpeed, maxSpeed);

	enemies.push_back(Enemy(logicCenter + deviation, squareSide, speed));
	return;
}

void Tema1::DrawEnemies(glm::mat3 visMatrix)
{
	auto now = high_resolution_clock::now();
	duration<float> seconds = now - lastTimeEnemy;
	if (seconds.count() >= 3)
	{
		GenerateEnemy();
		lastTimeEnemy = now;
	}

	std::vector<Enemy>::iterator enemy = enemies.begin();

	while (enemy != enemies.end())
	{
		float centersAngle = (float)atan2(
			(double)enemy->center.y - (double)player.center.y,
			(double)enemy->center.x - (double)player.center.x
		);

		enemy->direction.x = -cos(centersAngle) * enemy->speed;
		enemy->direction.y = -sin(centersAngle) * enemy->speed;

		enemy->center.x += enemy->direction.x;
		enemy->center.y += enemy->direction.y;

		modelMatrix = visMatrix;
		modelMatrix *= transform2D::Translate(enemy->center.x, enemy->center.y);
		modelMatrix *= transform2D::Rotate(centersAngle);
		DrawEnemy(modelMatrix, enemy);
	}
}


void Tema1::DrawProjectiles(glm::mat3 visMatrix)
{
	std::vector<Projectile>::iterator projectile = projectiles.begin();

	while (projectile != projectiles.end())
	{
		projectile->center.x += projectile->direction.x;
		projectile->center.y += projectile->direction.y;

		modelMatrix = visMatrix;
		modelMatrix *= transform2D::Translate(projectile->center.x, projectile->center.y);

		bool deleteProjectile = false;

		for (Obstacle obstacle : obstacles)
		{
			if (CheckCollision(projectile->getRectangle(), obstacle.getRectangle()))
			{
				projectile = projectiles.erase(projectile);
				deleteProjectile = true;
				break;
			}
		}
		if (deleteProjectile) continue;

		if (!CheckCollision(projectile->getRectangle(), border))
		{
			projectile = projectiles.erase(projectile);
			deleteProjectile = true;
		}
		if (deleteProjectile) continue;

		RenderMesh2D(meshes["projectileMesh"], shaders["VertexColor"], modelMatrix);
		++projectile;
	}

	return;
}

void Tema1::DrawBorder(glm::mat3 visMatrix)
{
	modelMatrix = visMatrix;
	//modelMatrix *= transform2D::Translate(logicCenter.x, logicCenter.y);
	modelMatrix *= transform2D::Translate(logicCenter.x, logicCenter.y);

	RenderMesh2D(meshes["borderMesh"], shaders["VertexColor"], modelMatrix);
	return;
}

void Tema1::DrawPlayer(glm::mat3 modelMatrix)
{
	glm::mat3 bodyModelMatrix = modelMatrix;
	glm::mat3 handModelMatrix = modelMatrix;

	glm::mat3 leftHandModelMatrix = handModelMatrix * transform2D::Translate(radius * cos(AI_MATH_PI / 6), radius * sin(AI_MATH_PI / 6));
	leftHandModelMatrix *= transform2D::Scale(0.3, 0.3);
	RenderMesh2D(meshes["circle"], shaders["VertexColor"], leftHandModelMatrix);

	glm::mat3 rightHandModelMatrix = handModelMatrix * transform2D::Translate(radius * cos(-AI_MATH_PI / 6), radius * sin(-AI_MATH_PI / 6));
	rightHandModelMatrix *= transform2D::Scale(0.3, 0.3);
	RenderMesh2D(meshes["circle"], shaders["VertexColor"], rightHandModelMatrix);

	RenderMesh2D(meshes["circle"], shaders["VertexColor"], bodyModelMatrix);
}



void Tema1::DrawObstacles(glm::mat3 visMatrix)
{
	glm::mat3 modelMatrix;

	for (Obstacle obstacle : obstacles)
	{
		modelMatrix = visMatrix;

		modelMatrix *= transform2D::Translate(obstacle.center.x, obstacle.center.y);
		modelMatrix *= transform2D::Scale(obstacle.scale.x, obstacle.scale.y);


		RenderMesh2D(meshes["obstacleMesh"], shaders["VertexColor"], modelMatrix);
	}
}

void Tema1::DrawHealth(glm::mat3 visMatrix)
{
	modelMatrix = visMatrix;

	modelMatrix *= transform2D::Translate(player.center.x, player.center.y);
	modelMatrix *= transform2D::Translate(windowMax.x / 2.5, windowMax.y / 2.5);

	glm::mat3 healthWireframeModelMatrix = modelMatrix;
	modelMatrix *= transform2D::Translate((healthValue - maxHealth) / 50, 0);
 	modelMatrix *= transform2D::Scale(healthValue / 50, 0.5);
	
	RenderMesh2D(meshes["healthMesh"], shaders["VertexColor"], modelMatrix);

	healthWireframeModelMatrix *= transform2D::Scale(maxHealth / 50, 0.5);
	RenderMesh2D(meshes["healthWireframeMesh"], shaders["VertexColor"], healthWireframeModelMatrix);

}

void Tema1::DrawScene(glm::mat3 visMatrix)
{
	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawHealth(visMatrix);
	DrawEnemies(visMatrix);

	if (!gameOver)
	{
		DrawProjectiles(visMatrix);
	}
	DrawBorder(visMatrix);
	DrawObstacles(visMatrix);

	modelMatrix = visMatrix;

	modelMatrix *= transform2D::Translate(player.center.x, player.center.y);

	glm::ivec2 reversedCursorPosition = window->GetCursorPosition();
	glm::vec2 cursorPosition = { (float)reversedCursorPosition.x, (float)(resolution.y - reversedCursorPosition.y) };

	lookAngle = (float)atan2(
		(double)cursorPosition.y - resolution.y / 2,
		(double)cursorPosition.x - resolution.x / 2);


	modelMatrix *= transform2D::Rotate(lookAngle);


	if (!gameOver)
	{
		DrawPlayer(modelMatrix);
	}
}


float Tema1::RandomFloat(float minimum, float maximum)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float variation = random * (maximum - minimum);
	return minimum + variation;
}

void Tema1::InitializeObstacles()
{
	int noObstacles = rand() % 12 + 6;
	for (int idObstacle = 0; idObstacle < noObstacles; ++idObstacle)
	{
		Obstacle obstacle = Obstacle();

		obstacle.scale.x = RandomFloat(1, 3);
		obstacle.scale.y = RandomFloat(1, 3);

		glm::vec2 absDeviation, deviation, sign;


		float minimumDeviation = radius + obstacle.getSize().x / 2;
		float maximumDeviation = border.size.x / 2;
		absDeviation.x = RandomFloat(minimumDeviation, maximumDeviation);
		sign.x = rand() % 2 == 0 ? -1 : 1;

		minimumDeviation = radius + obstacle.getSize().y / 2;
		maximumDeviation = border.size.y / 2;
		absDeviation.y = RandomFloat(minimumDeviation, maximumDeviation);
		sign.y = rand() % 2 == 0 ? -1 : 1;

		deviation = absDeviation * sign;
		obstacle.center = logicCenter + deviation;

		obstacles.push_back(obstacle);
	}
}

bool Tema1::CheckCollision(Rectangle r1, Rectangle r2)
{
	glm::bvec2 collision;

	collision.x =
		r1.getMax().x >= r2.getMin().x &&
		r2.getMax().x >= r1.getMin().x;

	collision.y =
		r1.getMax().y >= r2.getMin().y &&
		r2.getMax().y >= r1.getMin().y;

	return collision.x && collision.y;
}

bool Tema1::CheckCollision(Player player, Rectangle rectangle)
{
	// calculate AABB info (center, half-extents)
	glm::vec2 rectangleHalfExtents(rectangle.size.x / 2.0f, rectangle.size.y / 2.0f);


	// get difference vector between both centers
	glm::vec2 centerDifference = player.center - rectangle.center;
	glm::vec2 clamped = glm::clamp(centerDifference, -rectangleHalfExtents, +rectangleHalfExtents);

	// add clamped value to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = rectangle.center + clamped;

	return glm::length(closest - player.center) < player.radius;
}

bool Tema1::CanMove(Player player)
{
	// If the player collides with the border (is inside it) it can move
	// It should be inside it as a whole object
	bool canMove = CheckCollision(player, collisionBorder);
	if (!canMove) return false;


	// If the new position defines a collision any obstacle, that means the player shouldn't have moved there
	for (Obstacle obstacle : obstacles)
	{
		canMove = !CheckCollision(player, obstacle.getRectangle());
		if (!canMove) return false;
	}

	return true;
}


void Tema1::Init()
{
	resolution = window->GetResolution(); // la fiecare frame onWindowResize

	auto camera = GetSceneCamera();
	//camera->SetOrthographic(-(float)resolution.x / 2, (float)resolution.x / 2, -(float)resolution.y / 2, (float)resolution.y / 2, 0.01f, 400); // set object in center
	//camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	//glm::vec3 corner = glm::vec3(0, 0, 0);
	center = glm::vec3(0, 0, 0);

	// Initialize sx and sy (the scale factors)
	scaleX = 1;
	scaleY = 1;

	// Initialize angularStep
	angularStep = 0;
	bonusAngularStepCenter = 0;
	bonusAngularStepOutside = 0;

	lookAngle = 0;

	player = Player();

	Mesh* playerMesh = object2D_tema::CreateCircle("circle", center, player.radius, glm::vec3(1, 0.75, 0.25), true);
	AddMeshToList(playerMesh);

	Mesh* projectileMesh = object2D_tema::CreateProjectile("projectileMesh", center);
	AddMeshToList(projectileMesh);

	Mesh* borderMesh = object2D_tema::CreateBorder("borderMesh", center, worldSizeMultiplayer, glm::vec3(0, 0, 0));
	AddMeshToList(borderMesh);
	border = Rectangle(
		logicCenter,
		windowMax * worldSizeMultiplayer
	);
	collisionBorder = Rectangle(
		logicCenter,
		border.size - 4 * player.radius
	);

	Mesh* obstacleMesh = object2D_tema::CreateSquareFromCenter("obstacleMesh", center, squareSide, glm::vec3(0.5, 0, 0.5), true);
	AddMeshToList(obstacleMesh);

	Mesh* enemyMesh = object2D_tema::CreateSquareFromCenter("enemyMesh", center, squareSide, glm::vec3(1, 0.2, 0.3), true);
	AddMeshToList(enemyMesh);

	// Will be scaled
	Mesh* healthMesh = object2D_tema::CreateSquareFromCenter("healthMesh", center, squareSide, glm::vec3(0, 1, 0), true);
	AddMeshToList(healthMesh);

	Mesh* healthWireframeMesh= object2D_tema::CreateSquareFromCenter("healthWireframeMesh", center, squareSide, glm::vec3(0, 0, 0), false);
	AddMeshToList(healthWireframeMesh);

	logicSpace.x = 0;       // logic x
	logicSpace.y = 0;       // logic y
	logicSpace.width = windowMax.x;   // logic width
	logicSpace.height = windowMax.y;  // logic height

	lastTimeShot = high_resolution_clock::now();
	lastTimeEnemy = lastTimeShot;

	InitializeObstacles();

}


void Tema1::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{
	if (gameOver)
	{
		auto now = high_resolution_clock::now();
		duration<float> seconds = now - timeDeath;
		if (seconds.count() >= 2.0)
		{
			cout << "Game over! \n";
			exit(0);
		}

	}


	resolution = window->GetResolution(); // la fiecare frame onWindowResize

	// Sets the screen area where to draw - the left half of the window
	viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
	SetViewportArea(viewSpace, glm::vec3(0), true);

	// Compute the 2D visualization matrix
	visMatrix = glm::mat3(1);

	visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

	DrawScene(visMatrix);

}


void Tema1::FrameEnd()
{}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	glm::vec2 newPlayerCenter = player.center;
	LogicSpace newLogicSpace = logicSpace;

	if (window->KeyHold(GLFW_KEY_S))
	{
		newPlayerCenter.y -= deltaTime * playerSpeed;
		newLogicSpace.y -= deltaTime * playerSpeed;
	}

	if (window->KeyHold(GLFW_KEY_A))
	{
		newPlayerCenter.x -= deltaTime * playerSpeed;
		newLogicSpace.x -= deltaTime * playerSpeed;
	}

	if (window->KeyHold(GLFW_KEY_W))
	{
		newPlayerCenter.y += deltaTime * playerSpeed;
		newLogicSpace.y += deltaTime * playerSpeed;
	}

	if (window->KeyHold(GLFW_KEY_D))
	{
		newPlayerCenter.x += deltaTime * playerSpeed;
		newLogicSpace.x += deltaTime * playerSpeed;
	}

	Player newPlayer = player;
	newPlayer.center = newPlayerCenter;

	if (CanMove(newPlayer))
	{
		player = newPlayer;
		logicSpace = newLogicSpace;
	}
}


void Tema1::OnKeyPress(int key, int mods)
{

}


void Tema1::OnKeyRelease(int key, int mods)
{
	// Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Add mouse move event

}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{

	// Add mouse button press event
	// Add key press event
	glm::vec2 direction = { 0,0 };
	Projectile projectile = Projectile();
	projectile.center = player.center;
	projectile.direction.x = cos(lookAngle) * projectileSpeed;
	projectile.direction.y = sin(lookAngle) * projectileSpeed;

	auto newTimeShot = high_resolution_clock::now();
	duration<float> seconds = newTimeShot - lastTimeShot;
	if (seconds.count() >= 0.5)
	{
		lastTimeShot = newTimeShot;
		projectiles.push_back(projectile);
	}
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{}


void Tema1::OnWindowResize(int width, int height)
{}

