#include <aupch.h>

#include <Core/Log.h>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Aurora {

	std::shared_ptr<spdlog::logger> Log::s_Logger;
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;

	void Log::Init() {

		AllocConsole();
		auto out = freopen("CONOUT$", "w", stdout);
		auto in = freopen("CONIN$", "r", stdin);
		auto err = freopen("CONOUT$", "w", stderr);

		s_CoreLogger = spdlog::stdout_color_mt("Core");
		s_Logger = spdlog::stdout_color_mt("Aurora");

		/*
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Aurora.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		s_Logger = std::make_shared<spdlog::logger>("AURORA", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_Logger);
		s_Logger->set_level(spdlog::level::trace);
		s_Logger->flush_on(spdlog::level::trace);
		*/

		s_CoreLogger->set_pattern("%^[%H:%M:%S.%e] %n: %v%$");
		s_Logger->set_pattern("%^[%H:%M:%S:%e] %n: %v%$");

		AuCoreLogInfo("Console log successfully initialized!");
		
	}
}