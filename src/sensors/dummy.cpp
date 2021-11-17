#include "./dummy.h"

#include <Arduino.h>

dummy::dummy() : Sensor("DummySensor Ta mere la pute !"){};
dummy::~dummy(){};

const dummy::SensorMesure dummy::mesures[4] = {{"temperature", "C"},
                                               {"pressure", "Pa"},
                                               {"humidity", "%"},
                                               {"wind", "m/s"}};
const u_int *dummy::mesuresSampleRates[4] = {0, 0, 0, 0};

dummy::SensorMesureData *dummy::read_() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresBuffers[i].unshift(this->read(i));
    this->mesuresDatas[i] = this->mesuresBuffers[i][0];
  }
  return this->mesuresDatas;
}
dummy::SensorMesureData dummy::read_(int index) { return 143.47; }

void dummy::loop() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
  }
}

Sensor::SensorMesureData *dummy::read() { return this->mesuresDatas; }

Sensor::SensorMesureData dummy::read(int index) {
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData *dummy::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData dummy::average(int last, int index) {
  return this->__average(last, index, this->mesuresCount, this->mesuresBuffers);
}