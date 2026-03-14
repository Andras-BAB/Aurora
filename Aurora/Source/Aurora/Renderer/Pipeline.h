#pragma once

#include "RenderPass.h"
#include "Shader.h"
#include "Buffer.h"
#include <memory>

namespace Aurora {
	enum class PrimitiveTopology {
		Triangles,
		Lines,
		Points
	};

	enum class CullMode {
		None,
		Front,
		Back,
		FrontAndBack
	};

	enum class FrontFace {
		Clockwise,
		CounterClockwise
	};

	enum class ShaderStage {
		Vertex,
		Fragment,
		Geometry,
		TessellationControl,
		TessellationEvaluation,
		Compute
	};
	
	struct PipelineSpecification {
		std::shared_ptr<Shader> shader;
		
		std::shared_ptr<RenderPass> renderPass;
		BufferLayout vertexLayout;

		PrimitiveTopology topology = PrimitiveTopology::Triangles;
		CullMode cullMode = CullMode::Back;
		FrontFace frontFace = FrontFace::CounterClockwise;
		bool depthTest = true;
		bool depthWrite = true;
		bool wireframe = false;
		float lineWidth = 1.0f;

		// Blending
		bool blendEnable = false;
	};

	struct ComputePipelineSpecification {
		std::shared_ptr<Shader> computeShader;
	};

	class Pipeline {
	public:
		virtual ~Pipeline() = default;

		virtual void Bind() = 0;

		virtual const PipelineSpecification& GetSpecification() const = 0;
		virtual std::shared_ptr<Shader> GetShader(ShaderStage stage) const = 0;

		static std::shared_ptr<Pipeline> Create(const PipelineSpecification& spec);
	};

}
