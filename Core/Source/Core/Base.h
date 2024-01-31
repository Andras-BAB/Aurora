#pragma once

#ifdef AURORA_API_EXPORT
	#define AURORA_API __declspec(dllexport)
#else
	#define AURORA_API __declspec(dllimport)
#endif // AURORA_API


#include <memory>

#define BIT(x) (1 << x)

#define BindEvent(x) std::bind(&x, this, std::placeholders::_1)

namespace Aurora {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
