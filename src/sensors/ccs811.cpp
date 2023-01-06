#include <Arduino.h>

#include "./ccs811.h"

ccs811::ccs811() : Sensor("ccs811") {
  this->ccs.begin() ? this->status = Sensor::StatusReady
                    : (this->status = Sensor::StatusError) &&
                          (this->error = Sensor::ErrorNotFound);
  ;
  while (!ccs.available()) {Serial.println("\t waiting for ccs811");}

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
  String out = this->name + "\n";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat("\t" + this->toString(i));
  }
  return out;
}
String ccs811::toString(int index) {
  return this->mesures[index].name + ": " + String(this->read(index)) + " " +
         this->mesures[index].unit + "\n";
}
String ccs811::toCsv() {
  String out = "";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toCsv(i));
  }
  return out;
}
String ccs811::toCsv(int index) {
  return "\"" + this->name + "\", \"" + this->mesures[index].name + "\", \"" +
         this->read(index) + "\", \"" + this->mesures[index].unit + "\"\n";
}
String ccs811::toJson() {
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
String ccs811::toJson(int index) {
  return "\"" + this->mesures[index].name + "\":" + String(this->read(index));
}
String ccs811::toXml() {
  String out = "<" + this->name + ">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toXml(i));
  }
  out.concat("</" + this->name + ">");
  return out;
}
String ccs811::toXml(int index) {
  return "<" + this->mesures[index].name +
         " unit=\""+this->mesures[index].unit+"\">" + String(this->read(index)) +
         "</" + this->mesures[index].name + ">";
}
String ccs811::toHtml() {
  String out = "<div class=\"sensor " + this->name +
               "\"><div class=\"snName\">" + this->name +
               "</div><div class=\"snMss\">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toHtml(i));
  }
  out.concat("</div></div>");
  return out;
}
String ccs811::toHtml(int index) {
  return "<div class=\"snMs " + this->mesures[index].name +
         "\"><span class=\"snMs-name\">" + this->mesures[index].name +
         "</span> : <span class=\"snMs-value\">" +
         String(this->read(index)) +
         "</span><span class=\"snMs-unit\">" +
         this->mesures[index].unit + "</span></div>";
}