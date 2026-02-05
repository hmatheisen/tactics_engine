#pragma once

#include <cstdint>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>

namespace Tactics
{

    enum class LogLevel : std::uint8_t
    {
        Debug,
        Info,
        Warning,
        Error
    };

    class Logger
    {
    public:
        Logger() = default;
        ~Logger();

        // Delete copy constructor and assignment operator
        Logger(const Logger &) = delete;
        auto operator=(const Logger &) -> Logger & = delete;

        // Delete move constructor and assignment operator
        Logger(Logger &&) = delete;
        auto operator=(Logger &&) -> Logger & = delete;

        // Get singleton instance
        static auto instance() -> Logger &;

        // Set log level
        void set_level(LogLevel level);

        // Enable/disable file logging
        void set_file_logging(bool enabled, const std::string &file_path = "tactics.log");

        // Log methods
        void debug(std::string_view message);
        void info(std::string_view message);
        void warning(std::string_view message);
        void error(std::string_view message);

    private:
        LogLevel m_level{LogLevel::Info};
        bool m_file_logging_enabled{false};
        std::ofstream m_log_file;
        mutable std::mutex m_mutex;

        auto should_log(LogLevel level) const -> bool;
        void log(LogLevel level, std::string_view message);
        static auto level_to_string(LogLevel level) -> std::string_view;
        static auto get_timestamp() -> std::string;
    };

    // Convenience functions for easier logging
    void log_debug(std::string_view message);
    void log_info(std::string_view message);
    void log_warning(std::string_view message);
    void log_error(std::string_view message);
} // namespace Tactics
