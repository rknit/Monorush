#ifndef ENEMY_H
#define ENEMY_H

#include "ScriptableEntity.h"
#include "GameManager.h"

class EnemyController : public ScriptableEntity {

public:

	int healthPoint = 10;
	float speed = 2;
	float turnSpeed = 2;
	bool run_once = false;
	bool death = false;

	float disposeWaitTime = 0.4f, _counter = 0;

	void Hurt(int damage) {
		healthPoint -= damage;
		healthPoint = glm::max(0, healthPoint);

		auto& tag = GetComponent<TagComponent>();
		std::cout << tag.name << " : health " << healthPoint << ", " << damage << " damage\n";
	}

	void OnCreate() {
		healthPoint += (int)(GameManager::difficulty * 6);
		speed += (int)(GameManager::difficulty * 0.3f);
		turnSpeed += (int)(glm::pow(GameManager::difficulty, 2) * 0.3f);
		turnSpeed = glm::min<float>(turnSpeed, 10);

		auto& tag = GetComponent<TagComponent>();
		std::cout << tag.name << " : health " << healthPoint << ", speed " << speed << ", turnSpeed " << turnSpeed << "\n";
	}

	void OnUpdate(Time time) {

		auto& playerTransform = FindEntityOfName("Player").GetComponent<TransformComponent>();
		auto& transform = GetComponent<TransformComponent>();
		auto& rigidBody = GetComponent<RigidbodyComponent>();
		auto& collider = GetComponent<CollisionComponent>();

		glm::vec3 direction = playerTransform.position - transform.position;
		if (GameManager::gameOver) {
			if (!run_once) {
				run_once = true;
				collider.active = false;
			}
			direction *= -1;
			speed += time.deltaTime;
		}

		if (healthPoint <= 0) {
			if (!death) {
				death = true;
				GetComponent<SpriteSheetComponent>().DrawAtIndex(1);
				GetComponent<SpriteRendererComponent>().Color({ 1, 1, 1, 0.75f });
			}
			if (_counter >= disposeWaitTime) {
				entity.Destroy();
			}
			else _counter += time.deltaTime;
			return;
		}

		glm::vec3 curDir = glm::length(rigidBody.velocity) < 1 ? 
			rigidBody.velocity : glm::normalize(rigidBody.velocity);
		glm::vec3 dirNormalize = glm::normalize(direction);
		direction = glm::lerp(curDir, dirNormalize, turnSpeed * 0.1f);

		rigidBody.velocity = direction * (float)speed;
	}
};

#endif