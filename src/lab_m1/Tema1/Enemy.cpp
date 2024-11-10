#include "Enemy.h"
#include "Projectile.h"
#include <iostream>

Enemy::Enemy(
    glm::vec3 position,
    int chunkIndex,
    float cannonAngle,
    float cannonAngleStep,
    int maxHealth,
    float range,
    float shootCD)
    : Tank(position, chunkIndex, cannonAngle, cannonAngleStep, maxHealth)
{
    active = false;
    this->range = range;
    aimLeft = true;
    this->shootCD = shootCD;
    shootTimer = 0.f;
}

Enemy::Enemy() : Tank()
{
    active = false;
    aimLeft = true;
    shootTimer = 0.f;
}

void Enemy::UpdateAI(
    glm::vec3 target,
    std::vector<std::tuple<float, float>> heightMap,
    int visibleChunksNumber,
    int firstChunkIndex,
    glm::vec3 gravity,
    std::vector<Projectile>& projectiles,
    std::vector<Projectile>& projectilesPool,
    float deltaTime)
{
    glm::vec3 position = GetPosition();
    if (!active || IsDead()) {
        return;
    }

    shootTimer += deltaTime;

    if (abs(target.x - position.x) > range) {
        float newPosX;
        if (target.x < position.x) {

            // Update position
            newPosX = position.x - speed * deltaTime;
            SetPosition(glm::vec3(newPosX, position.y, position.z));

            for (int i = GetChunkIndex(); i >= firstChunkIndex; i--) {
                if (std::get<0>(heightMap[i]) <= newPosX) {
                    SetChunkIndex(i);
                    break;
                }
            }
        }
        else {

            // Update position
            newPosX = position.x + speed * deltaTime;
            SetPosition(glm::vec3(newPosX, position.y, position.z));

            for (int i = GetChunkIndex() + 1; i < visibleChunksNumber + firstChunkIndex; i++) {
                if (std::get<0>(heightMap[i]) > newPosX) {
                    SetChunkIndex(i - 1);
                    break;
                }
            }
        }
    }
    else {
        glm::vec3 intersection = GetIntersection();
        if (glm::distance(target, intersection) < colliderRadius) {
            if (shootTimer >= shootCD) {
                FireProjectile(Projectile::projectileSpeed, gravity, projectiles, projectilesPool);
                shootTimer = 0.f;
            }
        }
        else {
            if (!UpdateCannonAngle(aimLeft, deltaTime)) {
                aimLeft = !aimLeft;
            }
        }
    }
}

bool Enemy::IsActive()
{
	return active;
}
void Enemy::SetStatus(bool status)
{
	active = status;
}