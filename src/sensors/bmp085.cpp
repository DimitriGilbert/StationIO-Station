#include <Arduino.h>

#include "./bmp085.h"

bmp085::bmp085() : Sensor("bmp085") {
  this->bmp.begin() ? this->status = Sensor::StatusReady
                    : (this->status = Sensor::StatusError) &&
                          (this->error = Sensor::ErrorNotFound);

  this->mesuresSampleLast[0] = (unsigned long)0;

  this->mesuresSampleLast[1] = (unsigned long)0;

  this->mesuresSampleLast[2] = (unsigned long)5000;
};
bmp085::~bmp085(){};

const bmp085::SensorMesure bmp085::mesures[4] = {
    {"temperature", "*C"},
    {"pressure", "Pa"},
    {"altitude", "m"},
};
const u_int* bmp085::mesuresSampleRates[4] = {
    (const u_int*)5000,
    (const u_int*)5000,
    (const u_int*)60000,
};

size_t bmp085::getMesuresCount() { return this->mesuresCount; }
Sensor::SensorMesureData* bmp085::read_() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
    this->mesuresBuffers[i].unshift(this->mesuresDatas[i]);
  }
  return this->mesuresDatas;
}
Sensor::SensorMesureData bmp085::read_(int index) {
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
void bmp085::setMesure(int index, float value) {
  this->mesuresDatas[index] = value;
  this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
}
void bmp085::loop() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    if (millis() - this->mesuresSampleLast[i] >
        (unsigned long)this->mesuresSampleRates[i]) {
      this->read_(i);
    }
  }
}

Sensor::SensorMesureData* bmp085::read() {
  return this->mesuresDatas;
}

Sensor::SensorMesureData bmp085::read(int index) {
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData bmp085::readBuffer(int index, int bufferIndex) {
  bufferIndex = bufferIndex > 40 ? 40 : bufferIndex < 0 ? 0 : bufferIndex;
  return this->mesuresBuffers[index][bufferIndex];
}

Sensor::SensorMesureData* bmp085::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData bmp085::average(int last, int index) {
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

String bmp085::toString() {
  String out = this->name + "\n";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat("\t" + this->toString(i));
  }
  return out;
}
String bmp085::toString(int index) {
  return this->mesures[index].name + ": " + String(this->read(index)) + " " +
         this->mesures[index].unit + "\n";
}
String bmp085::toCsv() {
  String out = "";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toCsv(i));
  }
  return out;
}
String bmp085::toCsv(int index) {
  return "\"" + this->name + "\", \"" + this->mesures[index].name + "\", \"" +
         this->read(index) + "\", \"" + this->mesures[index].unit + "\"\n";
}
String bmp085::toJson() {
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
String bmp085::toJson(int index) {
  return "\"" + this->mesures[index].name + "\":" + String(this->read(index));
}
String bmp085::toXml() {
  String out = "<" + this->name + ">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toXml(i));
  }
  out.concat("</" + this->name + ">");
  return out;
}
String bmp085::toXml(int index) {
  return "<" + this->mesures[index].name +
         " unit=\""+this->mesures[index].unit+"\">" + String(this->read(index)) +
         "</" + this->mesures[index].name + ">";
}
String bmp085::toHtml() {
  String out = "<div class=\"sensor " + this->name +
               "\"><div class=\"snName\">" + this->name +
               "</div><div class=\"snMss\">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toHtml(i));
  }
  out.concat("</div></div>");
  return out;
}
String bmp085::toHtml(int index) {
  return "<div class=\"snMs " + this->mesures[index].name +
         "\"><span class=\"snMs-name\">" + this->mesures[index].name +
         "</span> : <span class=\"snMs-value\">" +
         String(this->read(index)) +
         "</span><span class=\"snMs-unit\">" +
         this->mesures[index].unit + "</span></div>";
}
