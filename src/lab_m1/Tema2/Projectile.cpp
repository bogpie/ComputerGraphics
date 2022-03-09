#include "Projectile.h"

Projectile::Projectile()
{
	direction = glm::vec3(0);
	position = glm::vec3(0);
	speed = 5;
	radius = 0.025;
	lifespanInSeconds = 1.0;
}

Sphere Projectile::getSphere()
{
	return Sphere(position, radius);
}
