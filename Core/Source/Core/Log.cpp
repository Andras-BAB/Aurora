#include <aupch.h>

#include <Core/Log.h>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Aurora {

	//std::shared_ptr<spdlog::logger> Log::s_Logger;
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;

	void Log::Init() {
		/*std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		//logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Aurora.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		//logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		s_Logger = std::make_shared<spdlog::logger>("AURORA", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_Logger);
		s_Logger->set_level(spdlog::level::trace);
		s_Logger->flush_on(spdlog::level::trace);*/

		//s_CoreLogger = std::make_shared<spdlog::logger>("Aurora");
		//s_CoreLogger->set_pattern("%^[%H:%M:%S:%e] %n: %v%$");
		
		// create a color multi-threaded logger
		//auto console = spdlog::stdout_color_mt("console");
		//auto err_logger = spdlog::stderr_color_mt("stderr");    
		//spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");

		s_CoreLogger = spdlog::stdout_color_mt("console");
		
	}

	void Log::LogCore() {

		spdlog::error("LogCore func");
		s_CoreLogger->info("Hello from Core log!");
	}

	void Log::SetLogger(std::shared_ptr<spdlog::logger>& logger) {

		s_CoreLogger = logger;

	}

}