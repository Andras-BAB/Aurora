#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"

#include "Aurora/Math/Math.h"

namespace Aurora {

	class Input {
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static math::Vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}