#pragma once

#include "main.h"

namespace FCUPanel {
class Log {
public:
	static void Init();

	//inline static std::shared_ptr<spdlog::logger> &GetCoreLogger() {
	//	return s_CoreLogger;
	//};
	inline static std::shared_ptr<spdlog::logger> &GetLogger() {
		return s_MainLogger;
	};

private:
	//static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_MainLogger;
	//static std::shared_ptr<spdlog::logger> s_ConsoleLogger;

};
} // namespace MCEngine

// Log Macros
//#define MCE_CORE_TRACE(...) ::MCEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
//#define MCE_CORE_INFO(...) ::MCEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
//#define MCE_CORE_WARN(...) ::MCEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
//#define MCE_CORE_ERROR(...) ::MCEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
//#define MCE_CORE_FATAL(...) ::MCEngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define FCU_TRACE(...) ::FCUPanel::Log::GetLogger()->trace(__VA_ARGS__)
#define FCU_DEBUG(...) ::FCUPanel::Log::GetLogger()->debug(__VA_ARGS__)
#define FCU_INFO(...) ::FCUPanel::Log::GetLogger()->info(__VA_ARGS__)
#define FCU_WARN(...) ::FCUPanel::Log::GetLogger()->warn(__VA_ARGS__)
#define FCU_ERROR(...) ::FCUPanel::Log::GetLogger()->error(__VA_ARGS__)
#define FCU_CRITICAL(...) ::FCUPanel::Log::GetLogger()->critical(__VA_ARGS__)