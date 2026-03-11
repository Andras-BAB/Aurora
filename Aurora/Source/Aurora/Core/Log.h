#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#pragma warning(pop)

namespace Aurora {
	class Log {
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
//inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector) {
//	return os << glm::to_string(vector);
//}
//
//template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
//inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix) {
//	return os << glm::to_string(matrix);
//}
//
//template<typename OStream, typename T, glm::qualifier Q>
//inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion) {
//	return os << glm::to_string(quaternion);
//}

// Core log macros
#define AU_CORE_TRACE(...)		Aurora::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define AU_CORE_INFO(...)		Aurora::Log::GetCoreLogger()->info(__VA_ARGS__)
#define AU_CORE_WARN(...)		Aurora::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define AU_CORE_ERROR(...)		Aurora::Log::GetCoreLogger()->error(__VA_ARGS__)
#define AU_CORE_CRITICAL(...)	Aurora::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define AU_TRACE(...)			Aurora::Log::GetLogger()->trace(__VA_ARGS__)
#define AU_INFO(...)			Aurora::Log::GetLogger()->info(__VA_ARGS__)
#define AU_WARN(...)			Aurora::Log::GetLogger()->warn(__VA_ARGS__)
#define AU_ERROR(...)			Aurora::Log::GetLogger()->error(__VA_ARGS__)
#define AU_CRITICAL(...)		Aurora::Log::GetLogger()->critical(__VA_ARGS__)