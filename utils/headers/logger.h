#pragma once

#include <string>

enum class LogEventType
{
    Lifecycle,
    Request,
    Response,
    Notification,
    Internal
};

enum class LogSeverity
{
    Info,
    Warning,
    Error
};

bool initialiseLogger(const std::string &directory, std::string &out_logfile);
bool logEvent(const std::string &logfile,
              const std::string &event_description,
              LogEventType event_type,
              LogSeverity severity);
