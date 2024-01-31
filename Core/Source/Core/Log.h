#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Aurora {

	class Log {
	public:
		static void Init();

		//static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

		static void LogCore();
		static void SetLogger(std::shared_ptr<spdlog::logger>& logger);

	private:

		//static std::shared_ptr<spdlog::logger> s_Logger;
		static std::shared_ptr<spdlog::logger> s_CoreLogger;

	};

}
/*
#define AuTrace(...)    ::Aurora::Log::GetLogger()->trace(__VA_ARGS__)
#define AuInfo(...)     ::Aurora::Log::GetLogger()->info(__VA_ARGS__)
#define AuWarn(...)     ::Aurora::Log::GetLogger()->warn(__VA_ARGS__)
#define AuError(...)    ::Aurora::Log::GetLogger()->error(__VA_ARGS__)
#define AuCritical(...) ::Aurora::Log::GetLogger()->critical(__VA_ARGS__)
*/
