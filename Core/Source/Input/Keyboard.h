#pragma once

#include <queue>
#include <bitset>
#include <optional>

namespace Aurora {

	class Keyboard {

		friend class Window;

	public:

		class Event {
		public:
			enum class Type {
				Press,
				Release,
				Invalid
			};

		public:
			Event();
			Event(Type type, unsigned char code);

			inline bool IsPress() const { return m_Type == Type::Press; }
			inline bool IsRelease() const { return m_Type == Type::Release; }
			inline bool IsValid() const { return m_Type != Type::Invalid; }
			inline unsigned char GetCode() const { return m_Code; }

		private:
			Type m_Type;
			unsigned char m_Code;
		};

		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;

		// Key event stuff
		bool IsKeyPressed(unsigned char keyCode) const;
		std::optional<Event> ReadKey();
		bool IsKeyIsEmpty() const;
		void FlushKey();

		// Char event stuff
		char ReadChar();
		bool CharIsEmpty() const;
		void FlushChar();
		void Flush();

		void EnableAutoRepeat();
		void DisableAutoRepeat();
		bool AutoRepeatIsEnabled() const;

	private:

		void OnKeyPressed(unsigned char keyCode);
		void OnKeyReleased(unsigned char keyCode);
		void OnChar(char character);
		void ClearState();

		template<typename T>
		static void TrimBuffer(std::queue<T>& buffer) {
			while (buffer.size() > bufferSize) {
				buffer.pop();
			}
		}

	private:

		static constexpr unsigned int nKeys = 256u;
		static constexpr unsigned int bufferSize = 16u;
		bool autorepeatEnabled = false;
		std::bitset<nKeys> keyStates;
		std::queue<Event> keyBuffer;
		std::queue<char> charBuffer;

	};


}