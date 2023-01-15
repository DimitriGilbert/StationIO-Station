#include "./bmp280.h"

#include <Arduino.h>

#include "../html.h"

bmp280::bmp280() : Sensor("bmp280") {
  this->mesuresSampleLast[0] = (unsigned long)0;
  this->mesuresSampleLast[1] = (unsigned long)0;
  this->mesuresSampleLast[2] = (unsigned long)5000;
};
bmp280::~bmp280(){};

const bmp280::SensorMesure bmp280::mesures[3] = {
    {"temperature", "*C"},
    {"pressure", "Pa"},
    {"altitude", "m"},
};
const u_int* bmp280::mesuresSampleRates[3] = {
    (const u_int*)5000,
    (const u_int*)5000,
    (const u_int*)60000,
};

// void bmp280::onSetup(StationClass station, int index) {}
void bmp280::begin(){
  this->bmp.begin() || this->bmp.begin(BMP280_ADDRESS_ALT)
      ? this->status = Sensor::StatusReady
      : (this->status = Sensor::StatusError) &&
            (this->error = Sensor::ErrorNotFound);
}
String bmp280::getName() { return this->name; }
String bmp280::getMesureName(u_int index) { return this->mesures[index].name; }
Sensor::SensorMesure bmp280::getMesure(u_int index) {
  return this->mesures[index];
}
size_t bmp280::getMesuresCount() { return this->mesuresCount; }
Sensor::SensorMesureData* bmp280::read_() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
    this->mesuresBuffers[i].unshift(this->mesuresDatas[i]);
  }
  return this->mesuresDatas;
}
Sensor::SensorMesureData bmp280::read_(int index) {
  if (index < this->mesuresCount) {
    Sensor::SensorMesureData value = 0;
    this->mesuresSampleLast[index] = millis();
    switch (index) {
      // mesure temperature
      case 0:
        value = this->bmp.readTemperature();
        break;

      // mesure pressure
      case 1:
        value = this->bmp.readPressure();
        break;

      // mesure altitude
      case 2:
        value = this->bmp.readAltitude(1013);
        break;
    }
    this->setMesure(index, value);
    return this->mesuresDatas[index];
  }
  return this->read_(0);
}
void bmp280::setMesure(int index, float value) {
  this->mesuresDatas[index] = value;
  this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
}
void bmp280::loop() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    if (millis() - this->mesuresSampleLast[i] >
        (unsigned long)this->mesuresSampleRates[i]) {
      this->read_(i);
    }
  }
}

Sensor::SensorMesureData* bmp280::read() {
  return this->mesuresDatas;
}

Sensor::SensorMesureData bmp280::read(int index) {
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData bmp280::readBuffer(int index, int bufferIndex) {
  bufferIndex = bufferIndex > 40 ? 40 : bufferIndex < 0 ? 0 : bufferIndex;
  return this->mesuresBuffers[index][bufferIndex];
}

Sensor::SensorMesureData* bmp280::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData bmp280::average(int last, int index) {
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

String bmp280::toString() {
  return SensorToString(this);
}
String bmp280::toString(int index) {
  return SensorMesureToString(this->getMesure(index), this->read(index));
}
String bmp280::toCsv() {
  return SensorToCsv(this);
}
String bmp280::toCsv(int index) {
  return SensorMesureToCsv(this->getMesure(index), this->read(index));
}
String bmp280::toJson() {
  return SensorToJson(this);
}
String bmp280::toJson(int index) {
  return SensorMesureToJson(this->getMesure(index), this->read(index));
}
String bmp280::toXml() {
  return SensorToXml(this);
}
String bmp280::toXml(int index) {
  return SensorMesureToXml(this->getMesure(index), this->read(index));
}
String bmp280::jsUtils() {
  return HtmlElt(
      "script",
      "const bmp280_utils = {format_pressure:val=>parseFloat(val)/100"
      ",inChart: (name) => name !== 'altitude',format_pressure_unit:val=>hPa};"
  );
}
String bmp280::toHtml() {
  return SensorToHtml(this);
}
String bmp280::toHtml(int index) {
  return SensorMesureToHtml(this->getMesure(index), this->read(index));
}