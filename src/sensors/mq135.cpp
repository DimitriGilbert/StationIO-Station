#include "./mq135.h"

#include <Arduino.h>

#include "../html.h"

#ifdef ESP32
mq135::mq135(int pin) : Sensor("mq135"), mq(3.3, 12, pin) {
  this->mesuresSampleLast[0] = (unsigned long)1000;

  this->mesuresSampleLast[1] = (unsigned long)1000;

  this->mesuresSampleLast[2] = (unsigned long)1000;

  this->mesuresSampleLast[3] = (unsigned long)1000;

  this->mesuresSampleLast[4] = (unsigned long)1000;

  this->mesuresSampleLast[5] = (unsigned long)1000;

  this->mesuresSampleLast[6] = (unsigned long)1000;
};
#else
mq135::mq135(int pin) : Sensor("mq135"), mq(3.3, 10, pin) {
  this->mesuresSampleLast[0] = (unsigned long)1000;

  this->mesuresSampleLast[1] = (unsigned long)1000;

  this->mesuresSampleLast[2] = (unsigned long)1000;

  this->mesuresSampleLast[3] = (unsigned long)1000;

  this->mesuresSampleLast[4] = (unsigned long)1000;

  this->mesuresSampleLast[5] = (unsigned long)1000;

  this->mesuresSampleLast[6] = (unsigned long)1000;
};
#endif

mq135::~mq135(){};
const mq135::SensorMesure mq135::mesures[7] = {
    {"CO", "ppm"},  {"Alcohol", "ppm"}, {"CO2", "ppm"}, {"Toluen", "ppm"},
    {"NH4", "ppm"}, {"Aceton", "ppm"},  {"raw", "v"}};
const u_int* mq135::mesuresSampleRates[7] = {
    (const u_int*)5000, (const u_int*)5000, (const u_int*)5000,
    (const u_int*)5000, (const u_int*)5000, (const u_int*)5000,
    (const u_int*)1000,
};

// void mq135::onSetup(StationClass station, int index) {}
void mq135::begin() { this->mq.init(); }
String mq135::getName() { return this->name; }
String mq135::getMesureName(u_int index) { return this->mesures[index].name; }
Sensor::SensorMesure mq135::getMesure(u_int index) {
  return this->mesures[index];
}
size_t mq135::getMesuresCount() { return this->mesuresCount; }
Sensor::SensorMesureData* mq135::read_() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
    this->mesuresBuffers[i].unshift(this->mesuresDatas[i]);
  }
  return this->mesuresDatas;
}
Sensor::SensorMesureData mq135::read_(int index) {
  if (index < this->mesuresCount) {
    Sensor::SensorMesureData value = 0;
    this->mesuresSampleLast[index] = millis();
    // this->mq.serialDebug(true);
    switch (index) {
      // mesure CO
      case 0:
        value = this->mq.readCO();
        break;

      // mesure Alcohol
      case 1:
        value = this->mq.readAlcohol();
        break;

      // mesure CO2
      case 2:
        value = this->mq.readCO2();
        break;

      // mesure Toluen
      case 3:
        value = this->mq.readToluen();
        break;

      // mesure NH4
      case 4:
        value = this->mq.readNH4();
        break;

      // mesure Aceton
      case 5:
        value = this->mq.readAceton();
        break;

      // mesure raw
      case 6:
        value = this->mq.sensor_volt;
        break;
    }
    this->setMesure(index, value);
    return this->mesuresDatas[index];
  }
  return this->read_(0);
}
void mq135::setMesure(int index, float value) {
  this->mesuresDatas[index] = value;
  this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
}

void mq135::loop() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    if (millis() - this->mesuresSampleLast[i] >
        (unsigned long)this->mesuresSampleRates[i]) {
      this->read_(i);
    }
  }
}

Sensor::SensorMesureData* mq135::read() { return this->mesuresDatas; }

Sensor::SensorMesureData mq135::read(int index) {
  if (index == 5) {
    return this->average(5, index);
  }
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData mq135::readBuffer(int index, int bufferIndex) {
  bufferIndex = bufferIndex > 40 ? 40 : bufferIndex < 0 ? 0 : bufferIndex;
  return this->mesuresBuffers[index][bufferIndex];
}

Sensor::SensorMesureData* mq135::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData mq135::average(int last, int index) {
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

String mq135::toString() {
  return SensorToString(this);
}
String mq135::toString(int index) {
  return SensorMesureToString(this->getMesure(index), this->read(index));
}
String mq135::toCsv() {
  return SensorToCsv(this);
}
String mq135::toCsv(int index) {
  return SensorMesureToCsv(this->getMesure(index), this->read(index));
}
String mq135::toJson() {
  return SensorToJson(this);
}
String mq135::toJson(int index) {
  return SensorMesureToJson(this->getMesure(index), this->read(index));
}
String mq135::toXml() {
  return SensorToXml(this);
}
String mq135::toXml(int index) {
  return SensorMesureToXml(this->getMesure(index), this->read(index));
}
String mq135::jsUtils() {
  return HtmlElt("script", "const mq135_utils={inChart:name=>name==='raw'};");
}
String mq135::toHtml() {
  return SensorToHtml(this);
}
String mq135::toHtml(int index) {
  return SensorMesureToHtml(this->getMesure(index), this->read(index));
}