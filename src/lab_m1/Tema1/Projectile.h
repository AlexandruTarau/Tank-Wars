#pragma once

#include "utils/glm_utils.h"
#include "transform2D.h"
#include <vector>
#include <tuple>

class Tank;
class Enemy;

class Projectile {
public:
	static constexpr float projectileSpeed = 600.0f;
	static constexpr float projectileRadius = 3.0f;
	static constexpr float explosionRadius = 50.0f;
	static constexpr int damage = 10;

	Projectile(glm::vec3 position, glm::vec3 velocity);

	glm::mat3 BuildProjectile(std::vector<std::tuple<float, float>> heightMap);

	bool Update(
		std::vector<std::tuple<float, float>>& heightMap,
		int visibleChunksNumber,
		int firstChunkIndex,
		int lastChunkIndex,
		int excessChunksNumber,
		Tank& tank1, Tank& tank2,
		std::vector<Enemy>& enemies,
		std::vector<Projectile>& projectiles,
		std::vector<Projectile>& projectilesPool,
		glm::vec3 gravity,
		glm::ivec2 resolution,
		float deltaTime);

	glm::vec3 GetPosition();

	void SetPosition(glm::vec3 position);

	void SetVelocity(glm::vec3 velocity);

private:
	glm::vec3 position;
	glm::vec3 velocity;
};
