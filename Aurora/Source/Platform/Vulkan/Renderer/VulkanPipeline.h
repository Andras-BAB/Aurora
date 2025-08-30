#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace Aurora {
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;

		/*
		glm::vec3 position;         // 3D-s pozíció
		glm::vec3 normal;           // Felület normálvektora (világításnál fontos)
		glm::vec2 texCoord;         // Textúra koordináta
		glm::vec3 color;            // Vertex szintű szín (maradhat!)

		// Haladó effektusokhoz
		glm::vec3 tangent;          // Normál térképhez
		glm::vec3 bitangent;

		// Animációhoz
		glm::ivec4 boneIndices;     // Maximum 4 csont
		glm::vec4 boneWeights;      // Súlyok csontokhoz
		*/

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}
	};
	
	class VulkanPipeline {
	public:
		VulkanPipeline() = default;
		virtual ~VulkanPipeline() = default;

		void Init(VkDevice device, VkRenderPass renderPass);
		void Destroy(VkDevice device);

		VkPipeline& GetGraphicsPipeline();
		VkPipelineLayout& GetPipelineLayout();

	private:
		VkShaderModule CreateShaderModule(const std::vector<char>& code, VkDevice device) const; 

		friend class VulkanContext;
	private:
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
	};
}
