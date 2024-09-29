#include "aupch.h"
#include "Scene.h"

#include "Entity.h"

#include "Components.h"
#include "ScriptableEntity.h"
//#include "Aurora/Scripting/ScriptEngine.h"
#include "Aurora/Renderer/Renderer3D.h"

#include <glm/glm.hpp>

#include "Entity.h"

namespace Aurora {


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