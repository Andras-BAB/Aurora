#pragma once

namespace Core {
	namespace RefUtils {
		void AddToLiveReferences(void* instance);
		void RemoveFromLiveReference(void* instance);
		bool IsLive(void* instance);
	}

	class RefCounted {
	public:
		RefCounted() {
			RefUtils::AddToLiveReferences((void*)this);
		}
		virtual ~RefCounted() {
			RefUtils::RemoveFromLiveReference((void*)this);
		}

		void IncrementRefCount() const {
			++m_RefCount;
		}
		void DecrementRefCount() const {
			--m_RefCount;
		}

		uint32_t GetRefCount() const { return m_RefCount.load(); }
	private:
		mutable std::atomic<uint32_t> m_RefCount = 0;
	};

	template<typename T>
	class Ref {
	public:
		Ref() : m_Instance(nullptr) {}
		Ref(std::nullptr_t n) : m_Instance(nullptr) {}
		Ref(T* instance) : m_Instance(instance) {
			static_assert(std::is_base_of_v<RefCounted, T>, "Class is not derived from RefCounted!");
			IncRef();
		}

		template<typename T2>
			requires(std::is_base_of_v<T2, T> || std::is_base_of_v<T, T2>)
		Ref(const Ref<T2>& other) : m_Instance(static_cast<T*>(other.m_Instance)) {
			//m_Instance = static_cast<T*>(other.m_Instance);
			IncRef();
		}
		Ref(const Ref<T>& other) : m_Instance(other.m_Instance) {
			IncRef();
		}

		Ref(Ref&& other) noexcept : m_Instance(other.m_Instance) {
			other.m_Instance = nullptr;
		}

		template<typename T2>
			requires(std::is_base_of_v<T2, T> || std::is_base_of_v<T, T2>)
		Ref(Ref<T2>&& other) noexcept : m_Instance(static_cast<T*>(other.m_Instance)) {
			other.m_Instance = nullptr;
		}

		~Ref() {
			DecRef();
		}

		template<typename T2>
		Ref& operator=(const Ref<T2>& other) {
			other.IncRef();
			DecRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		Ref& operator=(Ref<T2>&& other) {
			DecRef();
			m_Instance = other.m_Instance;
			other.m_Instance = nullptr;
			return *this;
		}

		Ref& operator=(const Ref<T>& other) {
			if (this != &other) {
				//other.IncRef();
				if (other.m_Instance) {
					other.m_Instance->IncrementRefCount();
				}
				DecRef();
				m_Instance = other.m_Instance;
			}
			return *this;
		}

		Ref& operator=(Ref&& other) noexcept {
			if (this != &other) {
				DecRef();
				m_Instance = other.m_Instance;
				other.m_Instance = nullptr;
			}
			return *this;
		}

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		T* Raw() { return m_Instance; }
		const T* Raw() const { return m_Instance; }

		void Reset(T* instance = nullptr) {
			DecRef();
			m_Instance = instance;
			IncRef();
		}

		template<typename T2>
			requires(std::is_base_of_v<T2, T> || std::is_base_of_v<T, T2>)
		Ref<T2> As() const {
			return Ref<T2>(*this);
		}

		bool operator==(const Ref& other) const { return m_Instance == other.m_Instance; }
		bool operator!=(const Ref& other) const { return !(*this == other); }
		bool operator==(std::nullptr_t) const { return m_Instance == nullptr; }
		bool operator!=(std::nullptr_t) const { return m_Instance != nullptr; }

		explicit operator bool() const { return m_Instance != nullptr; }

		bool EqualsObject(const Ref<T>& other) {
			if (!m_Instance || !other.m_Instance) {
				return false;
			}
			return *m_Instance == *other.m_Instance;
		}

	private:
		void IncRef() {
			if (m_Instance) {
				m_Instance->IncrementRefCount();
			}
		}

		void DecRef() {
			if (m_Instance) {
				m_Instance->DecrementRefCount();

				if (m_Instance->GetRefCount() == 0) {
					delete m_Instance;
					m_Instance = nullptr;
				}
			}
		}
	private:
		template<class T2>
		friend class Ref;

		T* m_Instance = nullptr;
	};

	template<typename T, typename... Args>
	static Ref<T> MakeRef(Args&&... args) {
		return Ref<T>(new T(std::forward<Args>(args)...));
	}
}
