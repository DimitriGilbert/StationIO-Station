#include <Arduino.h>
#include <LibTeleinfo.h>

#include "./Station.h"
#include "./sensors/linky.h"
#include "./sensors/sht21.h"

bool prout = LOW;

// initializing station
StationClass station(
    "linky", {ssid : "MySSID", password : "MyPassword"}
);
// ___ sensors ___

void onLinkyData(ValueList* vallnk, uint8_t flag) {
  // data in, light on
  digitalWrite(LED_BUILTIN_AUX, LOW);
  int index = -1;
  float value = -1;
  // interesting data point ?
  if (strcmp(vallnk->name, "PAPP") == 0) {
    index = 3;
    value = atof(vallnk->value);
    digitalWrite(LED_BUILTIN, LOW);
  } else if (strcmp(vallnk->name, "HCHP") == 0) {
    index = 0;
    value = atof(vallnk->value);
    digitalWrite(LED_BUILTIN, LOW);
  } else if (strcmp(vallnk->name, "HCHC") == 0) {
    index = 1;
    value = atof(vallnk->value);
    digitalWrite(LED_BUILTIN, LOW);
  } else if (strcmp(vallnk->name, "PTEC") == 0) {
    index = 2;
    value = strcmp(vallnk->value, "HC..") == 0 ? 1 : 0;
    digitalWrite(LED_BUILTIN, LOW);
  }
  if (index != -1) {
    // setting datapoint in station sensor
    station.sensors[0]->setMesure(index, value);
  }
  // lights off
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_BUILTIN_AUX, HIGH);
}
// linky Teleinfo initialisation
TInfo tinfo;

// ___ sensors ___
Sensor* sensors[2] = {new linky(tinfo), new sht21()};

// Code taken from libteleinfo example for custom endpoint
String sendJSON(ValueList* me) {
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
        char* p = me->value;

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

void setup() {
  // builtin leds as outputs
  pinMode(LED_BUILTIN_AUX, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  // light on
  digitalWrite(LED_BUILTIN_AUX, LOW);

  // setting station serial output
  station.setSerial(Serial1);
  station.setup();
  station.setupOTA();

  // setting up linky serial
  Serial.begin(1200, SERIAL_7E1);
  // swapping serial pin
  Serial.swap();

  // // Init teleinfo
  tinfo.init();
  // attaching data callback
  tinfo.attachData(onLinkyData);

  // setting up sensors
  station.setupSensors(sensors, 2);

  // custom endpoint with complete linky datapoints
  station.webServer.on("/lky", [](AsyncWebServerRequest* request) {
    request->send(200, "application/json", sendJSON(tinfo.getList()));
  });
  // light off
  digitalWrite(LED_BUILTIN_AUX, HIGH);
}

void loop() {
  station.loop();
  // reading serial
  if (Serial.available()) {
    // reading char, if done in one line, it does not work, sometimes, maybe or not.... ask me how I know... :'(
    char c = Serial.read();
    // sending char to teleinfo
    tinfo.process(c);
  }
}