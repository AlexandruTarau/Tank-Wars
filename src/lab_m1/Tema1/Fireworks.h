#pragma once

#include "utils/glm_utils.h"
#include "transform2D.h"
#include <vector>
#include <tuple>

class Fireworks {
public:

	Fireworks(
		glm::vec3 position,
		float propagationFactor,
		int propagationNumber,
		float propagationCD,
		float scale,
		int raysNumber);

	std::vector<glm::mat3> Fire(float deltaTime);

	bool IsDead();
	
private:
	glm::vec3 position;
	float propagationFactor;
	int propagationNumber;
	float propagationCD;
	float propagationTimer;
	float scale;
	int raysNumber;
	bool isDead;
};
