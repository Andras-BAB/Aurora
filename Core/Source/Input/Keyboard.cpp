#include <aupch.h>
#include <Input/Keyboard.h>

namespace Aurora {

	bool Keyboard::IsKeyPressed(unsigned char keyCode) const {
		return keyStates[keyCode];
	}

	std::optional<Keyboard::Event> Keyboard::ReadKey() {
		if (keyBuffer.size() > 0u) {
			Keyboard::Event e = keyBuffer.front();
			keyBuffer.pop();
			return e;
		}
		return {};
	}

	bool Keyboard::IsKeyIsEmpty() const {
		return keyBuffer.empty();
	}

	char Keyboard::ReadChar() {
		if (charBuffer.size() > 0u) {
			unsigned char charCode = charBuffer.front();
			charBuffer.pop();
			return charCode;
		}
		return 0;
	}

	bool Keyboard::CharIsEmpty() const {
		return charBuffer.empty();
	}

	void Keyboard::FlushChar() {
		charBuffer = std::queue<char>();
	}

	void Keyboard::FlushKey() {
		keyBuffer = std::queue<Event>();
	}

	void Keyboard::Flush() {
		FlushKey();
		FlushChar();
	}

	void Keyboard::EnableAutoRepeat() {
		autorepeatEnabled = true;
	}

	void Keyboard::DisableAutoRepeat() {
		autorepeatEnabled = false;
	}

	bool Keyboard::AutoRepeatIsEnabled() const {
		return autorepeatEnabled;
	}

	void Keyboard::OnKeyPressed(unsigned char keyCode) {

		keyStates[keyCode] = true;
		keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keyCode));
		TrimBuffer(keyBuffer);
	}

	void Keyboard::OnKeyReleased(unsigned char keyCode) {
		keyStates[keyCode] = false;
		keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keyCode));
		TrimBuffer(keyBuffer);
	}

	void Keyboard::OnChar(char character) {
		charBuffer.push(character);
		TrimBuffer(charBuffer);
	}

	void Keyboard::ClearState() {
		keyStates.reset();
	}

	Keyboard::Event::Event() : m_Type(Type::Invalid), m_Code(0u) {

	}

	Keyboard::Event::Event(Type type, unsigned char code) : m_Type(type), m_Code(code) {

	}

}