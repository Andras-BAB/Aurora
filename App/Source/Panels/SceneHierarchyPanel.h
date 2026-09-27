#pragma once

#include <Aurora.h>

namespace Aurora {
	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const std::shared_ptr<Scene>& scene);

		void SetContext(const std::shared_ptr<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const;
		void SetSelectedEntity(Entity entity);

	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		bool HasMatchingDescendant(Entity entity);

	private:
		std::shared_ptr<Scene> m_Scene;
		Entity m_SelectedEntity;
		math::Vec3 m_SelectedEntityEuler = { 0.0f, 0.0f, 0.0f };

		ImGuiTextFilter m_HierarchyFilter;
		Entity m_EntityToBeDeleted;
		Entity m_EntityToExpand; // parent row to force open next frame, e.g. after creating a child
	};
}
