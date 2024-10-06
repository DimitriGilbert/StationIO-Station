#include "SimpleLogger.h"
#include <stdarg.h>

SimpleLogger::SimpleLogger() : _serial(nullptr), _minLevel(INFO) {}

void SimpleLogger::begin(HardwareSerial& serial, long baud_rate) {
    _serial = &serial;
    _serial->begin(baud_rate);
}

void SimpleLogger::setLogLevel(LogLevel level) {
    _minLevel = level;
}

void SimpleLogger::log(const String& message, LogLevel level) {
    if (level >= _minLevel && _serial != nullptr) {
        printTimestamp();
        _serial->print(levelToString(level));
        _serial->print(": ");
        _serial->println(message);
    }
}

void SimpleLogger::logf(const char* format, LogLevel level, ...) {
    if (level >= _minLevel && _serial != nullptr) {
        va_list args;
        va_start(args, level);
        
        printTimestamp();
        _serial->print(levelToString(level));
        _serial->print(": ");
        
        char buf[256];
        vsnprintf(buf, sizeof(buf), format, args);
        _serial->println(buf);
        
        va_end(args);
    }
}

void SimpleLogger::debug(const String& message) {
    log(message, DEBUG);
}

void SimpleLogger::info(const String& message) {
    log(message, INFO);
}

void SimpleLogger::warning(const String& message) {
    log(message, WARNING);
}

void SimpleLogger::error(const String& message) {
    log(message, ERROR);
}

const char* SimpleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case DEBUG:   return "DEBUG";
        case INFO:    return "INFO";
        case WARNING: return "WARNING";
        case ERROR:   return "ERROR";
        default:      return "UNKNOWN";
    }
}

void SimpleLogger::printTimestamp() const {
    unsigned long ms = millis();
    unsigned long seconds = ms / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    _serial->printf("[%02lu:%02lu:%02lu.%03lu] ", hours, minutes % 60, seconds % 60, ms % 1000);
}