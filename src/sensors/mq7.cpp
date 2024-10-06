#include "mq7.h"

#include <Arduino.h>

#include "../html.h"

mq7::mq7(int pin) : Sensor("mq7"), mq(3.3, 12, pin) {
  this->mesuresSampleLast[0] = (unsigned long)1000;
};
mq7::~mq7(){};

const mq7::SensorMesure mq7::mesures[2] = {
  {"CO", "ppm"},{"raw", "v"},
};
const u_int* mq7::mesuresSampleRates[2] = {
    (const u_int*)5000, (const u_int*)1000
};

// void mq7::onSetup(StationClass station, int index) {}
void mq7::begin() { this->mq.init(); }
String mq7::getName() { return this->name; }
String mq7::getMesureName(u_int index) { return this->mesures[index].name; }
Sensor::SensorMesure mq7::getMesure(u_int index) {
  return this->mesures[index];
}
size_t mq7::getMesuresCount() { return this->mesuresCount; }
Sensor::SensorMesureData* mq7::read_() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
    this->mesuresBuffers[i].unshift(this->mesuresDatas[i]);
  }
  return this->mesuresDatas;
}
Sensor::SensorMesureData mq7::read_(int index) {
  if (index < this->mesuresCount) {
    Sensor::SensorMesureData value = 0;
    this->mesuresSampleLast[index] = millis();
    // this->mq.serialDebug(true);
    switch (index) {
      // mesure CO
      case 0:
        value = this->mq.readCO();
        break;
      // mesure raw
      case 1:
        value = this->mq.sensor_volt;
        break;
    }
    this->setMesure(index, value);
    return this->mesuresDatas[index];
  }
  return this->read_(0);
}
void mq7::setMesure(int index, float value) {
  this->mesuresDatas[index] = value;
  this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
}

void mq7::loop() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    if (millis() - this->mesuresSampleLast[i] >
        (unsigned long)this->mesuresSampleRates[i]) {
      this->read_(i);
    }
  }
}

Sensor::SensorMesureData* mq7::read() { return this->mesuresDatas; }

Sensor::SensorMesureData mq7::read(int index) {
  if (index == 5) {
    return this->average(5, index);
  }
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData mq7::readBuffer(int index, int bufferIndex) {
  bufferIndex = bufferIndex > 40 ? 40 : bufferIndex < 0 ? 0 : bufferIndex;
  return this->mesuresBuffers[index][bufferIndex];
}

Sensor::SensorMesureData* mq7::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData mq7::average(int last, int index) {
  // return this->__average(last, index, this->mesuresCount,
  // this->mesuresBuffers);
  if (index > this->mesuresCount) {
    return 0;
  }
  SensorMesureData data = 0;

  if (last > this->mesuresBuffers[index].size()) {
    last = this->mesuresBuffers[index].size();
  }

  for (size_t i = 0; i < last; i++) {
    data = data + this->mesuresBuffers[index][i];
  }
  return data / float(last);
}

String mq7::toString() {
  return SensorToString(this);
}
String mq7::toString(int index) {
  return SensorMesureToString(this->getMesure(index), this->read(index));
}
String mq7::toCsv() {
  return SensorToCsv(this);
}
String mq7::toCsv(int index) {
  return SensorMesureToCsv(this->getMesure(index), this->read(index));
}
String mq7::toJson() {
  return SensorToJson(this);
}
String mq7::toJson(int index) {
  return SensorMesureToJson(this->getMesure(index), this->read(index));
}
String mq7::toXml() {
  return SensorToXml(this);
}
String mq7::toXml(int index) {
  return SensorMesureToXml(this->getMesure(index), this->read(index));
}
String mq7::jsUtils() {
  return HtmlElt("script", "const mq7_utils={inChart:name=>name==='raw'};");
}
String mq7::toHtml() {
  return SensorToHtml(this);
}
String mq7::toHtml(int index) {
  return SensorMesureToHtml(this->getMesure(index), this->read(index));
}