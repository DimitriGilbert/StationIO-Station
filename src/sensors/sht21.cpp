#include "./sht21.h"

#include <Arduino.h>

#include "../html.h"

sht21::sht21() : Sensor("sht21") {
  this->mesuresSampleLast[0] = (unsigned long)1000;
  this->mesuresSampleLast[1] = (unsigned long)1000;
  this->mesuresSampleLast[2] = (unsigned long)1000;
};
sht21::~sht21(){};

const sht21::SensorMesure sht21::mesures[3] = {
    {"temperature", "*C"},
    {"humidity", "%"},
    {"compensated_humidity", "%"},
};
const u_int* sht21::mesuresSampleRates[3] = {
    (const u_int*)5000,
    (const u_int*)5000,
    (const u_int*)5000,
};

// void sht21::onSetup(StationClass station, int index) {}
void sht21::begin() {
  Serial.println("sht21::begin()");
  this->sht.begin() ? this->status = Sensor::StatusReady
                    : (this->status = Sensor::StatusError) &&
                          (this->error = Sensor::ErrorNotFound);

  Serial.println("sht21::dobegin");
}
String sht21::getName() { return this->name; }
String sht21::getMesureName(u_int index) { return this->mesures[index].name; }
Sensor::SensorMesure sht21::getMesure(u_int index) {
  return this->mesures[index];
}
size_t sht21::getMesuresCount() {
  return this->mesuresCount;
}
Sensor::SensorMesureData* sht21::read_() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
    this->mesuresBuffers[i].unshift(this->mesuresDatas[i]);
  }
  return this->mesuresDatas;
}
Sensor::SensorMesureData sht21::read_(int index) {
  if (index < this->mesuresCount) {
    Sensor::SensorMesureData value = 0;
    this->mesuresSampleLast[index] = millis();
    switch (index) {
      // mesure temperature
      case 0:
        value = this->sht.readTemperature();
        break;

      // mesure humidity
      case 1:
        value = this->sht.readHumidity();
        break;

      // mesure compensated_humidity
      case 2:
        value = this->sht.getCompensatedHumidity(this->read_(0));
        break;
    }
    this->setMesure(index, value);
    return this->mesuresDatas[index];
  }
  return this->read_(0);
}
void sht21::setMesure(int index, float value) {
  this->mesuresDatas[index] = value;
  this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
}

void sht21::loop() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    if (millis() - this->mesuresSampleLast[i] >
        (unsigned long)this->mesuresSampleRates[i]) {
      this->read_(i);
    }
  }
}

Sensor::SensorMesureData* sht21::read() {
  return this->mesuresDatas;
}

Sensor::SensorMesureData sht21::read(int index) {
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData sht21::readBuffer(int index, int bufferIndex) {
  bufferIndex = bufferIndex > 40 ? 40 : bufferIndex < 0 ? 0 : bufferIndex;
  return this->mesuresBuffers[index][bufferIndex];
}

Sensor::SensorMesureData* sht21::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData sht21::average(int last, int index) {
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

String sht21::toString() {
  return SensorToString(this);
}
String sht21::toString(int index) {
  return SensorMesureToString(this->getMesure(index), this->read(index));
}
String sht21::toCsv() {
  return SensorToCsv(this);
}
String sht21::toCsv(int index) {
  return SensorMesureToCsv(this->getMesure(index), this->read(index));
}
String sht21::toJson() {
  return SensorToJson(this);
}
String sht21::toJson(int index) {
  return SensorMesureToJson(this->getMesure(index), this->read(index));
}
String sht21::toXml() {
  return SensorToXml(this);
}
String sht21::toXml(int index) {
  return SensorMesureToXml(this->getMesure(index), this->read(index));
}
String sht21::jsUtils() {
  return HtmlElt(
      "script",
      "const sht21_utils={inChart: name=>name!=='compensated_humidity'};"
  );
}
String sht21::toHtml() {
  return SensorToHtml(this);
}
String sht21::toHtml(int index) {
  return SensorMesureToHtml(this->getMesure(index), this->read(index));
}
