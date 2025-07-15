#pragma once
#include <complex.h>

namespace Aurora {
	
	class IGraphicsInstance {
	public:
		virtual ~IGraphicsInstance() = default;

		virtual void Init() = 0;
		virtual void Destroy() = 0;
		
		virtual void GetDevice() = 0;

		static std::unique_ptr<IGraphicsInstance> Create();
	};

}
