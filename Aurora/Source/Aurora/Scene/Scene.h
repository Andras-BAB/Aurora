#pragma once

#include "Aurora/Core/UUID.h"

#include "entt.hpp"
#include "Aurora/Core/Timestep.h"

namespace Aurora {

    class Entity;
    
    class Scene {
    public:
        Scene() = default;
        ~Scene() = default;

        // static std::shared_ptr<Scene> Copy(std::shared_ptr<Scene> other);
        
        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        void DestroyEntity(Entity entity);

        void OnRuntimeStart();
        void OnRuntimeStop();

        void OnSimulationStart();
        void OnSimulationStop();

        void OnUpdateRuntime(Timestep ts);
        // void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
        // void OnUpdateEditor(Timestep ts, EditorCamera& camera);
        void OnViewportResize(uint32_t width, uint32_t height);

        Entity DuplicateEntity(Entity entity);

        Entity FindEntityByName(std::string_view name);
        Entity GetEntityByUUID(UUID uuid);

        Entity GetPrimaryCameraEntity();

        bool IsRunning() const { return m_IsRunning; }
        bool IsPaused() const { return m_IsPaused; }

        void SetPaused(bool paused) { m_IsPaused = paused; }

        template<typename... Components>
        auto GetAllEntitiesWith() {
            return m_Registry.view<Components...>();
        }
        
    private:
        template<typename T>
        void OnComponentAdded(Entity entity, T& component);

        void RenderScene();
    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0;
        uint32_t m_ViewportHeight = 0;
        bool m_IsRunning = false;
        bool m_IsPaused = false;
        
        std::unordered_map<UUID, entt::entity> m_EntityMap;

        friend class Entity;
    };
}
