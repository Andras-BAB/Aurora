#include "aupch.h"
#include "Entity.h"

namespace Aurora {
    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
    }
}