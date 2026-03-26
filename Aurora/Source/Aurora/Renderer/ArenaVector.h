#pragma once

#include "FrameAllocator.h"
#include "Aurora/Core/Assert.h"

namespace Aurora {
	template <typename T>
	class ArenaVector {
	public:
		ArenaVector(FrameAllocator& allocator, size_t initialCapacity = 4) : m_Allocator(allocator), m_Size(0), m_Capacity(initialCapacity) {
			m_Data = static_cast<T*>(m_Allocator.Allocate(m_Capacity * sizeof(T), alignof(T)));
		}

		void push_back(const T& value) {
			CheckCapacity();
			new (&m_Data[m_Size++]) T(value);
		}

		void push_back(T&& value) {
			CheckCapacity();
			new (&m_Data[m_Size++]) T(std::move(value));
		}

		T& operator[](size_t index) {
			AU_CORE_ASSERT(index < m_Size, "Index out of range!");
			return m_Data[index];
		}

		const T& operator[](size_t index) const {
			AU_CORE_ASSERT(index < m_Size, "Index out of range!");
			return m_Data[index];
		}

		T* begin() { return m_Data; }
		T* end() { return m_Data + m_Size; }
		const T* begin() const { return m_Data; }
		const T* end() const { return m_Data + m_Size; }

		size_t size() const { return m_Size; }
		bool empty() const { return m_Size == 0; }

	private:
		void CheckCapacity() {
			if (m_Size >= m_Capacity) {
				size_t newCapacity = m_Capacity * 2;
				T* newData = static_cast<T*>(m_Allocator.Allocate(newCapacity * sizeof(T), alignof(T)));

				for (size_t i = 0; i < m_Size; ++i) {
					new (&newData[i]) T(std::move(m_Data[i]));

					m_Data[i].~T();
				}

				m_Data = newData;
				m_Capacity = newCapacity;
			}
		}

		FrameAllocator& m_Allocator;
		T* m_Data;
		size_t m_Size;
		size_t m_Capacity;
	};
}
