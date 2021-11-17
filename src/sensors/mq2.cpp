#include <Arduino.h>

#include "./mq2.h"

mq2::mq2(int pin) : Sensor("mq2"), mq(5, 10, pin) {
  this->mq.init();

  this->mesuresSampleLast[0] = (unsigned long)1000;

  this->mesuresSampleLast[1] = (unsigned long)1000;

  this->mesuresSampleLast[2] = (unsigned long)1000;

  this->mesuresSampleLast[3] = (unsigned long)1000;

  this->mesuresSampleLast[4] = (unsigned long)1000;

  this->mesuresSampleLast[5] = (unsigned long)1000;
};
mq2::~mq2(){};

const mq2::SensorMesure mq2::mesures[6] = {
    {"LPG", "ppm"},     {"H2", "ppm"},      {"CO", "ppm"},
    {"Alcohol", "ppm"}, {"Propane", "ppm"}, {"raw", "v"},
};
const u_int* mq2::mesuresSampleRates[6] = {
    (const u_int*)5000, (const u_int*)5000, (const u_int*)5000,
    (const u_int*)5000, (const u_int*)5000, (const u_int*)2000,
};

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
    this->mesuresDatas[index] = value;
    this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
    return this->mesuresDatas[index];
  }
  return this->read_(0);
}

void mq2::loop() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    if (millis() - this->mesuresSampleLast[i] >
        (unsigned long)this->mesuresSampleRates[i]) {
      this->read_(i);
    }
  }
}

Sensor::SensorMesureData* mq2::read() {
  return this->mesuresDatas;
}

Sensor::SensorMesureData mq2::read(int index) {
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
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
  return data / last;
}

String mq2::toString() {
  String out = this->name + "\n";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat("\t" + this->toString(i));
  }
  return out;
}
String mq2::toString(int index) {
  return this->mesures[index].name + ": " + String(this->read(index)) + " " +
         this->mesures[index].unit + "\n";
}
String mq2::toCsv() {
  String out = "";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toCsv(i));
  }
  return out;
}
String mq2::toCsv(int index) {
  return "\"" + this->name + "\", \"" + this->mesures[index].name + "\", \"" +
         this->read(index) + "\", \"" + this->mesures[index].unit + "\"\n";
}
String mq2::toJson() {
  String out = "\"" + this->name + "\": {";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    if (i > 0) {
      out.concat(", ");
    }
    out.concat(this->toJson(i));
  }
  out.concat("}");
  return out;
}
String mq2::toJson(int index) {
  return "\"" + this->mesures[index].name + "\":" + String(this->read(index));
}
String mq2::toXml() {
  String out = "<" + this->name + ">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toXml(i));
  }
  out.concat("</" + this->name + ">");
  return out;
}
String mq2::toXml(int index) {
  return "<" + this->mesures[index].name +
         " unit=\"+this->mesures[index].unit+\">" + String(this->read(index)) +
         "</" + this->mesures[index].name + ">";
}
