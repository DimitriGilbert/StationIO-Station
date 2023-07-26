#include <Arduino.h>
#ifndef StationIOLogger_h
#define StationIOLogger_h
#endif

class SimpleLogger {
  public:
  HardwareSerial sr = Serial;
  int8_t level = 0;

  SimpleLogger() {};
  void begin();
  void begin(int16_t speed);
  void begin(HardwareSerial& serial);
  void begin(HardwareSerial& serial, int8_t speed);

  void log(const String& data);
  void log(const String& data, int8_t level);
  void log(const char*& data);
  void log(const char*& data, int8_t level);

  void logt(const String& data);
  void logt(const String& data, int8_t level);
  void logt(const char*& data);
  void logt(const char*& data, int8_t level);
};
