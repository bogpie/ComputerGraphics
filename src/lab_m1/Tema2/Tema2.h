#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera.h"
#include "lab_m1/Tema2/Maze.h"
#include "lab_m1/Tema2/object2D_tema2.h"
#include "lab_m1/Tema2/Projectile.h"

namespace m1
{

	class Tema2 : public gfxc::SimpleScene
	{
	public:
		Tema2();
		~Tema2();

		void Init() override;

	private:


		struct ViewportSpace
		{
			ViewportSpace() : x(0), y(0), width(1), height(1) {}
			ViewportSpace(int x, int y, int width, int height)
				: x(x), y(y), width(width), height(height)
			{}
			int x;
			int y;
			int width;
			int height;
		};

		struct LogicSpace
		{
			LogicSpace() : x(0), y(0), width(1), height(1) {}
			LogicSpace(float x, float y, float width, float height)
				: x(x), y(y), width(width), height(height)
			{}
			float x;
			float y;
			float width;
			float height;
		};

		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;
		void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;
		bool checkCollision(Sphere sphere, Box box);
		bool checkCollision(Sphere sphere, Sphere other);
		bool isInside(Sphere sphere, Box box);
		bool canMove(Player player);
		void switchToThirdPersonCamera();
		void switchToFirstPersonCamera();
		void drawMaze();
		void drawPlayer();
		void drawFloor();
		void drawProjectiles();
		void drawBar(bool isHealth);
		void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear);
		glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
		void shootProjectile();
		glm::vec3 getFirstPersonPosition();
		double gameTotalLength = 60.0F; // seconds
		std::chrono::steady_clock::time_point gameStartTime;
		void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, int isExploding);
		void drawEnemies();

	protected:
		implemented::Camera* camera;
		glm::mat4 projectionMatrix;
		bool renderCameraTarget;

		// DONE(student): If you need any other class variables, define them here.
		float fov;
		bool isPerspective = true;

		struct Ortho
		{
			float left, right, bottom, top, zNear, zFar;
		};

		Ortho ortho, defaultOrtho = { -10.0f, 10.0f, -5.0f, 5.0f, 0, 100.0f };

		struct Perspective
		{
			float fov, aspect, zNear, zFar;
		};

		Perspective perspective;

		Perspective getDefaultPerspective();

		Maze maze;
		glm::vec2 angle = glm::vec2(0);
		bool isThirdPerson = true;
		bool isGameOver = false;
		std::vector<Projectile> projectiles;
		const float BIG_CUBE_SIDE = 1;
		const float BIG_SPHERE_RADIUS = 0.5;
	};

}   // namespace m1

