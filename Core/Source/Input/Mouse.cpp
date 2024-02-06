#include <aupch.h>
#include <Input/Mouse.h>
#include <Core/Window.h>

namespace Aurora {

	Mouse::Event::Event() : m_Type(Type::Invalid), m_LeftIsPressed(false), m_RightIsPressed(false),
		m_MiddleIsPressed(false), x(0), y(0) {

	}

	Mouse::Event::Event(Type type, Mouse& parent) :
		m_Type(type), m_LeftIsPressed(parent.leftIsPressed), m_RightIsPressed(parent.rightIsPressed),
		m_MiddleIsPressed(false), x(parent.x), y(parent.y) {

	}

	bool Mouse::Event::IsValid() const {
		return m_Type != Type::Invalid;
	}

	Mouse::Event::Type Mouse::Event::GetType() const {
		return m_Type;
	}

	std::pair<int, int> Mouse::Event::GetPos() const {
		return { x, y };
	}

	int Mouse::Event::GetPosX() const {
		return x;
	}

	int Mouse::Event::GetPosY() const {
		return y;
	}

	bool Mouse::Event::LeftIsPressed() const {
		return m_LeftIsPressed;
	}

	bool Mouse::Event::RightIsPressed() const {
		return m_RightIsPressed;
	}

	Mouse::Mouse() : x(0), y(0) {
	}

	std::pair<int, int> Mouse::GetPos() const {
		return { x, y };
	}

	int Mouse::GetPosX() const {
		return x;
	}

	int Mouse::GetPosY() const {
		return y;
	}

	bool Mouse::LeftIsPressed() const {
		return leftIsPressed;
	}

	bool Mouse::RightIsPressed() const {
		return rightIsPressed;
	}

	bool Mouse::MiddleIsPressed() const {
		return middleIsPressed;
	}

	bool Mouse::IsInWindow() const {
		return isInWindow;
	}

	Mouse::Event Mouse::Read() {
		if (buffer.size() > 0u) {
			Mouse::Event e = buffer.front();
			buffer.pop();
			return e;
		}
		return Mouse::Event();
	}

	void Mouse::Flush() {
		buffer = std::queue<Event>();
	}

	std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() {
		if (rawDeltaBuffer.empty()) {
			return std::nullopt;
		}
		const RawDelta d = rawDeltaBuffer.front();
		rawDeltaBuffer.pop();
		return d;
	}

	void Mouse::EnableRaw() {
		rawEnabled = true;
	}

	void Mouse::DisableRaw() {
		rawEnabled = false;
	}

	bool Mouse::RawEnabled() const {
		return rawEnabled;
	}

	// Private functions

	void Mouse::OnMouseMove(int newX, int newY) {
		x = newX;
		y = newY;
		buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
		TrimBuffer();
	}

	void Mouse::OnMouseLeave() {
		isInWindow = false;
		buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
		TrimBuffer();
	}

	void Mouse::OnMouseEnter() {
		isInWindow = true;
		buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
		TrimBuffer();
	}

	void Mouse::OnLeftPressed(int x, int y) {
		leftIsPressed = true;
		buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
		TrimBuffer();
	}

	void Mouse::OnLeftReleased(int x, int y) {
		leftIsPressed = false;
		buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
		TrimBuffer();
	}

	void Mouse::OnRightPressed(int x, int y) {
		rightIsPressed = true;
		buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
		TrimBuffer();
	}

	void Mouse::OnRightReleased(int x, int y) {
		rightIsPressed = false;
		buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
		TrimBuffer();
	}

	void Mouse::OnMiddlePressed(int x, int y) {
		middleIsPressed = true;
		buffer.push(Mouse::Event(Mouse::Event::Type::MRelease, *this));
		TrimBuffer();
	}

	void Mouse::OnMiddleReleased(int x, int y) {
		middleIsPressed = false;
		buffer.push(Mouse::Event(Mouse::Event::Type::MRelease, *this));
		TrimBuffer();
	}

	void Mouse::OnWheelUp(int x, int y) {
		buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
		TrimBuffer();
	}

	void Mouse::OnWheelDown(int x, int y) {
		buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
		TrimBuffer();
	}

	void Mouse::TrimBuffer() {

		while (buffer.size() > bufferSize) {
			buffer.pop();
		}

	}

	void Mouse::OnWheelDelta(int xParam, int yParam, int delta) {
		wheelDeltaCarry += delta;

		while (wheelDeltaCarry >= WHEEL_DELTA) {
			wheelDeltaCarry -= WHEEL_DELTA;
			OnWheelUp(xParam, yParam);
		}
		while (wheelDeltaCarry <= -WHEEL_DELTA) {
			wheelDeltaCarry += WHEEL_DELTA;
			OnWheelDown(xParam, yParam);
		}
	}

	void Mouse::TrimRawInputBuffer() {
		while (rawDeltaBuffer.size() > bufferSize) {
			rawDeltaBuffer.pop();
		}
	}

	void Mouse::OnRawDelta(int dx, int dy) {
		rawDeltaBuffer.push({ dx,dy });
		TrimBuffer();
	}

}