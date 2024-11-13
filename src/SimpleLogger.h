#ifndef SIMPLE_LOGGER_H
#define SIMPLE_LOGGER_H

#include <Arduino.h>

#include <HardwareSerial.h>

class SimpleLogger {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    SimpleLogger();
    
    void begin(HardwareSerial& serial, long baud_rate = 9600);
    void setLogLevel(LogLevel level);
    void log(const String& message, LogLevel level = INFO);
    void logf(const char* format, LogLevel level = INFO, ...);

    // Convenience methods for different log levels
    void debug(const String& message);
    void info(const String& message);
    void warning(const String& message);
    void error(const String& message);

private:
    HardwareSerial* _serial;
    LogLevel _minLevel;
    
    const char* levelToString(LogLevel level) const;
    void printTimestamp() const;
};

#endif // SIMPLE_LOGGER_H