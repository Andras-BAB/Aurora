#pragma once

#include "RenderPass.h"
#include <unordered_map>
#include <memory>

namespace Aurora {

	class RenderPassLibrary {
	public:
		static RenderPassLibrary& Get();

		void Add(const std::shared_ptr<RenderPass>& renderPass);
		void Add(const std::string& name, const std::shared_ptr<RenderPass>& renderPass);

		std::shared_ptr<RenderPass> Load(const std::string& name, const RenderPassSpecification& spec);
		std::shared_ptr<RenderPass> Get(const std::string& name);

		bool Exists(const std::string& name) const;

		// Előre definiált render pass-ek
		std::shared_ptr<RenderPass> GetForwardPass();
		std::shared_ptr<RenderPass> GetShadowPass();
		std::shared_ptr<RenderPass> GetPostProcessPass();

	private:
		RenderPassLibrary() = default;
		std::unordered_map<std::string, std::shared_ptr<RenderPass>> m_RenderPasses;
	};

}
