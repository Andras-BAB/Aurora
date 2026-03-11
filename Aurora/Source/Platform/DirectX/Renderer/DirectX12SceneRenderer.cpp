#include "aupch.h"

#include "DirectX12SceneRenderer.h"

#include "Aurora/Renderer/RenderCommand.h"
#include "Aurora/Renderer/Renderer3D.h"
#include "Aurora/Renderer/RenderProxyData.h"
#include "Aurora/Scene/Components.h"
#include "Aurora/Scene/Entity.h"
#include "Aurora/Scene/Scene.h"

namespace Aurora {
	void DirectX12SceneRenderer::Render(Scene* scene) {
		Entity cameraEntity = scene->GetPrimaryCameraEntity();
		if (!cameraEntity) return;
		Camera& camera = cameraEntity.GetComponent<CameraComponent>().Camera;

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

			//RenderProxyData proxyData;
			//proxyData.ObjectID = static_cast<uint32_t>(entity);
			//proxyData.Mesh = mesh.Mesh;
			//proxyData.Transform = transform.Transform;

			//Renderer3D::SubmitProxy(proxyData);

			//auto tempTransform = transform.GetTransform();
			//DirectX::XMMATRIX entityWorld = DirectX::XMLoadFloat4x4(&tempTransform);
			//DirectX::XMMATRIX entityWorld = transform.GetTransform();
			//math::Mat4 entityWorld = transform.GetTransform();

			//Renderer3D::SubmitEntity(Entity({ entity, scene }));
			
			//for (const auto& submesh : mesh.Mesh->GetMesh()->GetSubmeshInstances()) {
				// Itt kombináljuk a kettőt:
				//math::Mat4 submeshLocal = submesh.LocalTransform;
				//math::Mat4 finalWorld = submeshLocal * entityWorld;

				// Beküldjük a backendnek: 1 submesh = 1 Draw Call (vagy Proxy)
				//Renderer3D::SubmitMesh(mesh.Mesh.get(), submesh.Index, finalWorld);
				//Renderer3D::Draw(Entity{ entity, scene });
				//Renderer3D::SubmitEntity(Entity({ entity, scene }));
			//}
		}

		RenderCommand::EndScene();
	}
}
