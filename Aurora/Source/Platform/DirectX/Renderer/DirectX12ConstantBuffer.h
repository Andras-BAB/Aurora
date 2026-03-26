#pragma once
#include "Aurora/Renderer/IUniformBuffer.h"

namespace Aurora {
	
	class DirectX12ConstantBuffer : public IUniformBuffer {
	public:
		DirectX12ConstantBuffer(uint32_t size, uint32_t binding);
		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	};

}
