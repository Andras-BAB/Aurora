#pragma once

#include <utility>
#include <queue>
#include <optional>

namespace Aurora {

	class Mouse {
		friend class Window;

	public:

		struct RawDelta {
			int x, y;
		};

		class Event {
		public:
			enum class Type {
				LPress,
				LRelease,
				RPress,
				RRelease,
				MPress,
				MRelease,
				WheelUp,
				WheelDown,
				Move,
				Enter,
				Leave,
				Invalid
			};

		public:
			Event();
			Event(Type type, Mouse& parent);
			bool IsValid() const;
			Type GetType() const;
			std::pair<int, int> GetPos() const;
			int GetPosX() const;
			int GetPosY() const;
			bool LeftIsPressed() const;
			bool RightIsPressed() const;

		private:
			Type m_Type;
			bool m_LeftIsPressed;
			bool m_RightIsPressed;
			bool m_MiddleIsPressed;
			int x;
			int y;
		};

	public:
		Mouse();
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;
		std::pair<int, int> GetPos() const;
		std::optional<RawDelta> ReadRawDelta();
		int GetPosX() const;
		int GetPosY() const;
		bool LeftIsPressed() const;
		bool RightIsPressed() const;
		bool MiddleIsPressed() const;
		bool IsInWindow() const;
		Mouse::Event Read();
		inline bool IsEmpty() const { return buffer.empty(); }
		void Flush();

		void EnableRaw();
		void DisableRaw();
		bool RawEnabled() const;

	private:
		void OnMouseMove(int newX, int newY);
		void OnMouseLeave();
		void OnMouseEnter();
		void OnLeftPressed(int x, int y);
		void OnLeftReleased(int x, int y);
		void OnRightPressed(int x, int y);
		void OnRightReleased(int x, int y);
		void OnMiddlePressed(int x, int y);
		void OnMiddleReleased(int x, int y);
		void OnWheelUp(int x, int y);
		void OnWheelDown(int x, int y);
		void TrimBuffer();
		void OnWheelDelta(int xParam, int y, int delta);

		void OnRawDelta(int dx, int dy);
		void TrimRawInputBuffer();

	private:
		static constexpr unsigned int bufferSize = 16u;
		int x;
		int y;
		bool leftIsPressed = false;
		bool rightIsPressed = false;
		bool middleIsPressed = false;
		bool isInWindow = false;
		int wheelDeltaCarry = 0;
		std::queue<Event> buffer;
		std::queue<RawDelta> rawDeltaBuffer;
		bool rawEnabled = false;

	};

}