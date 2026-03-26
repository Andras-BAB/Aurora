#pragma once

#include "RenderGraph.h"
#include "IRenderCommandList.h"

namespace Aurora {
	struct SceneData;

	class IRenderPass {
	public:
		virtual ~IRenderPass() = default;
		virtual void Setup(IRenderGraphBuilder& builder) = 0;

		virtual void Execute(IRenderCommandList* cmdList, const IRenderGraphResources& resources, const SceneData& sceneData) = 0;
	};
}
