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
  // this->mesuresSampleLast[4] = (unsigned long)10000;
};
linky::~linky(){};

const linky::SensorMesure linky::mesures[4] = {
    {"hp", "W"}, {"hc", "W"}, {"tarif", ""}, {"power", "va"}, 
    // {"ADCO", "va"},
};
const u_int *linky::mesuresSampleRates[4] = {
    (const u_int *)5000, (const u_int *)5000, (const u_int *)5000,
    (const u_int *)5000, 
    // (const u_int *)5000,
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

// void linky::onSetup(StationClass station, int index) {}
String linky::getName() { return this->name; }
String linky::getMesureName(u_int index) { return this->mesures[index].name; }
Sensor::SensorMesure linky::getMesure(u_int index) {
  return this->mesures[index];
}
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

      // mesure ADC index power
      case 4:
        value = this->getValue("ADCO").toFloat();
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
  return SensorToString(this);
}
String linky::toString(int index) {
  return SensorMesureToString(this->getMesure(index), this->read(index));
}
String linky::toCsv() {
  return SensorToCsv(this);
}
String linky::toCsv(int index) {
  return SensorMesureToCsv(this->getMesure(index), this->read(index));
}
String linky::toJson() {
  return SensorToJson(this);
}
String linky::toJson(int index) {
  return SensorMesureToJson(this->getMesure(index), this->read(index));
}
String linky::toXml() {
  return SensorToXml(this);
}
String linky::toXml(int index) {
  return SensorMesureToXml(this->getMesure(index), this->read(index));
}
String linky::jsUtils() {
  return HtmlElt(
      "script",
      "const linky_utils = {format_hp:val=>parseFloat(val)/1000"
      ",format_hc:val=>parseFloat(val)/1000"
      ",format_unit_hp:val=>KW"
      ",format_unit_hc:val=>KW"
      ",format_tarif:val=>parseInt(val)==1?'heures creuses':'heures pleines'"
      ",inChart:name=>name==='power'};"
  );
}
String linky ::toHtml() {
  return SensorToHtml(this);
}
String linky ::toHtml(int index) {
  return SensorMesureToHtml(this->getMesure(index), this->read(index));
}
