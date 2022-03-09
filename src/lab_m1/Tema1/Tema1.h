#pragma once

#include "components/simple_scene.h"
#include <chrono>

#define SMALL_SQUARE_SIDE 0.5;


namespace m1
{
	const float projectileSide = 0.5;
	const float projectileSpeed = 0.5;
	const float fireRate = 0.5;
	const float maxHealth = 100;

	const float squareSide = 2;
	const float radius = 1;

	const float minSpeed = 0.05;
	const float playerSpeed = 10;
	const float maxSpeed = 0.2;

	const float worldSizeMultiplayer = 2;

	const glm::vec2 windowMax = { 32,18 };
	const glm::vec2 logicCenter = { 16,9 };

	class Player
	{
	public:
		Player() : center(m1::logicCenter), radius(m1::radius), speed(m1::playerSpeed) {}

		Player(glm::vec2 translate, float radius, float speed)
			: center(translate), radius(radius), speed(speed)
		{}

		glm::vec2 center;
		float radius;
		float speed;
	};



	class Rectangle
	{
	public:
		Rectangle() : center({ 0,0 }), size({ 0,0 }) {}
		Rectangle(glm::vec2 center, glm::vec2 size) : center(center), size(size) {}
		glm::vec2 center;
		glm::vec2 size;

		glm::vec2 getMin()
		{
			return center - size / 2.0F;
		}

		glm::vec2 getMax()
		{
			return center + size / 2.0F;
		}
	};

	class Enemy
	{
	public:
		Enemy() : center(m1::logicCenter), squareSide(m1::squareSide), speed(playerSpeed), direction({ 0,0 }) {}
		Enemy(glm::vec2 center, float squareSide, float speed) : center(center), squareSide(squareSide), speed(speed) {}
		glm::vec2 center;
		glm::vec2 direction;
		float squareSide;
		float speed;

		Rectangle getRectangle()
		{
			return Rectangle(center, { squareSide, squareSide });
		}
	};

	class Projectile
	{
	public:
		Projectile() : direction({ 0.0 }), center(logicCenter), fireDistance(fireDistance) {}
		Projectile(glm::vec2 direction, glm::vec2 center) : direction(direction), center(center) {}
		glm::vec2 direction;
		glm::vec2 center;
		float fireDistance;

		Rectangle getRectangle()
		{
			return Rectangle(center, { projectileSide, projectileSide });
		}
	};


	class Obstacle
	{
	public:
		Obstacle() : center({ 0, 0 }), scale({ 1, 1 }) {}
		glm::vec2 center;
		glm::vec2 scale;

		glm::vec2 getSize()
		{
			return squareSide * scale;
		}

		glm::vec2 getMin()
		{
			return center - getSize() / 2.0F;
		}

		glm::vec2 getMax()
		{
			return center + getSize() / 2.0F;
		}

		Rectangle getRectangle()
		{
			return Rectangle(center, getSize());
		}
	};

	class Tema1 : public gfxc::SimpleScene
	{
	public:
		Tema1();
		~Tema1();

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

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);
		void DrawScene(glm::mat3 visMatrix);
		void DrawProjectiles(glm::mat3 visMatrix);
		void DrawBorder(glm::mat3 visMatrix);
		void DrawPlayer(glm::mat3 modelMatrix);
		void DrawObstacles(glm::mat3 visMatrix);
		void InitializeObstacles();
		bool CheckCollision(Player player, Rectangle rectangle);
		bool CheckCollision(Rectangle r1, Rectangle r2);
		bool CanMove(Player player);
		float RandomFloat(float minimum, float maximum);
		void DrawEnemy(glm::mat3 modelMatrix, std::vector<Enemy>::iterator& enemy);
		void DrawEnemies(glm::mat3 visMatrix);
		void DrawHealth(glm::mat3 modelMatrix);
		void GenerateEnemy();


		glm::mat3 VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
		glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);

	protected:
		glm::mat3 modelMatrix;
		glm::vec3 center;

		float scaleX, scaleY;
		float angularStep;
		float bonusAngularStepCenter;
		float bonusAngularStepOutside;

		// TODO(student): If you need any other class variables, define them here.
		float signTranslate = 1;
		float signScale = 1;
		//float signRotate = 1;
		float xMinTranslate = 10;
		float xMaxTranslate = 500;
		float scaleMin = 0;
		float scaleMax = 2;


		float lookAngle;
		glm::ivec2 resolution;
		ViewportSpace viewSpace;
		LogicSpace logicSpace;
		glm::mat3 visMatrix;

		std::vector<Projectile> projectiles;
		std::vector<Enemy> enemies;

		std::chrono::steady_clock::time_point lastTimeShot;
		std::chrono::steady_clock::time_point lastTimeEnemy;
		std::chrono::steady_clock::time_point timeDeath;
		std::vector<Obstacle> obstacles;

		Player player;
		Rectangle border;
		Rectangle collisionBorder;
		float healthValue = maxHealth;
		int score = 0;
		bool gameOver = false;
	};
}   // namespace m1
