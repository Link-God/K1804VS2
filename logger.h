#pragma once

#include "pch.h"
#include <string>

struct ILogger {
    ILogger() = default;
    virtual ~ILogger() = default;
    virtual void log(const std::string& msg) = 0;
    virtual std::string getLog() = 0;
};

class Logger : public ILogger {
public:
    Logger() = default;
    virtual ~Logger() = default;
    virtual void log(const std::string& msg);
    virtual std::string getLog();
private:
    std::string _log;
};