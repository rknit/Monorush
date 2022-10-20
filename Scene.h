#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H

#include "Collision.h"
#include "Component.h"
#include "Entity.h"
#include "Renderer.h"
#include "UI.h"
#include "ScriptableEntity.h"

class Scene {
	entt::registry scene_registry;

	friend class ScriptableEntity;
public:
	Scene();
	static Ref<Scene> Create();
	void Destroy();
	
	Entity CreateEntity(std::string name = "");

	void OnUpdate(Time time);
};

#endif