#pragma once

namespace Aurora {
	class IGraphicsContext {
	public:
		virtual ~IGraphicsContext() = default;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void SwapBuffers() = 0;

		virtual void BeginFrame() = 0;
		virtual void SubmitFrame() = 0;

		virtual void OnWindowResize() = 0;

		static std::unique_ptr<IGraphicsContext> Create(void* window);
	};
}