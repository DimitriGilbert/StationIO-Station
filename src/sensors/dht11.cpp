#include "./dht11.h"

#include <Arduino.h>

#include "../html.h"

dht11::dht11(int pin) : Sensor("dht11"), dht(pin, DHT11) {
  this->mesuresSampleLast[0] = (unsigned long)1000;
  this->mesuresSampleLast[1] = (unsigned long)1000;
  this->mesuresSampleLast[2] = (unsigned long)1000;
};
dht11::~dht11(){};

const dht11::SensorMesure dht11::mesures[3] = {
    {"temperature", "*C"},
    {"humidity", "%"},
    {"heat_index", "%"},
};
const u_int* dht11::mesuresSampleRates[3] = {
    (const u_int*)5000,
    (const u_int*)5000,
    (const u_int*)5000,
};

// void dht11::onSetup(StationClass station, int index) {}
void dht11::begin() {
  this->dht.begin();
  this->status = Sensor::StatusReady;
}
String dht11::getName() { return this->name; }
String dht11::getMesureName(u_int index) { return this->mesures[index].name; }
Sensor::SensorMesure dht11::getMesure(u_int index) {
  return this->mesures[index];
}
size_t dht11::getMesuresCount() { return this->mesuresCount; }
Sensor::SensorMesureData* dht11::read_() {
  for (int i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
    this->mesuresBuffers[i].unshift(this->mesuresDatas[i]);
  }
  return this->mesuresDatas;
}
Sensor::SensorMesureData dht11::read_(int index) {
  if (index < this->mesuresCount) {
    Sensor::SensorMesureData value = 0;
    this->mesuresSampleLast[index] = millis();
    switch (index) {
      // mesure temperature
      case 0:
        value = this->dht.readTemperature();
        break;

      // mesure humidity
      case 1:
        value = this->dht.readHumidity();
        break;

      // mesure heat_index
      case 2:
        value = this->dht.computeHeatIndex(this->read(0), this->read(1), false);
        break;
    }
    this->setMesure(index, value);
    return this->mesuresDatas[index];
  }
  return this->read_(0);
}
void dht11::setMesure(int index, float value) {
  this->mesuresDatas[index] = value;
  this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
}

void dht11::loop() {
  // Serial.println("dht11::loop");
  for (size_t i = 0; i < this->mesuresCount; i++) {
    if (millis() - this->mesuresSampleLast[i] >
        (unsigned long)this->mesuresSampleRates[i]) {
      this->read_(i);
    }
  }
}

Sensor::SensorMesureData* dht11::read() {
  return this->mesuresDatas;
}

Sensor::SensorMesureData dht11::read(int index) {
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData dht11::readBuffer(int index, int bufferIndex) {
  bufferIndex = bufferIndex > 40 ? 40 : bufferIndex < 0 ? 0 : bufferIndex;
  return this->mesuresBuffers[index][bufferIndex];
}

Sensor::SensorMesureData* dht11::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData dht11::average(int last, int index) {
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

String dht11::toString() {
  return SensorToString(this);
}
String dht11::toString(int index) {
  return SensorMesureToString(this->getMesure(index), this->read(index));
}
String dht11::toCsv() {
  return SensorToCsv(this);
}
String dht11::toCsv(int index) {
  return SensorMesureToCsv(this->getMesure(index), this->read(index));
}
String dht11::toJson() {
  return SensorToJson(this);
}
String dht11::toJson(int index) {
  return SensorMesureToJson(this->getMesure(index), this->read(index));
}
String dht11::toXml() {
  return SensorToXml(this);
}
String dht11::toXml(int index) {
  return SensorMesureToXml(this->getMesure(index), this->read(index));
}
String dht11::jsUtils() {
  return "";
}
String dht11::toHtml() {
  return SensorToHtml(this);
}
String dht11::toHtml(int index) {
  return SensorMesureToHtml(this->getMesure(index), this->read(index));
}