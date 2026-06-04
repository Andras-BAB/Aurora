#pragma once

#include "Aurora/Core/UUID.h"
#include "Aurora/Renderer/Texture.h"
#include "Aurora/Renderer/Font.h"
#include "MeshInstance.h"

#include "Aurora/Math/Math.h"
#include "Aurora/Renderer/PerspectiveCamera.h"

#include "Aurora/Renderer/Buffer.h"
#include "Aurora/Renderer/ParticleSystem.h"

namespace Aurora {
	struct IDComponent {
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent {
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent {
		math::Vec3 Translation = { 0.0f, 0.0f, 0.0f };
		//math::Vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		math::Quat Rotation = math::Quat::Identity();
		math::Vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const math::Vec3& translation)
			: Translation(translation) {
		}

		math::Mat4 GetTransform() const {
			math::Mat4 tr = math::Mat4::Translation(Translation);
			//math::Mat4 rot = math::Mat4::RotateRollPitchYaw(Rotation);
			math::Mat4 rot = math::Mat4::Rotation(Rotation);
			math::Mat4 sc = math::Mat4::Scale(Scale);

			return sc * rot * tr;
		}
	};
	
	struct WorldTransformComponent {
		math::Mat4 Transform = math::Mat4::Identity();

		WorldTransformComponent() = default;
		WorldTransformComponent(const WorldTransformComponent&) = default;
		WorldTransformComponent(const math::Mat4& transform)
			: Transform(transform) {
		}
	};

	struct MeshComponent {
		std::shared_ptr<MeshInstance> Mesh = nullptr;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
	};
	
	struct RelationshipComponent {
		entt::entity Parent = entt::null;
		entt::entity FirstChild = entt::null;
		entt::entity NextSibling = entt::null;
		entt::entity PrevSibling = entt::null;

		bool HasParent() const { return Parent != entt::null; }
	};

	struct SkinnedMeshComponent {

	};

	struct ParticleEmitterComponent {
		uint32_t MaxParticles = 10000;
		bool IsActive = true;

		math::Vec3 Velocity = { 0.0f, 5.0f, 0.0f };
		float VelocityVariation = 2.0f;
		float VelocityRadial = 0.0f;

		float LifeTime = 2.0f;
		float LifeTimeVariation = 0.5f;

		math::Vec4 ColorBegin = { 0.0f, 0.4f, 0.8f, 1.0f };
		math::Vec4 ColorEnd = { 0.0f, 0.2f, 0.6f, 0.0f };

		float SizeBegin = 0.1f;
		float SizeEnd = 0.0f;

		math::Vec3 SpawnExtents = { 2.0f, 2.0f, 2.0f };
		BlendMode BlendMode = BlendMode::Additive;

		float EmissionRate = 1000.0f; // spawn rate/sec (0 all at once)
		bool Looping = true;
		
		float LeftoverSpawn = 0.0f;
		float TimeElapsed = 0.0f;
		bool IsInitialized = false;

		ParticleGradient Gradient;
		//TextureHandle GradientTexture;
		std::shared_ptr<ITexture2D> GradientTexture;
		std::shared_ptr<ITexture2D> Texture;
		bool IsGradientDirty = true;

		std::shared_ptr<StructuredBuffer> ParticleBuffer = nullptr;
		std::shared_ptr<StructuredBuffer> DeadListBuffer = nullptr;
		std::shared_ptr<StructuredBuffer> CounterBuffer = nullptr; // atomic counter 1 uint

		ParticleEmitterComponent() = default;
		ParticleEmitterComponent(const ParticleEmitterComponent&) = default;
	};

	struct PointLightComponent {
		math::Vec3 Color;
		float Radius;
		float Intensity;
	};

	struct DirectionalLightComponent {
		math::Vec3 Direction;
		math::Vec3 Strength;
	};

	struct SpotLightComponent {
		
	};

	struct SpriteRendererComponent {
		math::Vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		std::shared_ptr<ITexture2D> Texture;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const math::Vec4& color)
			: Color(color) {}
	};

	struct CameraComponent {
		//SceneCamera Camera;
		PerspectiveCamera Camera;
		bool Primary = true; // TODO: move to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct ScriptComponent {
		std::string ClassName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	// Forward declaration
	class ScriptableEntity;

	struct NativeScriptComponent {
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind() {
			InstantiateScript = []() {
				return static_cast<ScriptableEntity*>(new T());
			};
			DestroyScript = [](NativeScriptComponent* nsc) {
				delete nsc->Instance;
				nsc->Instance = nullptr;
			};
		}
	};

	// Physics

	struct Rigidbody2DComponent {
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};
	
	struct TextComponent {
		std::string TextString;
		std::shared_ptr<Font> FontAsset = Font::GetDefault();
		//math::Vec4 Color{ 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, WorldTransformComponent, SpriteRendererComponent,
			CameraComponent, ScriptComponent, NativeScriptComponent,
			Rigidbody2DComponent, TextComponent, RelationshipComponent, SkinnedMeshComponent,
			ParticleEmitterComponent>;
}
