#pragma once

#include "Aurora/Math/Math.h"

namespace Aurora {
	struct GPUParticle {
		math::Vec3 Position;
		float LifeRemaining;

		math::Vec3 Velocity;
		float LifeTime;

		math::Vec4 ColorBegin;
		math::Vec4 ColorEnd;

		float SizeBegin;
		float SizeEnd;
		math::Vec2 Padding;
	};

	struct ParticleSystemParams {
		math::Vec3 EmitterPosition;
		float DeltaTime;

		math::Vec3 EmitterVelocity;
		float LifeTime;

		math::Vec4 ColorBegin;
		math::Vec4 ColorEnd;
		//TextureHandle GradientTextureIndex;

		float SizeBegin;
		float SizeEnd;
		float VelocityVariation;
		uint32_t RandomSeed;

		math::Vec3 SpawnExtents;
		float VelocityRadial;

		uint32_t MaxParticles;

		uint32_t ParticleBufferIdx;
		uint32_t DeadListBufferIdx;
		uint32_t CounterBufferIdx;

		uint32_t EmitCount;
		math::Vec3 Padding;
	};

	struct ColorKey {
		float Time;
		math::Vec4 Color;
	};

	class ParticleGradient {
	public:
		std::vector<ColorKey> Keys;

		ParticleGradient() {
			Keys.push_back({ 0.0f, math::Vec4(1.0f, 1.0f, 1.0f, 1.0f) });
			Keys.push_back({ 1.0f, math::Vec4(1.0f, 1.0f, 1.0f, 0.0f) });
		}

		math::Vec4 Evaluate(float time) const {
			if (Keys.empty()) return { 1, 1, 1, 1 };
			if (Keys.size() == 1) return Keys[0].Color;

			if (time <= Keys.front().Time) return Keys.front().Color;
			if (time >= Keys.back().Time) return Keys.back().Color;

			// interpolation between 2 keys
			for (size_t i = 0; i < Keys.size() - 1; ++i) {
				if (time >= Keys[i].Time && time <= Keys[i + 1].Time) {
					float t = (time - Keys[i].Time) / (Keys[i + 1].Time - Keys[i].Time);
					return MathHelper::Lerp(Keys[i].Color, Keys[i + 1].Color, t);
				}
			}
			return { 1, 1, 1, 1 };
		}
	};
}
