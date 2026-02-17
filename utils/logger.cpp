// std::string initialiseLogger(){};
// This should return a string, which is the current datetime.
// This should serve as the logfile.
// Should be called at program start

// void log(*std::string logfile, int eventType, std::string eventDescription, int logSeverity){};
// This should write to the file details about the event that happened
// [current datetime] [LogLevel] [type of event] [event description]

// Both eventType and logSeverity should corrospond to std::strings

#include "headers/logger.h"

#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace
{
    bool format_current_time(const char* format, std::string& out)
    {
        // Output current time into out, based on format
        std::time_t now = std::time(nullptr);
        std::tm tm_value{};
        if (localtime_s(&tm_value, &now) != 0)
            return false;

        char buffer[64];
        if (std::strftime(buffer, sizeof(buffer), format, &tm_value) == 0)
            return false;

        out = buffer;
        return true;
    }

    const char* to_string(LogEventType type)
    {
        // LogEventType enum -> string
        // see headers/logger.h
        switch (type)
        {
        case LogEventType::Lifecycle:
            return "Lifecycle";
        case LogEventType::Request:
            return "Request";
        case LogEventType::Response:
            return "Response";
        case LogEventType::Notification:
            return "Notification";
        case LogEventType::Internal:
            return "Internal";
        }
        return "Misc";
    }

    const char* to_string(LogSeverity severity)
    {
        // LogSeverity enum -> string
        // see headers/logger.h
        switch (severity)
        {
        case LogSeverity::Info:
            return "Info";
        case LogSeverity::Warning:
            return "Warning";
        case LogSeverity::Error:
            return "Error";
        }
        return "Unknown";
    }
}

bool initialiseLogger(const std::string& directory, std::string& out_logfile)
{
    // Gets a viable filepath for the log, stores into out_logfile
    std::error_code ec;
    std::filesystem::create_directories(directory, ec);
    if (ec)
        return false;

    // filename of log should be current datetime of initialisation
    std::string stamp;
    if (!format_current_time("%Y-%m-%d_%H-%M-%S", stamp))
        return false;

    out_logfile = directory + "/log-" + stamp + ".txt";

    std::ofstream out(out_logfile, std::ios::out | std::ios::trunc);
    if (!out.is_open())
        return false;

    out << "Log started\n";
    return true;
}

bool logEvent(const std::string& logfile,
    const std::string& event_description,
    LogEventType event_type,
    LogSeverity severity)
{
    // Write event to logfile
    std::string stamp;
    if (!format_current_time("%Y-%m-%d %H:%M:%S", stamp))
        return false;

    std::ofstream out(logfile, std::ios::out | std::ios::app);
    if (!out.is_open())
        return false;

    out << "[" << stamp << "] "
        << "[" << to_string(severity) << "] "
        << "[" << to_string(event_type) << "] "
        << event_description << "\n";

    return true;
}
