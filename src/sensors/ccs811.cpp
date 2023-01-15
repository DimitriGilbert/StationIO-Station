#include "./ccs811.h"

#include <Arduino.h>

#include "../html.h"

ccs811::ccs811() : Sensor("ccs811") {
  this->mesuresSampleLast[0] = (unsigned long)1000;
  this->mesuresSampleLast[1] = (unsigned long)1000;
  this->mesuresSampleLast[2] = (unsigned long)1000;
};
ccs811::~ccs811(){};

const ccs811::SensorMesure ccs811::mesures[3] = {
    {"eCO2", ""},
    {"VOC", "ppm"},
    {"temperature", "*C"},
};
const u_int* ccs811::mesuresSampleRates[3] = {
    (const u_int*)5000,
    (const u_int*)5000,
    (const u_int*)5000,
};

// void ccs811::onSetup(StationClass station, int index) {}
void ccs811::begin() {
  this->ccs.begin() ? this->status = Sensor::StatusReady
                    : (this->status = Sensor::StatusError) &&
                          (this->error = Sensor::ErrorNotFound);
  while (!ccs.available()) {
    Serial.println("\t waiting for ccs811");
  }
}
String ccs811::getName() { return this->name; }
String ccs811::getMesureName(u_int index) { return this->mesures[index].name; }
Sensor::SensorMesure ccs811::getMesure(u_int index) {
  return this->mesures[index];
}
size_t ccs811::getMesuresCount() { return this->mesuresCount; }
Sensor::SensorMesureData* ccs811::read_() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
    this->mesuresBuffers[i].unshift(this->mesuresDatas[i]);
  }
  return this->mesuresDatas;
}
Sensor::SensorMesureData ccs811::read_(int index) {
  if (index < this->mesuresCount) {
    Sensor::SensorMesureData value = -999;
    this->mesuresSampleLast[index] = millis();
    if (this->ccs.readData()) {
      switch (index) {
        // mesure eCO2
        case 0:
          value = this->ccs.geteCO2();
          break;

        // mesure VOC
        case 1:
          value = this->ccs.getTVOC();
          break;

        // mesure temperature
        case 2:
          value = this->ccs.calculateTemperature();
          break;
      }
    }
    this->setMesure(index, value);
    return this->mesuresDatas[index];
  }
  return this->read_(0);
}
void ccs811::setMesure(int index, float value) {
  this->mesuresDatas[index] = value;
  this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
}

void ccs811::loop() {
  bool read = false;
  for (size_t i = 0; i < this->mesuresCount; i++) {
    if (millis() - this->mesuresSampleLast[i] >
            (unsigned long)this->mesuresSampleRates[i] &&
        this->ccs.available()) {
      if (!read) {
        this->ccs.readData();
        read = true;
      }
      this->read_(i);
    }
  }
}

Sensor::SensorMesureData* ccs811::read() {
  return this->mesuresDatas;
}

Sensor::SensorMesureData ccs811::read(int index) {
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData ccs811::readBuffer(int index, int bufferIndex) {
  bufferIndex = bufferIndex > 40 ? 40 : bufferIndex < 0 ? 0 : bufferIndex;
  return this->mesuresBuffers[index][bufferIndex];
}

Sensor::SensorMesureData* ccs811::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData ccs811::average(int last, int index) {
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

String ccs811::toString() {
  return SensorToString(this);
}
String ccs811::toString(int index) {
  return SensorMesureToString(this->getMesure(index), this->read(index));
}
String ccs811::toCsv() {
  return SensorToCsv(this);
}
String ccs811::toCsv(int index) {
  return SensorMesureToCsv(this->getMesure(index), this->read(index));
}
String ccs811::toJson() {
  return SensorToJson(this);
}
String ccs811::toJson(int index) {
  return SensorMesureToJson(this->getMesure(index), this->read(index));
}
String ccs811::toXml() {
  return SensorToXml(this);
}
String ccs811::toXml(int index) {
  return SensorMesureToXml(this->getMesure(index), this->read(index));
}
String ccs811::toHtml() {
  return SensorToHtml(this);
}
String ccs811::toHtml(int index) {
  return SensorMesureToHtml(this->getMesure(index), this->read(index));
}