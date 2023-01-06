#include <Arduino.h>
#include <ArduinoOTA.h>

#include "./Station.h"
#include "./html.h"

// including sensors
#include "./sensors/sht21.h"

// http client
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// initializing station
StationClass station(
    "midfan", {ssid : "mySSID", password : "myPass"}
);

String fanMode = "custom";
int fanSpeed = 100;
int pwmPin = D5;

// curves common
float minITemp = 20;
float maxITemp = 33;
float maxMTemp = 22.7;
// custom curve
float customTemps[5] = {21, 24.5, 26, 27, 28.5};
float customSpeeds[5] = {10, 33, 56, 85, 100};
String customMode = "linear";
String customRef = "midfan";

HTTPClient httpClient;
WiFiClient wifiClient;

void setFanSpeed(int speed) {
  fanSpeed = speed;
  analogWrite(pwmPin, fanSpeed);
  // station.logt(String(fanSpeed));
}
float getIntakeTemp() {
  // float itemp = getStationTemperature("192.168.1.10", "0", "0").toFloat();
  float itemp = station.getSensor(0)->read(0);
  return itemp;
}

bool safeCurve(float itemp) {
  int speed = fanSpeed;
  bool ret = true;
  if (itemp > maxITemp) {
    speed = 254;
    // station.log("intake high :"+String(itemp));
    ret = false;
  } else if (itemp < minITemp) {
    // because 256 < 0 duh !
    speed = 256;
    // station.log("intake low :" + String(itemp));
    ret = false;
  }
  if (!ret) {
    setFanSpeed(speed);
  }

  return ret;
}

bool safeCurve() { return safeCurve(getIntakeTemp()); }

void customCurve(BaseStation* station_) {
  if (fanMode == "custom") {
    // station.log("go custom");
    float rtemp = getIntakeTemp();
    if (safeCurve(rtemp)) {
      int speed = fanSpeed;
      float maxSpeed = 100;
      float tgSpeed = 0;
      float tempMin = customTemps[0];
      float tempMax = customTemps[4];


      for (size_t i = 0; i < 5; i++) {
        if (rtemp >= customTemps[i]) {
          tempMin = customTemps[i];
          tgSpeed = customSpeeds[i];
          maxSpeed = i == 4 ? 100 : customSpeeds[i + 1];
          tempMax = customTemps[i == 4 ? 4 : i + 1];
        }
      }

      if (customMode == "linear") {
        if (rtemp >= tempMax) {
          speed = maxSpeed * 2.54;
          // station.log("yeeeet !");
        } else {
          float maxd = (tempMax - tempMin);
          float dperc = (maxd - (tempMax - rtemp)) / maxd;
          float sperced = (tgSpeed + ((maxSpeed - tgSpeed) * dperc));
          speed = sperced * 2.54;
          // station.log("temp");
          // station.logt(String(tempMin));
          // station.logt(String(rtemp));
          // station.logt(String(tempMax));
          // station.log("speed");
          // station.logt(String(tgSpeed));
          // station.logt(String(maxSpeed));
          // station.log("%");
          // station.logt(String(dperc));
          // station.logt(String(sperced));
          // station.log(String(speed));
        }
      } else {
        speed = int(tgSpeed * 2.54);
      }
      setFanSpeed(speed);
    }
  }
}

// ___ sensors ___
Sensor* sensors[1] = {
    new sht21(),
};

// ___ timers ___
BaseStation::StationCallbackTimer_t timers[1] = {{customCurve, 10500}};

void setup() {
  station.setup();

  // setting up sensors
  station.setupSensors(sensors, 1);

  // setting up timers
  station.setupTimers(timers, 1);

  // setting up OTA
  station.setupOTA();

  // station.setupWebServer(srvReqs, 1);
  station.webServer.on("/fanspeed", [](AsyncWebServerRequest* request) {
    int speed = fanSpeed;
    if (request->hasParam("mode")) {
      fanMode = request->getParam("mode")->value();
    }
    if (request->hasParam("minITemp")) {
      minITemp = request->getParam("minITemp")->value().toFloat();
    }
    if (request->hasParam("maxITemp")) {
      maxITemp = request->getParam("maxITemp")->value().toFloat();
    }
    if (request->hasParam("maxMTemp")) {
      maxMTemp = request->getParam("maxMTemp")->value().toFloat();
    }
    if (request->hasParam("cTs")) {
      String customPrmStr = request->getParam("cTs")->value();
      if (customPrmStr != "") {
        bool customDone = false;
        int customIndex = 0;
        while (!customDone) {
          int csi = customPrmStr.indexOf(" ");
          if (csi != -1) {
            customTemps[customIndex] = customPrmStr.substring(0, csi).toFloat();
            customPrmStr = customPrmStr.substring(csi + 1);
          } else {
            customDone = true;
          }

          if (customIndex >= 4) {
            customTemps[4] = customPrmStr.toFloat();
            customDone = true;
          } else {
            customIndex++;
          }
        }
      }
    }
    if (request->hasParam("cSs")) {
      String customPrmStr = request->getParam("cSs")->value();
      if (customPrmStr != "") {
        bool customDone = false;
        int customIndex = 0;
        while (!customDone) {
          int csi = customPrmStr.indexOf(" ");
          if (csi != -1) {
            customSpeeds[customIndex] =
                customPrmStr.substring(0, csi).toFloat();
            customPrmStr = customPrmStr.substring(csi + 1);
          } else {
            customDone = true;
          }

          if (customIndex >= 4) {
            customSpeeds[4] = customPrmStr.toFloat();
            customDone = true;
          } else {
            customIndex++;
          }
        }
      }
    }
    if (request->hasParam("speed") && fanMode == "manual") {
      if (safeCurve()) {
        speed = request->getParam("speed")->value().toInt();
        if (speed > 255) {
          speed = 255;
        } else if (speed < 1) {
          speed = 1;
        }

        setFanSpeed(speed);
      }
    }
    if (request->hasParam("cM")) {
      if (request->getParam("cM")->value() == "linear") {
        customMode = "linear";
      } else {
        customMode = "stepped";
      }
    }

    float percspeed = float(fanSpeed) / 255 * 100;
    String customTStr = "";
    String customSStr = "";
    for (size_t i = 0; i < 5; i++) {
      if (i > 0) {
        customTStr += " ";
        customSStr += " ";
      }

      customTStr += String(customTemps[i]);
      customSStr += String(customSpeeds[i]);
    }

    // #region
    String data =
        commonHtmlHeader(station.name + ": Fans control") +
        commonBody(
            station.toHtml() +
            "<div class=\"pure-g\"><div class=\"pure-u-1-3\"></div><div "
            "class=\"pure-u-1-3\"><form "
            "class=\"pure-form "
            "pure-form-stacked\"method=GET><fieldset><legend>Current "
            "speed is " +
            String(percspeed) +
            "%</legend>"
            "<label for=\"mode\">Mode</label>"
            "<select name=\"mode\" id=\"mode\" "
            "onchange=\"document.querySelectorAll('*[target-mode]').forEach(el="
            ">{el.setAttribute('hidden', "
            "true);el.lastElementChild.setAttribute('disabled', "
            "true)});document.querySelectorAll('*[target-mode='+this.value+']')"
            ".forEach(el=>{el.removeAttribute('hidden');el.lastElementChild."
            "removeAttribute('"
            "disabled')})\"><option "
            "value=\"manual\"" +
            (fanMode == "manual" ? " selected" : "") +
            ">manual</option><option "
            "value=\"custom\"" +
            (fanMode == "custom" ? " selected" : "") +
            ">custom</option></select>"
            "<label for=\"speed\">" +
            String(percspeed) +
            "</label><input type=\"range\" "
            "min=1 max=254 value=\"" +
            String(fanSpeed) +
            "\" "
            "name=\"speed\" id=\"speed\" "
            "onchange=\"this.previousSibling.innerHTML=(this.value/254)*100;\" "
            "/><div id=\"advanced\" hidden>"
            "<div><label for=\"minITemp\">Min "
            "intake temperature</label><input type=\"number\" step=\"0.1\" "
            "name=\"minITemp\" id=\"minITemp\" value=\"" +
            minITemp +
            "\"/></div>"
            "<div><label for=\"maxITemp\">Max "
            "intake temperature</label><input type=\"number\" step=\"0.1\" "
            "name=\"maxITemp\" id=\"maxITemp\" value=\"" +
            maxITemp +
            "\"/></div>"
            "<div target-mode=\"winter\" hidden ><label "
            "for=\"maxMTemp\">Max "
            "Main temperature</label><input type=\"number\" step=\"0.1\" "
            "disabled name=\"maxMTemp\" id=\"maxMTemp\" value=\"" +
            maxMTemp +
            "\"/></div>"
            "<div target-mode=\"custom\" hidden ><label "
            "for=\"cTs\">temperature targets, 5, space "
            "separated</label><input type=\"text\" "
            "disabled name=\"cTs\" id=\"cTs\" value=\"" +
            customTStr +
            "\"/></div>"
            "<div target-mode=\"custom\" hidden ><label "
            "for=\"cSs\">Speed targets, 5, space "
            "separated</label><input type=\"text\" "
            "disabled name=\"cSs\" id=\"cSs\" value=\"" +
            customSStr +
            "\"/></div>"
            "<div target-mode=\"custom\" hidden ><label "
            "for=\"cM\">custom mode</label><select name=\"cM\" "
            "id=\"cM\" disabled><option "
            "value=\"linear\">linear</option><option value=\"stepped\"" +
            (customMode != "linear" ? " selected" : "") +
            ">stepped</option></select></div>"
            "</div></fieldset><button type=\"button\" class=\"pure-button\" "
            "onclick=\"let "
            "adv=document.getElementById('advanced');adv.hasAttribute('hidden')"
            "?adv.removeAttribute('hidden'):adv.setAttribute('hidden', "
            "true);document.getElementById('mode').onchange()\">"
            "Advanced</"
            "button><button "
            "type=\"submit\" class=\"pure-button "
            "pure-button-primary\">Set</button></form></div></div>"
        ) +
        "</html>";
    // #endregion
    request->send(200, "text/html", data);
  });

  // ___ custom code goes here ___

  // setting up fan control
  pinMode(pwmPin, OUTPUT);
  // analogWrite(pwmPin, fanSpeed);
  setFanSpeed(fanSpeed);
}

void loop() {
  station.loop();
  // ___ custom code goes here ___
}