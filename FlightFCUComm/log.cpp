#include "log.h"
#include<iostream>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h> // support for rotating file logging

namespace FCUPanel {

std::shared_ptr<spdlog::logger> Log::s_MainLogger;
//std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
//std::shared_ptr<spdlog::logger> Log::s_ConsoleLogger;

void Log::Init(spdlog::level::level_enum display_level) {
	//spdlog::set_pattern("%^[%T] %n: %v%$");

	//s_CoreLogger = spdlog::stdout_color_mt("MCEngine");
	//s_CoreLogger->set_level(spdlog::level::trace);
    try
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);
        std::cout << "MultiLogger: create console sink OK." << std::endl;


        //s_MainLogger = spdlog::rotating_logger_st("FCUPanel", "logs/fcu_panel_log.txt", static_cast<size_t>(1024 * 1024 * 5), 10);
        //s_MainLogger = spdlog::sinks::rotating_file_sink_st("logs/fcu_panel_log.txt", static_cast<size_t>(1024 * 1024 * 5), 10);

        //s_MainLogger->set_level(spdlog::level::debug);
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_st>("logs/fcu_panel_log.txt", static_cast<size_t>(1024 * 1024 * 5), 10);
        file_sink->set_level(spdlog::level::trace);
        std::cout << "MultiLogger: create file sink OK." << std::endl;

        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(console_sink);
        sinks.push_back(file_sink);
        s_MainLogger = std::make_shared<spdlog::logger>("FCUPanel", begin(sinks), end(sinks));
        s_MainLogger->set_level(display_level);
        std::cout << "MultiLogger: create multi sink OK." << std::endl;
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }

	//s_MainLogger = spdlog::stdout_color_mt("Game");
	//s_ClientLogger->set_level(spdlog::level::trace);
}

} //namespace FCUPanel