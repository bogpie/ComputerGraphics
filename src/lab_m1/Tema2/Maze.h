#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <iostream>
#include <utility>
#include <random>
#include <algorithm>
#include <glm/ext/vector_float3.hpp>
#include <chrono>


class Sphere
{
public:
	Sphere() : center(glm::vec3(0)), radius(0.5) {}
	Sphere(glm::vec3 center, float radius) : center(center), radius(radius) {}
	glm::vec3 center;
	float radius;
};


class Box
{
public:

	Box() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)), center(glm::vec3(0.0f)) {}

	Box(glm::vec3 min, glm::vec3 max) : min(min), max(max), center((max-min) * 0.5f) {}

	Box(glm::vec3 center, float side) :
		min(glm::vec3(center.x - side / 2, center.y - side / 2, center.z - side / 2)),
		max(glm::vec3(center.x + side / 2, center.y + side / 2, center.z + side / 2)),
		center(center)
	{}

	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 center;
};


class Enemy
{
public:
	glm::vec3 position;
	Box boundingBox;
	glm::vec3 direction;
	float speed;
	float radius;
	std::chrono::steady_clock::time_point timeDirectionChange;
	bool isShot;
	std::chrono::steady_clock::time_point timeShot;
	float intervalDirectionChange;
	glm::vec3 color;

public:
	Enemy(glm::vec3 position);
	Sphere getSphere();
	void generateDirection();
};


class Player
{
public:
	glm::vec3 position;
	float life;

public:
	Player();
	Player(glm::vec3 position);
	Sphere getSphere();
	float getHealthPercentage();
};

class Maze
{

public:
	const int DEFAULT_NO_LINES = 10;
	const int DEFAULT_NO_COLS = 10;
	const int DEFAULT_MAX_NO_EXITS = 3;
	const int DEFAULT_MAX_NO_ENEMIES = 15;

	int noLines;
	int noCols;
	int MAX_NO_ENEMIES;
	int MAX_NO_EXITS;
	Box boundingBox;
	Player player;
	std::vector<std::vector<int>> matrix;
	std::vector<Box> walls;
	std::vector<Enemy> enemies;

public:
	Maze(int noLines, int noCols, int MAX_NO_ENEMIES, int MAX_NO_EXITS);

	Maze();

	enum Cell { Road, Wall, EnemyCell, PlayerStartingCell };

	void initialize();

	void printMatrix();

	bool isInside(int idLine, int idCol);

	int getNoRoads(int idLine, int idCol);

	std::vector<std::pair<int, int>> getWallNeighbours(int idLine, int idCol, int& noWallNeighboursWithBorder);

	void DFS(int idLine, int idCol);

	void generate();
};

#endif //MAZE_H