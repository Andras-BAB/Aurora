#pragma once

#include <Core/Layer.h>

namespace Aurora {

	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer() = default;

		void OnCreate() override;
		void OnDestroy() override;
		void OnEvent(Event& e) override;

		void Begin();
		void End();

	};

}