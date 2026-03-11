#include "aupch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "ScriptableEntity.h"
//#include "Aurora/Scripting/ScriptEngine.h"
#include "Aurora/Renderer/Renderer3D.h"
#include "Entity.h"

#include <glm/glm.hpp>

#include "Aurora/Scripting/ScriptEngine.h"

namespace Aurora {

	Entity Scene::CreateEntity(const std::string& name) {
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>();
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		
		m_EntityMap[uuid] = entity;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity) {
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart() {
	}

	void Scene::OnRuntimeStop() {
	}

	void Scene::OnSimulationStart() {
	}

	void Scene::OnSimulationStop() {
	}

	void Scene::OnUpdateRuntime(Timestep ts) {
		if (!m_IsPaused) {
			{
				auto view = m_Registry.view<ScriptComponent>();
				for (auto entity : view) {
					Entity e = { entity, this };
					ScriptEngine::OnUpdateEntity(e, ts);
				}
			}
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height) {
	}

	Entity Scene::DuplicateEntity(Entity entity) {
		return {};
	}

	Entity Scene::FindEntityByName(std::string_view name) {
		for (const auto view = m_Registry.view<TagComponent>(); const auto entity : view) {
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name) {
				return Entity{ entity, this };
			}
		}
		return {};
	}

	Entity Scene::GetEntityByUUID(UUID uuid) {
		if (m_EntityMap.contains(uuid)) {
			return { m_EntityMap.at(uuid), this };
		}
		return {};
	}

	Entity Scene::GetPrimaryCameraEntity() {
		return {};
	}

	void Scene::RenderScene() {
		// auto view = m_Registry.view<MeshComponent>();
		// for (auto entity : view) {
		//     
		// }
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) {
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0) {
			//component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		}
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component) {
	}
	
	template<>
	void Scene::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component) {
	}
	
}
