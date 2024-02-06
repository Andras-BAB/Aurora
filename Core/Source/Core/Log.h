#pragma once

// Get rid of spdlog warnings
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Aurora {

	class Log {
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

	private:

		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_Logger;

	};

}

#ifndef DIST

	#define AuCoreLogTrace(...)		Aurora::Log::GetCoreLogger()->trace(__VA_ARGS__)
	#define AuCoreLogInfo(...)		Aurora::Log::GetCoreLogger()->info(__VA_ARGS__)
	#define AuCoreLogWarn(...)		Aurora::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define AuCoreLogError(...)		Aurora::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define AuCoreLogCritical(...)	Aurora::Log::GetCoreLogger()->critical(__VA_ARGS__)

	#define AuLogTrace(...)			Aurora::Log::GetLogger()->trace(__VA_ARGS__)
	#define AuLogInfo(...)			Aurora::Log::GetLogger()->info(__VA_ARGS__)
	#define AuLogWarn(...)			Aurora::Log::GetLogger()->warn(__VA_ARGS__)
	#define AuLogError(...)			Aurora::Log::GetLogger()->error(__VA_ARGS__)
	#define AuLogCritical(...)		Aurora::Log::GetLogger()->critical(__VA_ARGS__)

#else

	#define AuCoreLogTrace(...)
	#define AuCoreLogInfo(...)
	#define AuCoreLogWarn(...)
	#define AuCoreLogError(...)
	#define AuCoreLogCritical(...)

	#define AuLogTrace(...)
	#define AuLogInfo(...)
	#define AuLogWarn(...)
	#define AuLogError(...)
	#define AuLogCritical(...)

#endif // LOGGER DEFINES