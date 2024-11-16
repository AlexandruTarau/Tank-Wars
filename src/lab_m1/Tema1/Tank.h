#pragma once

#include "utils/glm_utils.h"
#include "transform2D.h"
#include <tuple>
#include <vector>

class Projectile;

class Tank {
public:
	static constexpr float height = 30.0f;
	static constexpr float width = 40.0f;
	static constexpr float radius = 15.0f;
	static constexpr float cannonLength = 30.0f;
	static constexpr float cannonThickness = 4.0f;
	static constexpr float colliderRadius = 20.0f;

	Tank(glm::vec3 position, int chunkIndex, float speed, float cannonAngleStep, int maxHealth, float shootCD);

	Tank();

	std::tuple<glm::mat3, glm::mat3> BuildTank(std::vector<std::tuple<float, float>> heightMap);

	glm::vec3& GetPosition();

	float GetAngle();

	int GetChunkIndex();

	float GetSpeed();

	float GetCannonAngle();

	int GetCurrentHealth();

	int GetMaxHealth();

	glm::vec3 GetIntersection();
	
	glm::vec3 GetCannonOffset();
	
	void SetPosition(glm::vec3 position);

	void SetChunkIndex(int chunkIndex);

	void SetSpeed(float speed);

	void SetIntersection(glm::vec3 intersection);

	void SetCannonAngle(float cannonAngle);

	void SetCannonAngleStep(float step);

	void FireProjectile(float projectileSpeed, glm::vec3 gravity,
		std::vector<Projectile>& projectiles, std::vector<Projectile>& projectilesPool);

	bool UpdateCannonAngle(bool left, float deltaTime);

	void TakeDamage(int damage);

	bool IsDead();

private:
	glm::vec3 position;
	float angle;
	int chunkIndex;
	float speed;
	float cannonAngle;
	float cannonAngleStep;
	glm::vec3 cannonOffset;
	int maxHealth;
	int currentHealth;
	bool isDead;
	glm::vec3 intersection;

public:
	float shootCD;
	float shootTimer;
};
