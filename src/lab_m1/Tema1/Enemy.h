#pragma once

#include "utils/glm_utils.h"
#include "transform2D.h"
#include <tuple>
#include <vector>
#include "Tank.h"

class Projectile;

class Enemy : public Tank {
public:
	Enemy(glm::vec3 position, int chunkIndex, float cannonAngle, float cannonAngleStep, int maxHealth);

	Enemy();


private:
};
