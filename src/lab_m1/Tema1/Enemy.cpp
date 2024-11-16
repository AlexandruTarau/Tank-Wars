#include "Enemy.h"
#include "Projectile.h"
#include <iostream>

Enemy::Enemy(
    glm::vec3 position,
    int chunkIndex,
    float speed,
    float cannonAngleStep,
    int maxHealth,
    float shootCD,
    float attackRange,
    float detectRange)
    : Tank(position, chunkIndex, speed, cannonAngleStep, maxHealth, shootCD)
{
    active = false;
    this->attackRange = attackRange;
    this->detectRange = detectRange;
    aimLeft = true;
    shootTimer = 0.f;
    actionTimer = 0.f;
    actionCD = 0.f;
    action = -1;
}

Enemy::Enemy() : Tank()
{
    active = false;
    aimLeft = true;
    shootTimer = 0.f;
    actionTimer = 0.f;
    actionCD = 0.f;
    action = -1;
}

void Enemy::UpdateAI(
    std::vector<glm::vec3> targets,
    std::vector<std::tuple<float, float>> heightMap,
    int visibleChunksNumber,
    int firstChunkIndex,
    int lastChunkIndex,
    int excessChunksNumber,
    glm::vec3 gravity,
    std::vector<Projectile>& projectiles,
    std::vector<Projectile>& projectilesPool,
    float deltaTime)
{
    glm::vec3 position = GetPosition();
    glm::vec3 target = targets[0];
    float distance = detectRange, minDistance = detectRange;
    float newPosX;
    float speed = GetSpeed();

    for (int i = 0; i < targets.size(); i++) {
        distance = abs(targets[i].x - position.x);

        if (distance < minDistance) {
            minDistance = distance;
            target = targets[i];
        }
    }

    distance = minDistance;

    if (!active || IsDead() ||
        position.x < std::get<0>(heightMap[firstChunkIndex]) ||
        position.x > std::get<0>(heightMap[firstChunkIndex + visibleChunksNumber])) {
        return;
    }

    shootTimer += deltaTime;
    actionTimer += deltaTime;

    if (distance > attackRange && distance < detectRange) {  // Chase State
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
    else if (distance < attackRange) {  // Attack State
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
    else {  // Idle State
        if (actionTimer >= actionCD) {
            srand(time(0));
            action = action < 2 && action != -1 ? -1 : rand() % 4;
            srand(time(0) + 69);

            actionCD = 1.f + rand() % 32 / 10.f;
            actionTimer = 0.f;
        }

        switch (action) {
        case 0: {  // Move left
            // Update position
            newPosX = position.x - speed / 2.f * deltaTime;
            
            if (newPosX >= 0) {
                SetPosition(glm::vec3(newPosX, position.y, position.z));

                for (int i = GetChunkIndex(); i >= firstChunkIndex; i--) {
                    if (std::get<0>(heightMap[i]) <= newPosX) {
                        SetChunkIndex(i);
                        break;
                    }
                }
            }
            break;
        }
        case 1: {  //Move right
            // Update position
            newPosX = position.x + speed / 2.f * deltaTime;
            float lastChunkPosX = std::get<0>(heightMap[lastChunkIndex]);

            if (newPosX <= lastChunkPosX - excessChunksNumber) {
                SetPosition(glm::vec3(newPosX, position.y, position.z));

                for (int i = GetChunkIndex() + 1; i < visibleChunksNumber + firstChunkIndex; i++) {
                    if (std::get<0>(heightMap[i]) > newPosX) {
                        SetChunkIndex(i - 1);
                        break;
                    }
                }
            }
            break;
        }
        case 2: {
            UpdateCannonAngle(true, deltaTime);
            break;
        }
        case 3: {
            UpdateCannonAngle(false, deltaTime);
            break;
        }
        default: {  // Do nothing
            break;
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