#pragma once
#include "Aurora/Renderer/Buffer.h"

#include <cstdint>
#include <vulkan/vulkan.h>

namespace Aurora {
	class VertexLayoutDescriptor {
	public:
		VertexLayoutDescriptor(uint32_t binding, const BufferLayout& layout)
			: m_Binding(binding), m_Layout(layout) {
			GenerateDescriptions();
		}

		const std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions() const {
			return m_AttributeDescriptions;
		}

		VkVertexInputBindingDescription GetBindingDescription() const {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = m_Binding;
			bindingDescription.stride = m_Layout.GetStride();
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // vagy instancinghez VK_VERTEX_INPUT_RATE_INSTANCE
			return bindingDescription;
		}

	private:
		void GenerateDescriptions() {
			m_AttributeDescriptions.clear();
			uint32_t location = 0; // kezdő shader location

			for (const auto& element: m_Layout) {
				VkVertexInputAttributeDescription desc{};
				desc.binding = m_Binding;
				desc.location = location++;
				desc.format = ShaderDataTypeToVkFormat(element.Type);
				desc.offset = static_cast<uint32_t>(element.Offset);
				m_AttributeDescriptions.push_back(desc);
			}
		}

		static VkFormat ShaderDataTypeToVkFormat(ShaderDataType type) {
			switch (type) {
			case ShaderDataType::Float:		return VK_FORMAT_R32_SFLOAT;
			case ShaderDataType::Float2:	return VK_FORMAT_R32G32_SFLOAT;
			case ShaderDataType::Float3:	return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::Float4:	return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::Int:		return VK_FORMAT_R32_SINT;
			case ShaderDataType::Int2:		return VK_FORMAT_R32G32_SINT;
			case ShaderDataType::Int3:		return VK_FORMAT_R32G32B32_SINT;
			case ShaderDataType::Int4:		return VK_FORMAT_R32G32B32A32_SINT;
			default:
				AU_CORE_ERROR("Unsupported ShaderDataType!");
				return VK_FORMAT_UNDEFINED;
			}
		}

	private:
		uint32_t m_Binding;
		BufferLayout m_Layout;
		std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
	};
}
