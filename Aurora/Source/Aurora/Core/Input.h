#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"

#include "Aurora/Math/Math.h"

namespace Aurora {

	class Input {
	public:
		static bool IsKeyPressed(KeyCode key, void* targetWindow = nullptr);

		static bool IsMouseButtonPressed(MouseCode button, void* targetWindow = nullptr);
		static math::Vec2 GetMousePosition(void* targetWindow = nullptr);
		static float GetMouseX();
		static float GetMouseY();

	private:
		std::array<std::atomic<bool>, 1024> s_KeyStates;
		std::atomic<float> s_MouseX;
		std::atomic<float> s_MouseY;
	};
}