#include "Fireworks.h"
#include <iostream>

Fireworks::Fireworks(
	glm::vec3 position,
	float propagationFactor,
	int propagationNumber,
	float propagationCD,
	float scale,
	int raysNumber)
{
	this->position = position;
	this->propagationFactor = propagationFactor;
	this->propagationNumber = propagationNumber;
	this->propagationCD = propagationCD;
	this->scale = scale;
	this->raysNumber = raysNumber;
	propagationTimer = 0.f;
	isDead = false;
}

std::vector<glm::mat3> Fireworks::Fire(float deltaTime)
{
	std::vector<glm::mat3> result;

	propagationTimer += deltaTime;

	if (propagationTimer >= propagationCD) {
		propagationTimer = 0.f;
		propagationNumber--;
		propagationFactor *= 2.f;
		scale *= 1.2f;
	}

	if (propagationNumber == -1) {
		isDead = true;
		return result;
	}

	for (int i = 1; i <= raysNumber; i++) {
		glm::mat3 modelMatrix = glm::mat3(1);
		float angle = 2.f * glm::pi<float>() * i / raysNumber;

		modelMatrix *= transform2D::Translate(
			position.x + propagationFactor * cos(angle), position.y + propagationFactor * sin(angle));
		modelMatrix *= transform2D::Rotate(angle);
		modelMatrix *= transform2D::Scale(scale, 4);

		result.push_back(modelMatrix);
	}

	return result;
}

bool Fireworks::IsDead()
{
	return isDead;
}