#pragma once

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <glm/ext/vector_float3.hpp>
#include <chrono>
#include "lab_m1/Tema2/Maze.h"

class Projectile
{
public:
	Projectile();

	glm::vec3 position;
	glm::vec3 direction;
	float radius;
	float speed;
	std::chrono::steady_clock::time_point timeShot;
	float lifespanInSeconds;

	Sphere getSphere();
};


#endif PROJECTILE_H