#include "Tank.h"
#include "Projectile.h"
#include <iostream>

Tank::Tank(glm::vec3 position, int chunkIndex, float cannonAngle, float cannonAngleStep, int maxHealth)
{
	this->position = position;
	this->chunkIndex = chunkIndex;
    this->cannonAngle = cannonAngle;
    this->cannonOffset = glm::vec3(0.f, 3.f * height / 5.f, 0.f);
    this->cannonAngleStep = cannonAngleStep;
    this->maxHealth = maxHealth;
    this->currentHealth = maxHealth;
    this->isDead = false;
}

Tank::Tank() {}

std::tuple<glm::mat3, glm::mat3> Tank::BuildTank(std::vector<std::tuple<float, float>> heightMap)
{
    float ax = std::get<0>(heightMap[chunkIndex]);
    float bx = std::get<0>(heightMap[chunkIndex + 1]);
    float ay = std::get<1>(heightMap[chunkIndex]);
    float by = std::get<1>(heightMap[chunkIndex + 1]);
    
    float groundAngle = atan2(by - ay, bx - ax);
    float t = (position.x - ax) / (bx - ax);

    std::tuple<glm::mat3, glm::mat3> tank;

    position.y = ay + t * (by - ay);

    // Tank base
    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(position.x, position.y);
    modelMatrix *= transform2D::Rotate(groundAngle);

    std::get<0>(tank) = modelMatrix;

    // Tank Cannon
    cannonOffset.x = -3.f * height / 5.f * sin(groundAngle);
    cannonOffset.y = 3.f * height / 5.f * cos(groundAngle) + 2.f * height / 15.f;

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(position.x + cannonOffset.x, position.y + cannonOffset.y);
    modelMatrix *= transform2D::Translate(0.f, -cannonThickness / 2.f);
    modelMatrix *= transform2D::Rotate(cannonAngle * glm::pi<float>() / 180.f);
    modelMatrix *= transform2D::Translate(0.f, cannonThickness / 2.f);
    modelMatrix *= transform2D::Scale(cannonLength, cannonThickness);

    std::get<1>(tank) = modelMatrix;

    return tank;
}

void Tank::FireProjectile(float projectileSpeed, glm::vec3 gravity,
    std::vector<Projectile>& projectiles, std::vector<Projectile>& projectilesPool)
{
    float angle = (cannonAngle - 90.f) * glm::pi<float>() / 180.f;
    float cannonFireOffsetX = -cannonLength * sin(angle);
    float cannonFireOffsetY = cannonLength * cos(angle);

    glm::vec3 startPos = glm::vec3(position.x + cannonOffset.x + cannonFireOffsetX,
        position.y + cannonOffset.y + cannonFireOffsetY, 0.f);
    glm::vec3 velocity =
        glm::vec3(
            cos(cannonAngle * glm::pi<float>() / 180.f),
            sin(cannonAngle * glm::pi<float>() / 180.f),
            0) * projectileSpeed;
    
    if (!projectilesPool.empty()) {
        Projectile projectile = projectilesPool.back();
        projectile.SetPosition(startPos);
        projectile.SetVelocity(velocity);

        projectiles.push_back(projectile);
        projectilesPool.pop_back();
    }
    else {
        projectiles.push_back(Projectile(startPos, velocity));
    }
}

void Tank::UpdateCannonAngle(bool left, float deltaTime)
{
    if (left) {
        float newAngle = cannonAngle - cannonAngleStep * deltaTime;
        if (newAngle < -180) {
            newAngle = 180;
        }
        cannonAngle = newAngle;
    }
    else {
        float newAngle = cannonAngle + cannonAngleStep * deltaTime;
        if (newAngle > 180) {
            newAngle = -180;
        }
        cannonAngle = newAngle;
    }
}

void Tank::TakeDamage(int damage)
{
    currentHealth -= damage;
    if (currentHealth <= 0) {
        isDead = true;
    }
}

bool Tank::IsDead()
{
    return isDead;
}

float Tank::GetHeight()
{
    return height;
}

glm::vec3& Tank::GetPosition()
{
	return position;
}

float Tank::GetSpeed()
{
    return speed;
}

int Tank::GetChunkIndex()
{
    return chunkIndex;
}

float Tank::GetCannonAngle()
{
    return cannonAngle;
}

int Tank::GetCurrentHealth()
{
    return currentHealth;
}

int Tank::GetMaxHealth()
{
    return maxHealth;
}

glm::vec3 Tank::GetCannonOffset()
{
    return cannonOffset;
}

void Tank::SetCannonAngle(float cannonAngle)
{
    this->cannonAngle = cannonAngle;
}

void Tank::SetChunkIndex(int chunkIndex)
{
    this->chunkIndex = chunkIndex;
}

void Tank::SetPosition(glm::vec3 position)
{
	this->position = position;
}