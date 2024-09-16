#pragma once

#include "Aurora.h"

namespace Sandbox {

	class SandboxLayer : public Aurora::Layer {
	public:

		SandboxLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Aurora::Event& e) override;
		void OnUpdate(Aurora::Timestep ts) override;
		void OnImGuiRender() override;

	private:
		bool m_IsImGuiDemoVisible = false;
	};

}
