#include <SimpleLogger.h>

void SimpleLogger::begin() {
  begin(9600);
}

void SimpleLogger::begin(int16_t speed) {
  uint32_t br = sr.baudRate();
  if (br == 0) {
    sr.begin(speed);
  } else if (br != speed) {
    sr.updateBaudRate(speed);
  }
}

void SimpleLogger::begin(HardwareSerial& serial) {
  sr = serial;
  begin();
}

void SimpleLogger::begin(HardwareSerial& serial, int16_t speed) {
  sr = serial;
  begin(speed);
}

void SimpleLogger::log(const String& data) {
  if (level <= this->level) {
    sr.println(data);
  }
}

void SimpleLogger::log(const String& data, int8_t level) {
  if (level <= this->level) {
    sr.println(data);
  }
}

void SimpleLogger::log(const char*& data) {
  if (level <= this->level) {
    sr.println(data);
  }
}

void SimpleLogger::log(const char*& data, int8_t level) {
  if (level <= this->level) {
    sr.println(data);
  }
}

void SimpleLogger::logt(const String& data) {
  if (level <= this->level) {
    log(("\t"+String(data)).c_str(), level);
  }
}

void SimpleLogger::logt(const String& data, int8_t level) {
  if (level <= this->level) {
    log(("\t"+String(data)).c_str(), level);
  }
}

void SimpleLogger::logt(const char*& data) {
  if (level <= this->level) {
    log(("\t"+String(data)).c_str(), level);
  }
}

void SimpleLogger::logt(const char*& data, int8_t level) {
  if (level <= this->level) {
    log(("\t"+String(data)).c_str(), level);
  }
}

