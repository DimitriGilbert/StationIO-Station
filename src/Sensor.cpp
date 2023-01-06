#include "./Sensor.h"

Sensor::Sensor(String name) { this->name = name; }
Sensor::~Sensor() {}

bool Sensor::ready() { return this->status > 0; }

Sensor::SensorMesureData Sensor::__read(int index, size_t count,
                                        SensorMesureData *datas) {
  if (size_t(index) > count) {
    return 0;
  }
  return datas[index];
}

Sensor::SensorMesureData Sensor::__average(
    int last, int index, size_t count,
    CircularBuffer<SensorMesureData, 20>buffers[]) {
  if (size_t(index) > count) {
    return 0;
  }
  SensorMesureData data = 0;

  if (last > buffers[index].size()) {
    last = buffers[index].size();
  }

  for (size_t i = 0; i < size_t(last); i++) {
    data = data + buffers[index][i];
  }
  return data / float(last);
}
