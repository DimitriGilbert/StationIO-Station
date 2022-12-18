#include "./sht21.h"

#include <Arduino.h>

#include "./html.h"

sht21::sht21() : Sensor("sht21") {
  this->sht.begin() ? this->status = Sensor::StatusReady
                    : (this->status = Sensor::StatusError) &&
                          (this->error = Sensor::ErrorNotFound);

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
    this->mesuresDatas[index] = value;
    this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
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
  return data / last;
}

String sht21::toString() {
  String out = this->name + "\n";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat("\t" + this->toString(i));
  }
  return out;
}
String sht21::toString(int index) {
  return this->mesures[index].name + ": " + String(this->read(index)) + " " +
         this->mesures[index].unit + "\n";
}
String sht21::toCsv() {
  String out = "";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toCsv(i));
  }
  return out;
}
String sht21::toCsv(int index) {
  return "\"" + this->name + "\", \"" + this->mesures[index].name + "\", \"" +
         this->read(index) + "\", \"" + this->mesures[index].unit + "\"\n";
}
String sht21::toJson() {
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
String sht21::toJson(int index) {
  return "\"" + this->mesures[index].name + "\":" + String(this->read(index));
}
String sht21::toXml() {
  String out = "<" + this->name + ">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toXml(i));
  }
  out.concat("</" + this->name + ">");
  return out;
}
String sht21::toXml(int index) {
  return "<" + this->mesures[index].name +
         " unit=\""+this->mesures[index].unit+"\">" + String(this->read(index)) +
         "</" + this->mesures[index].name + ">";
}
String sht21::jsUtils() {
  return HtmlElt(
      "script",
      "const sht21_utils = {inChart: name => name!=='compensated_humidity'}"
  );
}
String sht21::toHtml() {
  String out = "<div class=\"sensor " + this->name +
               "\"><div class=\"sensor-name\">" + this->name +
               "</div><div class=\"sensor-mesures\">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toHtml(i));
  }
  out.concat("</div></div>");
  return out;
}
String sht21::toHtml(int index) {
  return "<div class=\"sensor-mesure " + this->mesures[index].name +
         "\"><span class=\"sensor-mesure-name\">" + this->mesures[index].name +
         "</span> : <span class=\"sensor-mesure-value\">" +
         String(this->read(index)) +
         "</span><span class=\"sensor-mesure-unit\">" +
         this->mesures[index].unit + "</span></div>";
}
