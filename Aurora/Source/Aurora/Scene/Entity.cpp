#include "aupch.h"
#include "Entity.h"

namespace Aurora {
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

	void Entity::SetParent(Entity parent) {

        if (!HasComponent<RelationshipComponent>()) AddComponent<RelationshipComponent>();
        if (!parent.HasComponent<RelationshipComponent>()) parent.AddComponent<RelationshipComponent>();

        RelationshipComponent& myRelation = GetComponent<RelationshipComponent>();
        RelationshipComponent& parentRelation = parent.GetComponent<RelationshipComponent>();

        // TODO: unparent from previous parent if exists

        myRelation.Parent = parent;

        if (parentRelation.FirstChild == entt::null) {
            parentRelation.FirstChild = m_EntityHandle;
        } else {
            entt::entity current = parentRelation.FirstChild;
            RelationshipComponent* rel = &m_Scene->m_Registry.get<RelationshipComponent>(current);
            while (rel->NextSibling != entt::null) {
                current = rel->NextSibling;
                rel = &m_Scene->m_Registry.get<RelationshipComponent>(current);
            }
            rel->NextSibling = m_EntityHandle;
            myRelation.PrevSibling = current;
        }
	}
}
