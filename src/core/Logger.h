#pragma once

#include <iostream>
#include <string>
#include <sstream>

namespace Archura {

/**
 * @brief Logger - Debug logging sistemi
 * 
 * Release modunda otomatik olarak deaktif olur
 */
class Logger {
public:
    enum class Level {
        Debug,
        Info,
        Warning,
        Error
    };

    static void SetEnabled(bool enabled) { s_Enabled = enabled; }
    static bool IsEnabled() { return s_Enabled; }

    static void Log(Level level, const std::string& message) {
#ifdef _DEBUG
        if (!s_Enabled) return;
        
        switch (level) {
            case Level::Debug:
                std::cerr << "[DEBUG] " << message << "\n";
                break;
            case Level::Info:
                std::cerr << "[INFO] " << message << "\n";
                break;
            case Level::Warning:
                std::cerr << "[WARNING] " << message << "\n";
                break;
            case Level::Error:
                std::cerr << "[ERROR] " << message << "\n";
                break;
        }
#endif
    }

    static void Debug(const std::string& msg) { Log(Level::Debug, msg); }
    static void Info(const std::string& msg) { Log(Level::Info, msg); }
    static void Warning(const std::string& msg) { Log(Level::Warning, msg); }
    static void Error(const std::string& msg) { Log(Level::Error, msg); }

private:
    static bool s_Enabled;
};

} // namespace Archura

// Convenience macro
#ifdef _DEBUG
    #define ARCH_LOG_DEBUG(msg) Archura::Logger::Debug(msg)
    #define ARCH_LOG_ERROR(msg) Archura::Logger::Error(msg)
    #define ARCH_LOG_WARN(msg) Archura::Logger::Warning(msg)
#else
    #define ARCH_LOG_DEBUG(msg)
    #define ARCH_LOG_ERROR(msg)
    #define ARCH_LOG_WARN(msg)
#endif
