#pragma once

#include <cstdint>
#include <cstdlib>
#include <utility>
#include <stdexcept>

namespace Aurora {
	class FrameAllocator {
	public:
		FrameAllocator(size_t capacity);

		virtual ~FrameAllocator();

		FrameAllocator(const FrameAllocator&) = delete;
		FrameAllocator& operator=(const FrameAllocator&) = delete;

		void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t));

		template<typename T, typename... Args>
		T* AllocateObject(Args&&... args) {
			void* mem = Allocate(sizeof(T), alignof(T));
			return new (mem) T(std::forward<Args>(args)...);
		}

		std::string_view AllocateString(std::string_view str);

		void Reset();

	private:
		uint8_t* m_Buffer;
		size_t m_Capacity;
		size_t m_Offset;
	};
}
