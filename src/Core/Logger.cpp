#include "Tactics/Core/Logger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

namespace Tactics
{

    Logger::Logger() : m_level(LogLevel::Info), m_file_logging_enabled(false) {}

    Logger::~Logger()
    {
        if (m_log_file.is_open())
        {
            m_log_file.close();
        }
    }

    auto Logger::instance() -> Logger &
    {
        static Logger instance;
        return instance;
    }

    void Logger::set_level(LogLevel level)
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        m_level = level;
    }

    void Logger::set_file_logging(bool enabled, const std::string &file_path)
    {
        std::scoped_lock<std::mutex> lock(m_mutex);

        if (m_log_file.is_open())
        {
            m_log_file.close();
        }

        m_file_logging_enabled = enabled;

        if (enabled)
        {
            m_log_file.open(file_path, std::ios::app);
            if (!m_log_file.is_open())
            {
                m_file_logging_enabled = false;
                error("Failed to open log file: " + std::string(file_path));
            }
        }
    }

    void Logger::debug(std::string_view message)
    {
        if (should_log(LogLevel::Debug))
        {
            log(LogLevel::Debug, message);
        }
    }

    void Logger::info(std::string_view message)
    {
        if (should_log(LogLevel::Info))
        {
            log(LogLevel::Info, message);
        }
    }

    void Logger::warning(std::string_view message)
    {
        if (should_log(LogLevel::Warning))
        {
            log(LogLevel::Warning, message);
        }
    }

    void Logger::error(std::string_view message)
    {
        if (should_log(LogLevel::Error))
        {
            log(LogLevel::Error, message);
        }
    }

    auto Logger::should_log(LogLevel level) const -> bool
    {
        return level >= m_level;
    }

    void Logger::log(LogLevel level, std::string_view message)
    {
        std::scoped_lock<std::mutex> lock(m_mutex);

        std::string log_line = "[" + std::string(Logger::get_timestamp()) + "] [" +
                               std::string(Logger::level_to_string(level)) + "] " +
                               std::string(message) + "\n";

        // Output to console
        if (level >= LogLevel::Warning)
        {
            std::cerr << log_line;
        }
        else
        {
            std::cout << log_line;
        }

        // Output to file if enabled
        if (m_file_logging_enabled && m_log_file.is_open())
        {
            m_log_file << log_line;
            m_log_file.flush();
        }
    }

    auto Logger::level_to_string(LogLevel level) -> std::string_view
    {
        switch (level)
        {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }

    auto Logger::get_timestamp() -> std::string
    {
        constexpr int MILLISECONDS_PER_SECOND = 1000;

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto milliseconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) %
            MILLISECONDS_PER_SECOND;

        std::stringstream stream;
        stream << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        stream << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();

        return stream.str();
    }

    // Convenience functions for easier logging
    void log_debug(std::string_view message)
    {
        Tactics::Logger::instance().debug(message);
    }
    void log_info(std::string_view message)
    {
        Tactics::Logger::instance().info(message);
    }
    void log_warning(std::string_view message)
    {
        Tactics::Logger::instance().warning(message);
    }
    void log_error(std::string_view message)
    {
        Tactics::Logger::instance().error(message);
    }
} // namespace Tactics
