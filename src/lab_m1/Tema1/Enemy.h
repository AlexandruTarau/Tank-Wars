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
		float speed,
		float cannonAngleStep,
		int maxHealth,
		float shootCD,
		float attackRange,
		float detectRange);

	Enemy();

	void UpdateAI(
		std::vector<glm::vec3> targets,
		std::vector<std::tuple<float, float>> heightMap,
		int visibleChunksNumber,
		int firstChunkIndex,
		int lastChunkIndex,
		int excessChunksNumber,
		glm::vec3 gravity,
		std::vector<Projectile>& projectiles,
		std::vector<Projectile>& projectilesPool,
		float deltaTime);

	bool IsActive();

	void SetStatus(bool status);

private:
	bool active;
	float attackRange;
	float detectRange;
	bool aimLeft;
	float actionCD;
	float actionTimer;
	int action;
};
