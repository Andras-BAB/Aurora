#pragma once

#include "Core/Layer.h"

namespace Sandbox {

	class SandboxLayer : public Aurora::Layer {
	public:

		SandboxLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Aurora::Event& e) override;
		void OnUpdate(Aurora::Timestep ts) override;

	};

}
