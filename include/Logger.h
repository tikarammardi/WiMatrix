//
// Created by Tikaram Mardi on 19/07/25.
//

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3
};

class Logger {
private:
    static LogLevel currentLogLevel;
    static String getTimestamp();
    static String getLevelString(LogLevel level);

public:
    static void init(unsigned long baudRate = 115200);
    static void setLogLevel(LogLevel level);

    static void debug(const String& message, const String& component = "");
    static void info(const String& message, const String& component = "");
    static void warn(const String& message, const String& component = "");
    static void error(const String& message, const String& component = "");

    // System information logging
    static void logSystemInfo();
    static void logNetworkInfo();
    static void logWebRequest(const String& method, const String& uri, int responseCode);
};

#endif // LOGGER_H

