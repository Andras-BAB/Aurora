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

		Unparent();

		// set the new parent
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

	void Entity::Unparent() {
		if (!HasComponent<RelationshipComponent>()) return;

		RelationshipComponent& myRelation = GetComponent<RelationshipComponent>();

		if (myRelation.Parent != entt::null) {
			RelationshipComponent& oldParentRelation = m_Scene->m_Registry.get<RelationshipComponent>(myRelation.Parent);

			if (oldParentRelation.FirstChild == m_EntityHandle) {
				oldParentRelation.FirstChild = myRelation.NextSibling;
			}

			if (myRelation.PrevSibling != entt::null) {
				RelationshipComponent& prevRelation = m_Scene->m_Registry.get<RelationshipComponent>(myRelation.PrevSibling);
				prevRelation.NextSibling = myRelation.NextSibling;
			}

			if (myRelation.NextSibling != entt::null) {
				RelationshipComponent& nextRelation = m_Scene->m_Registry.get<RelationshipComponent>(myRelation.NextSibling);
				nextRelation.PrevSibling = myRelation.PrevSibling;
			}

			myRelation.Parent = entt::null;
			myRelation.PrevSibling = entt::null;
			myRelation.NextSibling = entt::null;
		}
	}
}
