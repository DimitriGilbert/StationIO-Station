#include "./bmp085.h"

bmp085::bmp085() : Sensor("bmp085") {}

void bmp085::begin() {
  if (bmp.begin()) {
    status = StatusReady;
  } else {
    status = StatusError;
    error = ErrorNotFound;
  }
}

Sensor::SensorMeasure bmp085::getMeasure(uint8_t index) const {
  return mesures[index];
}

String bmp085::getMeasureName(uint8_t index) const {
  return mesures[index].name;
}

size_t bmp085::getMeasureCount() const {
  return mesuresCount;
}

Sensor::SensorMeasureData bmp085::read(int index) const {
  if (index >= 0 && index < mesuresCount) {
    return mesuresDatas[index];
  }
  return 0;
}

Sensor::SensorMeasureData bmp085::readBuffer(int index, int bufferIndex) const {
  if (index >= 0 && index < mesuresCount && bufferIndex >= 0 && bufferIndex < mesuresBuffers[index].size()) {
    return mesuresBuffers[index][bufferIndex];
  }
  return 0;
}

void bmp085::setMeasure(int index, float value) {
  if (index >= 0 && index < mesuresCount) {
    mesuresDatas[index] = value;
    mesuresBuffers[index].push(value);
  }
}

Sensor::SensorMeasureData bmp085::average(int last, int index) const {
  if (index >= 0 && index < mesuresCount) {
    float sum = 0;
    int count = min(last, static_cast<int>(mesuresBuffers[index].size()));
    for (int i = 0; i < count; ++i) {
      sum += mesuresBuffers[index][i];
    }
    return sum / count;
  }
  return 0;
}

String bmp085::jsUtils() const {
  return "const bmp085_utils = {format_pressure:val=>parseFloat(val)/100,inChart: (name) => name != 'altitude',format_pressure_unit:val=>hPa};";
}

void bmp085::loop() {
  mesuresDatas[0] = bmp.readTemperature();
  mesuresDatas[1] = bmp.readPressure();
  mesuresDatas[2] = bmp.readAltitude(1013.25);

  for (int i = 0; i < mesuresCount; ++i) {
    mesuresBuffers[i].push(mesuresDatas[i]);
  }
}

const bmp085::SensorMeasure bmp085::mesures[mesuresCount] = {
    {"temperature", "*C"},
    {"pressure", "Pa"},
    {"altitude", "m"},
};