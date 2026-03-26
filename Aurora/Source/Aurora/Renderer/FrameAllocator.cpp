#include "aupch.h"

#include "FrameAllocator.h"

namespace Aurora {
	FrameAllocator::FrameAllocator(size_t capacity) : m_Capacity(capacity), m_Offset(0) {
		m_Buffer = static_cast<uint8_t*>(::operator new(capacity));
	}

	FrameAllocator::~FrameAllocator() {
		::operator delete(m_Buffer);
	}

	void* FrameAllocator::Allocate(size_t size, size_t alignment) {
		size_t padding = (alignment - (reinterpret_cast<uintptr_t>(m_Buffer + m_Offset) % alignment)) % alignment;

		if (m_Offset + padding + size > m_Capacity) {
			throw std::bad_alloc();
		}

		m_Offset += padding;
		void* result = m_Buffer + m_Offset;
		m_Offset += size;

		return result;
	}

	std::string_view FrameAllocator::AllocateString(std::string_view str) {
		if (str.empty()) return {};

		char* memory = static_cast<char*>(Allocate(str.size() + 1, alignof(char)));

		std::memcpy(memory, str.data(), str.size());
		memory[str.size()] = '\0';

		return std::string_view(memory, str.size());
	}

	void FrameAllocator::Reset() {
		m_Offset = 0;
	}
}
