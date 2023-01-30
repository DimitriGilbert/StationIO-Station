#include "./linky.h"

#include <Arduino.h>

#include "../html.h"

Linky::Linky(TInfo &lnk) : Sensor("linky") {
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
Linky::~Linky(){};

const Linky::SensorMesure Linky::mesures[4] = {
    {"hp", "W"}, {"hc", "W"}, {"tarif", ""}, {"power", "va"}, 
    // {"ADCO", "va"},
};
const u_int *Linky::mesuresSampleRates[4] = {
    (const u_int *)5000, (const u_int *)5000, (const u_int *)5000,
    (const u_int *)5000, 
    // (const u_int *)5000,
};
void Linky::begin() {
}
String Linky::getValue(String name) {
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

// void Linky::onSetup(StationClass station, int index) {}
String Linky::getName() { return this->name; }
String Linky::getMesureName(u_int index) { return this->mesures[index].name; }
Sensor::SensorMesure Linky::getMesure(u_int index) {
  return this->mesures[index];
}
size_t Linky::getMesuresCount() { return this->mesuresCount; }
Sensor::SensorMesureData *Linky::read_() {
  for (size_t i = 0; i < this->mesuresCount; i++) {
    this->mesuresDatas[i] = this->read_(i);
    this->mesuresBuffers[i].unshift(this->mesuresDatas[i]);
  }
  return this->mesuresDatas;
}
Sensor::SensorMesureData Linky::read_(int index) {
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
void Linky::setMesure(int index, float value) {
  this->mesuresDatas[index] = value;
  this->mesuresBuffers[index].unshift(this->mesuresDatas[index]);
}

void Linky::loop() {}

Sensor::SensorMesureData *Linky::read() { return this->mesuresDatas; }

Sensor::SensorMesureData Linky::read(int index) {
  return this->__read(index, this->mesuresCount, this->mesuresDatas);
}

Sensor::SensorMesureData Linky::readBuffer(int index, int bufferIndex) {
  bufferIndex = bufferIndex > 40 ? 40 : bufferIndex < 0 ? 0 : bufferIndex;
  return this->mesuresBuffers[index][bufferIndex];
}

Sensor::SensorMesureData *Linky::average(int last) {
  Sensor::SensorMesureData data[this->mesuresCount];
  for (size_t i = 0; i < this->mesuresCount; i++) {
    data[i] = this->average(last, i);
  }

  return data;
}

Sensor::SensorMesureData Linky::average(int last, int index) {
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

String Linky::toString() {
  return SensorToString(this);
}
String Linky::toString(int index) {
  return SensorMesureToString(this->getMesure(index), this->read(index));
}
String Linky::toCsv() {
  return SensorToCsv(this);
}
String Linky::toCsv(int index) {
  return SensorMesureToCsv(this->getMesure(index), this->read(index));
}
String Linky::toJson() {
  return SensorToJson(this);
}
String Linky::toJson(int index) {
  return SensorMesureToJson(this->getMesure(index), this->read(index));
}
String Linky::toXml() {
  return SensorToXml(this);
}
String Linky::toXml(int index) {
  return SensorMesureToXml(this->getMesure(index), this->read(index));
}
String Linky::jsUtils() {
  return HtmlElt(
      "script",
      "const linky_utils = "
      "{format_hp:val=>{linky_utils.conso.hp.push(val);return "
      "parseFloat(val)/1000;}"
      ",format_hc:val=>{linky_utils.conso.hc.push(val);return "
      "parseFloat(val)/1000;}"
      ",format_unit_hp:val=>' KWh'"
      ",format_unit_hc:val=>' KWh'"
      ",format_tarif:val=>parseInt(val)==1?'heures creuses':'heures pleines'"
      ",inChart:name=>name==='power'"
      ",conso:{hp:[], hc:[]}};setTimeout(()=>{let lscr="
      "document.createElement('script');lscr.src='/linky/js';"
      "document.head.appendChild(lscr);}, 500);"
  );
}
String Linky::toHtml() {
  return SensorToHtml(this);
}
String Linky::toHtml(int index) {
  return SensorMesureToHtml(this->getMesure(index), this->read(index));
}

String LinkyJsExtra() {
  return "const msecPerDay=864e5;function daysSinceDate(e,t){return Math.floor((e-t)/864e5)}function setCookie(e,t,n){let l;if(n){let e=new Date;e.setTime(e.getTime()+24*n*60*60*1e3),l=\"; expires=\"+e.toGMTString()}else l=\"\";document.cookie=e+\"=\"+t+l+\"; path=/\"}function getCookie(e,t){let n=document.cookie.match(new RegExp(e+\"=([^;]+)\")),l=n?n[1]:null;return null===l&&(l=t,setCookie(e,l,365)),l}function lnkFrmTpl(e,t,n){return'<form id=\"lnkF\"><div class=\"row\"><div class=\"form-group col\"><label for=\"lnkFHPO\">HP offset</label><input type=\"number\" class=\"form-control\" id=\"lnkFHPO\" name=\"lnkFHPO\" value=\"'+e+'\"></div><div class=\"form-group col\"><label for=\"lnkFHCO\">HC offset</label><input type=\"number\" class=\"form-control\" id=\"lnkFHCO\" name=\"lnkFHCO\" value=\"'+t+'\"></div><div class=\"form-group col\"><label for=\"lnkFDate\">period start</label><input type=\"date\" class=\"form-control\" id=\"lnkFDate\" name=\"lnkFDate\" value=\"'+n+'\"></div><div class=\"col\"><button class=\"btn btn-primary\" type=\"button\" id=\"updLnkBtn\">Update</button></div></div></form>'}setTimeout((()=>{linky_utils.addon={hp_offset:getCookie(\"lnkhpo\",0),hc_offset:getCookie(\"lnkhco\",0),date_offset:getCookie(\"lnkdo\",(new Date).getFullYear()+\"-01-01\")},dgeli(\"chart-control\").parentElement.innerHTML+=lnkFrmTpl(linky_utils.addon.hp_offset,linky_utils.addon.hc_offset,linky_utils.addon.date_offset),setTimeout((()=>{dgeli(\"updLnkBtn\").addEventListener(\"click\",(function(e){let t=new FormData(e.target.form);linky_utils.addon.hp_offset=t.get(\"lnkFHPO\"),linky_utils.addon.hc_offset=t.get(\"lnkFHCO\"),linky_utils.addon.date_offset=t.get(\"lnkFDate\")})),dqsa(\".sensor.linky>.snMss>.snMs.hp>.snMs-value\")[0].addEventListener(\"updated\",(function(e){let t=(parseFloat(e.target.innerHTML)-linky_utils.addon.hp_offset)/daysSinceDate(new Date,new Date(linky_utils.addon.date_offset));t=Math.round(100*t)/100,e.target.nextElementSibling.innerHTML+=\" (\"+t+\" KWh/day)\"})),dqsa(\".sensor.linky>.snMss>.snMs.hc>.snMs-value\")[0].addEventListener(\"updated\",(function(e){let t=(parseFloat(e.target.innerHTML)-linky_utils.addon.hc_offset)/daysSinceDate(new Date,new Date(linky_utils.addon.date_offset));t=Math.round(100*t)/100,e.target.nextElementSibling.innerHTML+=\" (\"+t+\" KWh/day)\"}))}),1e3)}),1e3);";
}
String LinkyJsonData(ValueList *me) {
  bool firstdata = true;
  String out = "";
  if (me) {
    out.concat("{");
    while (me->next) {
      me = me->next;
      if (!firstdata) {
        out.concat(", ");
      } else {
        firstdata = false;
      }

      out.concat("\"");
      out.concat(me->name);
      out.concat("\":");

      // we have at least something ?
      if (me->value && strlen(me->value)) {
        boolean isNumber = true;
        // uint8_t c;
        char *p = me->value;

        // check if value is number
        while (*p && isNumber) {
          if (*p < '0' || *p > '9') isNumber = false;
          p++;
        }

        // this will add "" on not number values
        if (!isNumber) {
          out.concat("\"");
          out.concat(me->value);
          out.concat("\"");
        }
        // this will remove leading zero on numbers
        else {
          out.concat(atol(me->value));
        }
      }
    }
    // Json end
    out.concat("}");
  }
  return out;
}
void LinkyRegisterEndpoints(EspStation &station_, char *login, char *password) {
  station_.addEndpoint(
      {"/linky/js", login, password,
       [](BaseStation *station, AsyncWebServerRequest *request) {
         request->send(200, "text/javascript", LinkyJsExtra());
       }}
  );
}

void LinkyOnData(BaseStation* station, ValueList* vallnk, uint8_t flag) {
#ifdef ESP8266
  digitalWrite(LED_BUILTIN_AUX, LOW);
#endif
  int index = -1;
  float value = -1;
  if (strcmp(vallnk->name, "PAPP") == 0) {
    index = 3;
    value = atof(vallnk->value);
#ifdef ESP8266
    digitalWrite(LED_BUILTIN, LOW);
#endif
  } else if (strcmp(vallnk->name, "HCHP") == 0) {
    index = 0;
    value = atof(vallnk->value);
#ifdef ESP8266
    digitalWrite(LED_BUILTIN, LOW);
#endif
  } else if (strcmp(vallnk->name, "HCHC") == 0) {
    index = 1;
    value = atof(vallnk->value);
#ifdef ESP8266
    digitalWrite(LED_BUILTIN, LOW);
#endif
  } else if (strcmp(vallnk->name, "PTEC") == 0) {
    index = 2;
    value = strcmp(vallnk->value, "HC..") == 0 ? 1 : 0;
#ifdef ESP8266
    digitalWrite(LED_BUILTIN, LOW);
#endif
  }
  if (index != -1) {
    station->getSensor("linky")->setMesure(index, value);
  }
#ifdef ESP8266
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_BUILTIN_AUX, HIGH);
#endif
}
