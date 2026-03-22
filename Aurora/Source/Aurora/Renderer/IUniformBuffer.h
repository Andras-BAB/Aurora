#pragma once

namespace Aurora {
		
	class IUniformBuffer {
	public:
		virtual ~IUniformBuffer() = default;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		
		static std::shared_ptr<IUniformBuffer> Create(uint32_t size, uint32_t binding);
	};
	
}
