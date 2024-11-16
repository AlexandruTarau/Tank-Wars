#include "Projectile.h"
#include "Tank.h"
#include "Enemy.h"
#include <iostream>

Projectile::Projectile(glm::vec3 position, glm::vec3 velocity)
{
	this->position = position;
	this->velocity = velocity;
}

glm::mat3 Projectile::BuildProjectile(std::vector<std::tuple<float, float>> heightMap)
{
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(position.x, position.y);
	modelMatrix *= transform2D::Scale(projectileRadius, projectileRadius);

	return modelMatrix;
}

bool Projectile::Update(
	std::vector<std::tuple<float, float>>& heightMap,
	int visibleChunksNumber,
	int firstChunkIndex,
	int lastChunkIndex,
	int excessChunksNumber,
	Tank& tank1,
	Tank& tank2,
	std::vector<Enemy>& enemies,
	std::vector<Projectile>& projectiles,
	std::vector<Projectile>& projectilesPool,
	glm::vec3 gravity,
	glm::ivec2 resolution,
	float deltaTime)
{
	float ax = 0.f, ay = 0.f, bx = 0.f, by = 0.f, t = 0.f;
	float firstChunkPosX = std::get<0>(heightMap[firstChunkIndex]);
	float lastChunkPosX = firstChunkPosX + resolution.x;

	position += velocity * deltaTime;
	velocity -= gravity * deltaTime;

	for (int i = std::max(0, firstChunkIndex - excessChunksNumber); i < std::min(visibleChunksNumber + firstChunkIndex + excessChunksNumber, lastChunkIndex) - 1; i++) {
		if (std::get<0>(heightMap[i]) <= position.x && std::get<0>(heightMap[i + 1]) >= position.x) {
			ax = std::get<0>(heightMap[i]);
			ay = std::get<1>(heightMap[i]);
			bx = std::get<0>(heightMap[i + 1]);
			by = std::get<1>(heightMap[i + 1]);
			break;
		}
	}
	t = (position.x - ax) / (bx - ax);
	glm::vec3 iPoint = glm::vec3(ax, ay, 0) * (1 - t) + glm::vec3(bx, by, 0) * t;

	// Out of map
	if (position.y < 0 || position.x < firstChunkPosX - excessChunksNumber || position.x > lastChunkPosX + excessChunksNumber) {
		return false;
	}

	// Collision with terrain
	if (position.y - iPoint.y < 1.f) {
		for (int i = std::max(0, firstChunkIndex - excessChunksNumber); i < std::min(visibleChunksNumber + firstChunkIndex + excessChunksNumber, lastChunkIndex); i++) {
			ax = std::get<0>(heightMap[i]);
			ay = std::get<1>(heightMap[i]);
			float dx = ax - position.x;
			float dy = ay - position.y;

			// Detect points inside the radius
			if (ax > position.x - Projectile::explosionRadius && ax < position.x + Projectile::explosionRadius) {
				float newY = position.y - sqrt(Projectile::explosionRadius * Projectile::explosionRadius - dx * dx);

				if (newY < ay) {
					std::get<1>(heightMap[i]) = newY;
				}
			}
		}
		return true;
	}

	// Collision with tanks
	glm::vec3 tank1Pos = tank1.GetPosition();
	glm::vec3 tank2Pos = tank2.GetPosition();
	float distanceT1 = sqrt((position.x - tank1Pos.x) * (position.x - tank1Pos.x) +
		(position.y - tank1Pos.y) * (position.y - tank1Pos.y));
	float distanceT2 = sqrt((position.x - tank2Pos.x) * (position.x - tank2Pos.x) +
		(position.y - tank2Pos.y) * (position.y - tank2Pos.y));

	if (!tank1.IsDead() && distanceT1 < projectileRadius + Tank::colliderRadius) {
		tank1.TakeDamage(damage);

		return true;
	}
	if (!tank2.IsDead() && distanceT2 < projectileRadius + Tank::colliderRadius) {
		tank2.TakeDamage(damage);

		return true;
	}

	for (int i = 0; i < enemies.size(); i++) {
		glm::vec3 enemyPos = enemies[i].GetPosition();
		float distance = sqrt((position.x - enemyPos.x) * (position.x - enemyPos.x) +
			(position.y - enemyPos.y) * (position.y - enemyPos.y));

		if (!enemies[i].IsDead() && distance < projectileRadius + Tank::colliderRadius) {
			enemies[i].TakeDamage(damage);

			return true;
		}
	}

	return false;
}

glm::vec3 Projectile::GetPosition()
{
	return position;
}

void Projectile::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void Projectile::SetVelocity(glm::vec3 velocity)
{
	this->velocity = velocity;
}
