#ifndef COMPONENT_H
#define COMPONENT_H

#include "Magia.h"

class ScriptableEntity;

struct TagComponent {
	std::string tag;
};

struct TransformComponent {
	TransformComponent* parent = NULL;
	glm::vec3 position = {0, 0, 0 };
	glm::vec3 rotation = { 0, 0, 0 };
	glm::vec3 scale = { 1, 1, 1 };
};

struct SpriteRendererComponent {
	Ref<VAO> handle;
	Ref<Texture> texture;
	glm::vec4 albedo = { 1, 1, 1, 1 };
	glm::vec2 size = { 1, 1 };
	bool parallelTexture = false;
	glm::vec2 textureOffset = { 0, 0 };
	float UVrepeat = 1;
	int order = 0;
};

struct CameraComponent {
	glm::vec2 resolution = { 16, 9 };
	bool primary = false;
};

struct CollisionComponent {
	bool collision = true;
	glm::vec2 origin = { 0, 0 };
	glm::vec2 size = { 1, 1 };
};

struct RigidbodyComponent {
	bool simulate = true;
	glm::vec3 position = { 0, 0, 0 };
	glm::vec3 velocity = { 0, 0, 0 };
};

struct SpriteSheetComponent {
	glm::vec2 size = { 1, 1 };
	int sizePerSprite = 1;
	int drawIndex = 0;
	Ref<Texture> sheet;
};

struct AnimatorComponent {

	struct AnimatorObject {
		Ref<Texture> animation;
		glm::vec2 size = { 1, 1 };
		int sizePerSprite = 1;
		int fps = 5;
		int drawIndex = 0;
		float _currentTime = 0;
	};

	std::map<int, AnimatorObject> animation_map;
	int current_id = INT_MAX;

	void AddAnimation(int id, AnimatorObject& object) {
		if (current_id == INT_MAX) current_id = id;
		animation_map[id] = object;
	}

	void AddAnimation(int id, Ref<Texture>& texture, glm::vec2 size, int fps, int sizePerSprite = 1) {
		AnimatorObject ani = { texture, size, sizePerSprite, fps, 0, 0 };
		AddAnimation(id, ani);
	}
};

struct NativeScriptComponent {
	ScriptableEntity* instance = NULL;

	ScriptableEntity*(*InstantiateScript)() = NULL;
	void(*DestroyScript)(NativeScriptComponent*) = NULL;

	template<typename T>
	void Bind() {
		InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
		DestroyScript = [](NativeScriptComponent* script) { delete script->instance; script->instance = NULL; };
	
		InstantiateScript();
	}

	template<typename T>
	auto Get() {
		return (static_cast<T*>(instance));
	}
};

#endif