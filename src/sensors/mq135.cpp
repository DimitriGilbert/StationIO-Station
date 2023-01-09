#include "./mq135.h"

#include <Arduino.h>

#include "./html.h"

mq135::mq135(int pin) : Sensor("mq135"), mq(5, 10, pin) {
  this->mesuresSampleLast[0] = (unsigned long)1000;

  this->mesuresSampleLast[1] = (unsigned long)1000;

  this->mesuresSampleLast[2] = (unsigned long)1000;

  this->mesuresSampleLast[3] = (unsigned long)1000;

  this->mesuresSampleLast[4] = (unsigned long)1000;

  this->mesuresSampleLast[5] = (unsigned long)1000;

  this->mesuresSampleLast[6] = (unsigned long)1000;
};
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
  String out = this->name + "\n";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat("\t" + this->toString(i));
  }
  return out;
}
String mq135::toString(int index) {
  return this->mesures[index].name + ": " + String(this->read(index)) + " " +
         this->mesures[index].unit + "\n";
}
String mq135::toCsv() {
  String out = "";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toCsv(i));
  }
  return out;
}
String mq135::toCsv(int index) {
  return "\"" + this->name + "\", \"" + this->mesures[index].name + "\", \"" +
         this->read(index) + "\", \"" + this->mesures[index].unit + "\"\n";
}
String mq135::toJson() {
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
String mq135::toJson(int index) {
  return "\"" + this->mesures[index].name + "\":" + String(this->read(index));
}
String mq135::toXml() {
  String out = "<" + this->name + ">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toXml(i));
  }
  out.concat("</" + this->name + ">");
  return out;
}
String mq135::toXml(int index) {
  return "<" + this->mesures[index].name + " unit=\"" +
         this->mesures[index].unit + "\">" + String(this->read(index)) + "</" +
         this->mesures[index].name + ">";
}
String mq135::jsUtils() {
  return HtmlElt("script", "const mq135_utils={inChart:name=>name==='raw'};");
}
String mq135::toHtml() {
  String out = "<div class=\"text-center sensor " + this->name +
               "\"><div class=\"snName\">" + this->name +
               "</div><div class=\"snMss row\">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toHtml(i));
  }
  return out + "</div></div>" + this->jsUtils();
}
String mq135::toHtml(int index) {
  // return "<div class=\"snMs " + this->mesures[index].name +
  //        "\"><span class=\"snMs-name\">" + this->mesures[index].name +
  //        "</span> : <span class=\"snMs-value\">" +
  //        String(this->read(index)) +
  //        "</span><span class=\"snMs-unit\">" +
  //        this->mesures[index].unit + "</span></div>";
  return HtmlDiv(
      HtmlElt("span", this->mesures[index].name, HtmlClass("snMs-name")) +
          " : " +
          HtmlElt("span", String(this->read(index)), HtmlClass("snMs-value")) +
          HtmlElt("span", this->mesures[index].unit, HtmlClass("snMs-unit")),
      HtmlClass("col snMs " + this->mesures[index].name)
  );
}