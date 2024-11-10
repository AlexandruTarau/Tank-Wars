#pragma once

#include "utils/glm_utils.h"
#include "transform2D.h"
#include <tuple>
#include <vector>
#include "Tank.h"

class Projectile;

class Enemy : public Tank {
public:
	Enemy(
		glm::vec3 position,
		int chunkIndex,
		float cannonAngle,
		float cannonAngleStep,
		int maxHealth,
		float range,
		float shootCD);

	Enemy();

	void UpdateAI(
		glm::vec3 target,
		std::vector<std::tuple<float, float>> heightMap,
		int visibleChunksNumber,
		int firstChunkIndex,
		glm::vec3 gravity,
		std::vector<Projectile>& projectiles,
		std::vector<Projectile>& projectilesPool,
		float deltaTime);

	bool IsActive();

	void SetStatus(bool status);

private:
	bool active;
	float range;
	bool aimLeft;
	float shootCD;
	float shootTimer;
};
