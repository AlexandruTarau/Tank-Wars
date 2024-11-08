#include "Enemy.h"
#include "Projectile.h"
#include <iostream>

Enemy::Enemy(glm::vec3 position, int chunkIndex, float cannonAngle, float cannonAngleStep, int maxHealth)
    : Tank(position, chunkIndex, cannonAngle, cannonAngleStep, maxHealth)
{
    
}

Enemy::Enemy() : Tank() {}