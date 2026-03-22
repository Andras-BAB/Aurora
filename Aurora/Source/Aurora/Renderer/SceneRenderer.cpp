#include "aupch.h"

#include "SceneRenderer.h"

#include "RenderCommand.h"
#include "Renderer3D.h"
#include "RendererAPI.h"
#include "Aurora/Scene/Entity.h"

namespace Aurora {
	void SceneRenderer::Render(Scene* scene) {
		Entity cameraEntity = scene->GetPrimaryCameraEntity();
		if (!cameraEntity) return;
		PerspectiveCamera& camera = cameraEntity.GetComponent<CameraComponent>().Camera;

		SceneData data;
		data.ViewMatrix.m = camera.GetView();
		data.ProjectionMatrix.m = camera.GetProj();
		data.EyePosition = camera.GetPosition3f();

		RenderCommand::BeginScene(data);

		RenderCommand::Clear();
		RenderCommand::DrawIndexed(nullptr);

		auto view = scene->GetAllEntitiesWith<MeshComponent, WorldTransformComponent>();

		for (auto entity : view) {
			auto [mesh, transform] = view.get<MeshComponent, WorldTransformComponent>(entity);

			if (!mesh.Mesh) continue;

			bool isHierarchical = scene->GetRegistry().all_of<RelationshipComponent>(entity);
			const std::vector<SubmeshInstance>& instances = mesh.Mesh->GetMesh()->GetSubmeshInstances();

			for (const auto& instance : instances) {
				RenderProxyData proxyData;
				proxyData.ObjectID = static_cast<uint32_t>(entity);
				proxyData.Mesh = mesh.Mesh;
				proxyData.SubmeshIndex = instance.SubmeshIndex;

				math::Mat4 entityWorld = transform.Transform;

				if (isHierarchical) {
					proxyData.Transform = entityWorld;
				} else {
					math::Mat4 submeshLocal = instance.LocalTransform;
					proxyData.Transform = submeshLocal * entityWorld;
				}

				Renderer3D::SubmitProxy(proxyData);
			}
		}

		RenderCommand::EndScene();
	}
}
