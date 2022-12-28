#include "./bme280.h"

#include <Arduino.h>
#include <html.h>

bme280::bme280() : Sensor("bme280") {
  this->bme.begin() || this->bme.begin(0x76)
      ? this->status = Sensor::StatusReady
      : (this->status = Sensor::StatusError) &&
            (this->error = Sensor::ErrorNotFound);

  this->mesuresSampleLast[0] = (unsigned long)0;

  this->mesuresSampleLast[1] = (unsigned long)0;

  this->mesuresSampleLast[2] = (unsigned long)0;

  this->mesuresSampleLast[3] = (unsigned long)5000;
};
bme280::~bme280(){};

const bme280::SensorMesure bme280::mesures[4] = {
    {"temperature", "*C"},
    {"humidity", "%"},
    {"pressure", "Pa"},
    {"altitude", "m"},
};
const u_int* bme280::mesuresSampleRates[4] = {
    (const u_int*)5000,
    (const u_int*)5000,
    (const u_int*)5000,
    (const u_int*)60000,
};

size_t bme280::getMesuresCount() { return this->mesuresCount; }
Sensor::SensorMesureData* bme280::read_() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
    this->mesuresBuffers[i].unshift(this->mesuresDatas[i]);
  }
  return this->mesuresDatas;
}
Sensor::SensorMesureData bme280::read_(int index) {
  if (index < this->mesuresCount) {
    Sensor::SensorMesureData value = 0;
    this->mesuresSampleLast[index] = millis();
    switch (index) {
      // mesure temperature
      case 0:
        value = this->bme.readTemperature();
        break;

      // mesure humidity
      case 1:
        value = this->bme.readHumidity();
        break;

      // mesure pressure
      case 2:
        value = this->bme.readPressure();
        break;

      // mesure altitude
      case 3:
        value = this->bme.readAltitude(1013);
        break;
    }
    this->setMesure(index, value);
    return this->mesuresDatas[index];
  }
  return this->read_(0);
}
void bme280::setMesure(int index, float value) {
  this->mesuresDatas[index] = value;
  this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
}
void bme280::loop() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    if (millis() - this->mesuresSampleLast[i] >
        (unsigned long)this->mesuresSampleRates[i]) {
      this->read_(i);
    }
  }
}

Sensor::SensorMesureData* bme280::read() { return this->mesuresDatas; }

Sensor::SensorMesureData bme280::read(int index) {
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData bme280::readBuffer(int index, int bufferIndex) {
  bufferIndex = bufferIndex > 40 ? 40 : bufferIndex < 0 ? 0 : bufferIndex;
  return this->mesuresBuffers[index][bufferIndex];
}

Sensor::SensorMesureData* bme280::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData bme280::average(int last, int index) {
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

String bme280::toString() {
  String out = this->name + "\n";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat("\t" + this->toString(i));
  }
  return out;
}
String bme280::toString(int index) {
  return this->mesures[index].name + ": " + String(this->read(index)) + " " +
         this->mesures[index].unit + "\n";
}
String bme280::toCsv() {
  String out = "";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toCsv(i));
  }
  return out;
}
String bme280::toCsv(int index) {
  return "\"" + this->name + "\", \"" + this->mesures[index].name + "\", \"" +
         this->read(index) + "\", \"" + this->mesures[index].unit + "\"\n";
}
String bme280::toJson() {
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
String bme280::toJson(int index) {
  return "\"" + this->mesures[index].name + "\":" + String(this->read(index));
}
String bme280::toXml() {
  String out = "<" + this->name + ">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toXml(i));
  }
  out.concat("</" + this->name + ">");
  return out;
}
String bme280::toXml(int index) {
  return "<" + this->mesures[index].name + " unit=\"" +
         this->mesures[index].unit + "\">" + String(this->read(index)) + "</" +
         this->mesures[index].name + ">";
}
String bme280::jsUtils() {
  return HtmlElt(
      "script",
      "const bme280_utils = {format_pressure: (val) => parseFloat(val) / "
      "100,inChart: (name) => name != '',};"
  );
}
String bme280::toHtml() {
  String out = "<div class=\"sensor " + this->name +
               "\"><div class=\"snName\">" + this->name +
               "</div><div class=\"snMss\">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toHtml(i));
  }
  out.concat("</div></div>" + this->jsUtils());
  return out;
}
String bme280::toHtml(int index) {
  return "<div class=\"snMs " + this->mesures[index].name +
         "\"><span class=\"snMs-name\">" + this->mesures[index].name +
         "</span> : <span class=\"snMs-value\">" + String(this->read(index)) +
         "</span><span class=\"snMs-unit\">" + this->mesures[index].unit +
         "</span></div>";
}
