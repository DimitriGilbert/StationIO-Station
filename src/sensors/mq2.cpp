#include "./mq2.h"

#include <Arduino.h>

#include "./html.h"

#ifdef ESP32
mq2::mq2(int pin) : Sensor("mq2"), mq(3.3, 12, pin) {
  this->mesuresSampleLast[0] = (unsigned long)1000;

  this->mesuresSampleLast[1] = (unsigned long)1000;

  this->mesuresSampleLast[2] = (unsigned long)1000;

  this->mesuresSampleLast[3] = (unsigned long)1000;

  this->mesuresSampleLast[4] = (unsigned long)1000;

  this->mesuresSampleLast[5] = (unsigned long)1000;
};
#else
mq2::mq2(int pin) : Sensor("mq2"), mq(3.3, 10, pin) {
  this->mesuresSampleLast[0] = (unsigned long)1000;

  this->mesuresSampleLast[1] = (unsigned long)1000;

  this->mesuresSampleLast[2] = (unsigned long)1000;

  this->mesuresSampleLast[3] = (unsigned long)1000;

  this->mesuresSampleLast[4] = (unsigned long)1000;

  this->mesuresSampleLast[5] = (unsigned long)1000;
};
#endif
mq2::~mq2(){};

const mq2::SensorMesure mq2::mesures[6] = {
    {"LPG", "ppm"},     {"H2", "ppm"},      {"CO", "ppm"},
    {"Alcohol", "ppm"}, {"Propane", "ppm"}, {"raw", "v"},
};
const u_int* mq2::mesuresSampleRates[6] = {
    (const u_int*)5000, (const u_int*)5000, (const u_int*)5000,
    (const u_int*)5000, (const u_int*)5000, (const u_int*)1000,
};

// void mq2::onSetup(StationClass station, int index) {}
void mq2::begin() { this->mq.init(); }
String mq2::getName() { return this->name; }
String mq2::getMesureName(u_int index) { return this->mesures[index].name; }
Sensor::SensorMesure mq2::getMesure(u_int index) {
  return this->mesures[index];
}
size_t mq2::getMesuresCount() { return this->mesuresCount; }
Sensor::SensorMesureData* mq2::read_() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
    this->mesuresBuffers[i].unshift(this->mesuresDatas[i]);
  }
  return this->mesuresDatas;
}
Sensor::SensorMesureData mq2::read_(int index) {
  if (index < this->mesuresCount) {
    Sensor::SensorMesureData value = 0;
    this->mesuresSampleLast[index] = millis();
    // this->mq.serialDebug(true);
    switch (index) {
      // mesure LPG
      case 0:
        value = this->mq.readLPG();
        break;

      // mesure H2
      case 1:
        value = this->mq.readH2();
        break;

      // mesure CO
      case 2:
        value = this->mq.readCO();
        break;

      // mesure Alcohol
      case 3:
        value = this->mq.readAlcohol();
        break;

      // mesure Propane
      case 4:
        value = this->mq.readPropane();
        break;

      // mesure raw
      case 5:
        value = this->mq.sensor_volt;
        break;
    }
    this->setMesure(index, value);
    return this->mesuresDatas[index];
  }
  return this->read_(0);
}
void mq2::setMesure(int index, float value) {
  this->mesuresDatas[index] = value;
  this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
}

void mq2::loop() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    if (millis() - this->mesuresSampleLast[i] >
        (unsigned long)this->mesuresSampleRates[i]) {
      this->read_(i);
    }
  }
}

Sensor::SensorMesureData* mq2::read() { return this->mesuresDatas; }

Sensor::SensorMesureData mq2::read(int index) {
  if (index == 5) {
    return this->average(5, index);
  }
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData mq2::readBuffer(int index, int bufferIndex) {
  bufferIndex = bufferIndex > 40 ? 40 : bufferIndex < 0 ? 0 : bufferIndex;
  return this->mesuresBuffers[index][bufferIndex];
}

Sensor::SensorMesureData* mq2::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData mq2::average(int last, int index) {
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

String mq2::toString() {
  return SensorToString(this);
}
String mq2::toString(int index) {
  return SensorMesureToString(this->getMesure(index), this->read(index));
}
String mq2::toCsv() {
  return SensorToCsv(this);
}
String mq2::toCsv(int index) {
  return SensorMesureToCsv(this->getMesure(index), this->read(index));
}
String mq2::toJson() {
  return SensorToJson(this);
}
String mq2::toJson(int index) {
  return SensorMesureToJson(this->getMesure(index), this->read(index));
}
String mq2::toXml() {
  return SensorToXml(this);
}
String mq2::toXml(int index) {
  return SensorMesureToXml(this->getMesure(index), this->read(index));
}
String mq2::jsUtils() {
  return HtmlElt("script", "const mq2_utils={inChart:name=>name==='raw'};");
}
String mq2::toHtml() {
  return SensorToHtml(this);
}
String mq2::toHtml(int index) {
  return SensorMesureToHtml(this->getMesure(index), this->read(index));
}