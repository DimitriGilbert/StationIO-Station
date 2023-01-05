#include "./linky.h"

#include <Arduino.h>

#include "./html.h"

linky::linky(TInfo &lnk) : Sensor("linky") {
  // Serial.begin(1200, SERIAL_7E1);
  // Serial.swap();
  this->lnk = lnk;
  // this->lnk.init();
  // this->lnk.attachData(fn_data);
  this->mesuresSampleLast[0] = (unsigned long)10000;
  this->mesuresSampleLast[1] = (unsigned long)10000;
  this->mesuresSampleLast[2] = (unsigned long)10000;
  this->mesuresSampleLast[3] = (unsigned long)10000;
};
linky::~linky(){};

const linky::SensorMesure linky::mesures[4] = {
    {"hp", "w"},
    {"hc", "W"},
    {"tarif", ""},
    {"power", "va"},
};
const u_int *linky::mesuresSampleRates[4] = {
    (const u_int *)5000,
    (const u_int *)5000,
    (const u_int *)5000,
    (const u_int *)5000,
};

String linky::getValue(String name) {
  const char *tgt = name.c_str();
  int tgtl = strlen(tgt);
  ValueList *me = this->lnk.getList();
  String data = "does not exists";
  while (me->next) {
    // go to next node
    me = me->next;

    // Check if we match this LABEL
    if (tgtl == strlen(me->name) && strcmp(me->name, tgt) == 0) {
      // this one has a value ?
      if (me->value) {
        data = String(me->value);
      }
    }
  }
  return data;
}

void linky::onSetup(StationClass station, int index) {}
size_t linky::getMesuresCount() { return this->mesuresCount; }
Sensor::SensorMesureData *linky::read_() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
    this->mesuresBuffers[i].unshift(this->mesuresDatas[i]);
  }
  return this->mesuresDatas;
}
Sensor::SensorMesureData linky::read_(int index) {
  if (index < this->mesuresCount) {
    Sensor::SensorMesureData value = 0;
    this->mesuresSampleLast[index] = millis();
    switch (index) {
      // mesure hp
      case 0:
        value = this->getValue("HCHP").toFloat();
        break;

      // mesure hc
      case 1:
        value = this->getValue("HCHC").toFloat();
        break;

      // mesure tarif
      case 2:
        value = this->getValue("PTEC") == "HC.." ? 1 : 57;
        break;

      // mesure power
      case 3:
        value = this->getValue("PAPP").toFloat();
        break;
    }
    this->setMesure(index, value);
    return this->mesuresDatas[index];
  }
  return this->read_(0);
}
void linky::setMesure(int index, float value) {
  this->mesuresDatas[index] = value;
  this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
}

void linky::loop() {}

Sensor::SensorMesureData *linky::read() { return this->mesuresDatas; }

Sensor::SensorMesureData linky::read(int index) {
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData linky::readBuffer(int index, int bufferIndex) {
  bufferIndex = bufferIndex > 40 ? 40 : bufferIndex < 0 ? 0 : bufferIndex;
  return this->mesuresBuffers[index][bufferIndex];
}

Sensor::SensorMesureData *linky::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData linky::average(int last, int index) {
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

String linky::toString() {
  String out = this->name + "\n";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat("\t" + this->toString(i));
  }
  return out;
}
String linky::toString(int index) {
  return this->mesures[index].name + ": " + String(this->read(index)) + " " +
         this->mesures[index].unit + "\n";
}
String linky::toCsv() {
  String out = "";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toCsv(i));
  }
  return out;
}
String linky::toCsv(int index) {
  return "\"" + this->name + "\", \"" + this->mesures[index].name + "\", \"" +
         this->read(index) + "\", \"" + this->mesures[index].unit + "\"\n";
}
String linky::toJson() {
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
String linky::toJson(int index) {
  return "\"" + this->mesures[index].name + "\":" + String(this->read(index));
}
String linky::toXml() {
  String out = "<" + this->name + ">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toXml(i));
  }
  out.concat("</" + this->name + ">");
  return out;
}
String linky::toXml(int index) {
  return "<" + this->mesures[index].name + " unit=\"" +
         this->mesures[index].unit + "\">" + String(this->read(index)) + "</" +
         this->mesures[index].name + ">";
}
String linky::jsUtils() {
  return HtmlElt(
      "script",
      "const linky_utils = {format_hp: (val) => parseFloat(val) / "
      "1000,format_hc: (val) => parseFloat(val) / 1000,format_tarif: (val) => "
      "(val == 'HC..' ? 'heures creuses' : 'heures pleines'),inChart: (name) "
      "=> "
      "name == 'power',};","",false
  );
}
String linky ::toHtml() {
  String out = "<div class=\"sensor " + this->name +
               "\"><div class=\"snName\">" + this->name +
               "</div><div class=\"snMss\">";
  for (size_t i = 0; i < this->mesuresCount; i++) {
    out.concat(this->toHtml(i));
  }
  out.concat("</div></div>" + this->jsUtils());
  return out;
}
String linky ::toHtml(int index) {
  return "<div class=\"snMs " + this->mesures[index].name +
         "\"><span class=\"snMs-name\">" + this->mesures[index].name +
         "</span> : <span class=\"snMs-value\">" +
         String(this->read(index)) +
         "</span><span class=\"snMs-unit\">" +
         this->mesures[index].unit + "</span></div>";
}
