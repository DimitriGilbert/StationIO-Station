#include "./Station.h"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <Wire.h>

#include "./html.h"

// Base Station
BaseStation::BaseStation(String name) {
  this->status = BaseStation::StatusStarting;
  this->sensorCount = 0;
  this->name = name;
  // this->serial = Serial;
  this->stationTypeName = "Arduino";
  this->log("starting...");
}

BaseStation::~BaseStation() {}
void BaseStation::setSerial(HardwareSerial sr) { this->serial = sr; }
void BaseStation::log(const String& data) { this->serial.println(data); }
void BaseStation::logt(const String& data) {
  this->serial.println("\t" + data);
}

void BaseStation::setup() {
  this->serial.begin(9600);
  Wire.begin();
  this->log("\nWelcome to StationIO");
  this->logt("on " + this->stationTypeName);
  this->logt("station " + this->name);
  this->status = BaseStation::StatusReady;
}

void BaseStation::setupSensors(Sensor** sensors, size_t sensorCount) {
  this->sensors = sensors;
  this->sensorCount = sensorCount;
  this->log("Sensors : " + String(this->sensorCount));
  for (size_t i = 0; i < this->sensorCount; i++) {
    this->logt(String(i) + " -> " + this->sensors[i]->name);
  }
}

void BaseStation::setupLoopCallback(
    StationCallback_t* loopCallbacks, int loopCallbackCount
) {
  this->loopCallbacks = loopCallbacks;
  this->loopCallbackCount = loopCallbackCount;
  this->log("Loop calback :" + String(loopCallbackCount));
}

void BaseStation::setupTimerCallback(
    StationCallbackTimer_t* timerCallbacks, int timerCallbackCount
) {
  this->setupTimerCallback(timerCallbacks, timerCallbackCount, 0);
}

void BaseStation::setupTimerCallback(
    StationCallbackTimer_t* timerCallbacks, int timerCallbackCount, int delay
) {
  this->timerCallbacks = timerCallbacks;
  this->timerCallbackCount = timerCallbackCount;
  this->log("Timer callbacks :" + String(timerCallbackCount));
  for (size_t i = 0; i < timerCallbackCount; i++) {
    this->logt(
        String(i) + " -> every " + String(this->timerCallbacks[i].interval) +
        " in " + String(this->timerCallbacks[i].next)
    );
  }
}

void BaseStation::setupOTA() {
  ArduinoOTA.onStart([this]() {
    this->log("Starting OTA update on :");
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      this->logt("sketch");
    }
    // U_SPIFFS
    else {
      this->logt("filesystem");
      SPIFFS.end();
      this->logt("\tfs unmounted");
    }
  });
  ArduinoOTA.onEnd([this]() { this->log("OTA update ended"); });
  ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
    this->logt(
        "\tOTA update progress: " + String(progress) + "/" + String(total / 100)
    );
    // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

void BaseStation::setup(Sensor** sensors, int sensorCount) {
  this->setup();
  this->setupSensors(sensors, sensorCount);
}

void BaseStation::setup(
    StationCallback_t* loopCallbacks, int loopCallbackCount
) {
  this->setup();
  this->setupLoopCallback(loopCallbacks, loopCallbackCount);
}

void BaseStation::setup(
    StationCallbackTimer_t* timerCallbacks, int timerCallbackCount
) {
  this->setup();
  this->setupTimerCallback(timerCallbacks, timerCallbackCount);
}

bool BaseStation::ready() { return this->ready(BaseStation::StatusReady); }

bool BaseStation::ready(int minStatus) { return this->status >= minStatus; }

void BaseStation::loop() {
  ArduinoOTA.handle();

  for (size_t i = 0; i < this->sensorCount; i++) {
    this->sensors[i]->loop();
  }

  for (size_t i = 0; i < this->loopCallbackCount; i++) {
    this->loopCallbacks[i](this);
  }

  for (size_t i = 0; i < this->timerCallbackCount; i++) {
    u_int ti = millis();
    if (ti >= this->timerCallbacks[i].next) {
      this->timerCallbacks[i].callback(this);
      this->timerCallbacks[i].next = ti + this->timerCallbacks[i].interval;
    }
  }
}

I2CScan BaseStation::scanI2C() {
  byte error, address;
  int nDevices;
  nDevices = 0;
  String devices = "";
  String errors = "";
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      devices.concat("0x");
      if (address < 16) devices.concat("0");
      devices.concat(String(address, HEX));
      devices.concat("\n");
      nDevices++;
    } else if (error == 4) {
      errors.concat("error @ 0x");
      if (address < 16) errors.concat("0");
      errors.concat(String(address, HEX));
    }
  }
  return {
      nDevices,
      devices,
      errors,
  };
}

String BaseStation::toString() {
  String out = "";
  for (size_t i = 0; i < this->sensorCount; i++) {
    out.concat(this->toString(i));
  }
  return out;
}
String BaseStation::toString(int index) {
  return this->getSensor(index)->toString();
}
String BaseStation::toCsv() {
  String out = "";
  for (size_t i = 0; i < this->sensorCount; i++) {
    out.concat(this->toCsv(i));
  }
  return out;
}
String BaseStation::toCsv(int index) { return this->getSensor(index)->toCsv(); }
String BaseStation::toJson() {
  String out = "{\"name\":\"" + this->name + "\",\"sensors\":{";
  for (size_t i = 0; i < this->sensorCount; i++) {
    if (i > 0) {
      out.concat(",");
    }

    out.concat(this->toJson(i));
  }
  out.concat("}}");
  return out;
}
String BaseStation::toJson(int index) {
  return this->getSensor(index)->toJson();
}
String BaseStation::toXml() {
  String out = "";
  for (size_t i = 0; i < this->sensorCount; i++) {
    out.concat(this->toXml(i));
  }
  return out;
}
String BaseStation::toXml(int index) { return this->getSensor(index)->toXml(); }
String BaseStation::toHtml() {
  String out = HtmlElt(
      "h1",
      "Welcome to Station " +
          HtmlElt("span", this->name, HtmlClass("station-name")),
      HtmlClass("welcome")
  );
  for (size_t i = 0; i < this->sensorCount; i++) {
    out.concat(this->toHtml(i));
  }
  return out;
}
String BaseStation::toHtml(int index) {
  return HtmlDiv(
      this->getSensor(index)->toHtml(),
      HtmlClass("stSn") + HtmlAttribute("sn-i", String(index), false) +
          HtmlId("sensor-" + String(index))
  );
}
Sensor* BaseStation::getSensor(int index) {
  if (index >= this->sensorCount) {
    index = this->sensorCount - 1;
  }

  return this->sensors[index];
}
String BaseStation::getSensorName(int index) {
  return this->getSensor(index)->name;
}

// ESP Staion
EspStation::EspStation(String name) : BaseStation(name), webServer(80) {
  this->stationTypeName = "ESP";
  this->setWifiInformation({ssid : "", password : ""});
}
EspStation::EspStation(String name, WifiInformation wifiInformation)
    : BaseStation(name), webServer(80) {
  this->stationTypeName = "ESP";
  this->setWifiInformation(wifiInformation);
}
EspStation::~EspStation() {}

void EspStation::setWifiInformation(WifiInformation wifiInformation) {
  this->wifiInformation = wifiInformation;
  this->log("set Wifi credentials");
}

void EspStation::setup() {
  BaseStation::setup();
  this->connect();
  this->serve();
}

void EspStation::setup(Sensor** sensors, int sensorCount) {
  BaseStation::setup(sensors, sensorCount);
  this->connect();
  this->serve();
}

void EspStation::connect() { this->connectWifi(); }

void EspStation::initWebServer() {
  this->log("Web Server :");
  this->webServer.onNotFound([](AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not found");
  });
  this->webServer.on("/", [this](AsyncWebServerRequest* request) {
    String data;
    AsyncWebHeader* hd = request->getHeader("Accept");
    const char* accv;
    if (request->hasParam("format")) {
      accv = request->getParam("format")->value().c_str();
    } else {
      accv = hd->value().c_str();
    }

    if (request->hasParam("sensor")) {
      int sid = request->getParam("sensor")->value().toInt();
      if (sid < this->sensorCount) {
        if (request->hasParam("mesure")) {
          int mesureid = request->getParam("mesure")->value().toInt();
          if (strcmp(accv, "application/json") == 0) {
            data = this->sensors[sid]->toJson(mesureid);
          } else if (strcmp(accv, "application/xml") == 0) {
            data = this->sensors[sid]->toXml(mesureid);
          } else if (strcmp(accv, "text/csv") == 0) {
            data = this->sensors[sid]->toCsv(mesureid);
          } else if (strcmp(accv, "text/plain") == 0) {
            data = this->sensors[sid]->toString(mesureid);
          } else if (strcmp(accv, "text/raw") == 0) {
            data = String(this->sensors[sid]->read(mesureid));
          } else {
            data = this->sensors[sid]->toHtml(mesureid);
            accv = "text/html";
          }
        } else {
          if (strcmp(accv, "application/json") == 0) {
            data = this->sensors[sid]->toJson();
          } else if (strcmp(accv, "application/xml") == 0) {
            data = this->sensors[sid]->toXml();
          } else if (strcmp(accv, "text/csv") == 0) {
            data = this->sensors[sid]->toCsv();
          } else if (strcmp(accv, "text/plain") == 0) {
            data = this->sensors[sid]->toString();
          } else if (strcmp(accv, "raw") == 0) {
            size_t tdataSize = this->sensors[sid]->getMesuresCount();
            for (size_t i = 0; i < tdataSize; i++) {
              data.concat(String(this->sensors[sid]->read(i)));
              if (i < tdataSize - 1) {
                data.concat("\n");
              }
            }
          } else {
            data = this->sensors[sid]->toHtml();
            accv = "text/html";
          }
        }
      }
    } else {
      if (strcmp(accv, "application/json") == 0) {
        data = this->toJson();
      } else if (strcmp(accv, "application/xml") == 0) {
        data = this->toXml();
      } else if (strcmp(accv, "text/csv") == 0) {
        data = this->toCsv();
      } else if (strcmp(accv, "text/plain") == 0) {
        data = this->toString();
      } else {
        data = this->toHtml();
        accv = "text/html";
      }
    }

    request->send(200, accv, data);
  });
  this->webServer.on("/average", [this](AsyncWebServerRequest* request) {
    String data;
    if (!request->hasParam("sensor")) {
      request->send(400, "text/plain", "Bad Request, missing sensor id");
    } else if (!request->hasParam("mesure")) {
      request->send(400, "text/plain", "Bad Request, missing mesure id");
    } else if (!request->hasParam("count")) {
      request->send(400, "text/plain", "Bad Request, missing count");
    } else {
      data = String(
          this->sensors[request->getParam("sensor")->value().toInt()]->average(
              request->getParam("count")->value().toInt(),
              request->getParam("mesure")->value().toInt()
          )
      );
      request->send(200, "text/plain", data);
    }
  });
  this->webServer.on("/buffer", [this](AsyncWebServerRequest* request) {
    String data;
    if (!request->hasParam("sensor")) {
      request->send(400, "text/plain", "Bad Request, missing sensor id");
    } else if (!request->hasParam("mesure")) {
      request->send(400, "text/plain", "Bad Request, missing mesure id");
    } else if (!request->hasParam("count")) {
      request->send(400, "text/plain", "Bad Request, missing count");
    } else {
      int msi = request->getParam("mesure")->value().toInt();
      int cnt = request->getParam("count")->value().toInt();
      Sensor* sn =
          this->getSensor(request->getParam("sensor")->value().toInt());
      // String msn = sn->getMesureName(msi);
      String msn = "mesure";

      String accv = "text/plain";
      if (request->hasParam("format")) {
        accv = request->getParam("format")->value();
      } else {
        accv = request->getHeader("Accept")->value();
      }

      if (accv == "application/json") {
        data.concat("[");
      } else if (accv == "application/xml") {
        data.concat("<buffer>");
      }
      
      for (int i = 0; i < cnt; i++) {
        if (accv == "application/json") {
          data.concat(String(sn->readBuffer(msi, i)));
          if (i < cnt - 1) {
            data.concat(",");
          }
        } else if (accv == "application/xml") {
          data.concat(
              "<" + msn + ">" + String(sn->readBuffer(msi, i)) + "</" + msn +
              ">"
          );
        } else {
          data.concat(String(sn->readBuffer(msi, i)) + "\n");
        }
      }
      if (accv == "application/json") {
        data.concat("]");
      } else if (accv == "application/xml") {
        data.concat("</buffer>");
      }

      request->send(200, accv, data);
    }
  });
  this->webServer.on("/fancy", [this](AsyncWebServerRequest* request) {
    String data = commonHtmlHeader(this->name + " : recap</title>") +
                  commonBody(this->toHtml()) + "</html>";
    request->send(200, "text/html", data);
  });
  this->webServer.on("/js", [this](AsyncWebServerRequest* request) {
    request->send(200, "application/javascript", commonJs());
  });
  this->webServer.on("/css", [this](AsyncWebServerRequest* request) {
    request->send(200, "text/css", commonCss());
  });
}

void EspStation::setupWebServer(
    StationWebCallbackInfo_t** routes, int callbackCount
) {
  if (this->wifi.status() == WL_CONNECTED) {
    this->initWebServer();
    for (size_t i = 0; i < callbackCount; i++) {
      this->webServer.on(
          routes[i]->route,
          [this, routes, i](AsyncWebServerRequest* request) {
            if (strcmp(routes[i]->login, "") != 0) {
              if (!request->authenticate(
                      routes[i]->login, routes[i]->password
                  )) {
                this->log(
                    "Web Server : \n\tAuthentication failed on " +
                    String(routes[i]->route)
                );
                request->requestAuthentication();
              }
            }
            routes[i]->callback(this, request);
          }
      );
    }
  }
}

void EspStation::connectWifi(WifiInformation wifiInformation) {
  this->setWifiInformation(wifiInformation);
  this->connectWifi();
}

void EspStation::connectWifi() {
  if (strlen(this->wifiInformation.ssid) > 0 &&
      strlen(this->wifiInformation.password) > 0) {
    if (this->ready(EspStation::StatusConnecting) &&
        this->wifi.status() != WL_CONNECTED) {
      this->log("Wifi :");
      this->logt("connecting to " + String(this->wifiInformation.ssid));
      this->wifi.begin(
          this->wifiInformation.ssid, this->wifiInformation.password
      );
      if (this->wifi.waitForConnectResult() != WL_CONNECTED) {
        this->status = EspStation::StatusError;
        this->error = EspStation::ErrorWifiConnection;
        this->logt(
            "connection failed ! error : " + String(this->wifi.status())
        );
      } else {
        this->status = EspStation::StatusConnected;
        this->logt("connected as " + String(this->wifi.localIP().toString()));
      }
    }
  } else {
    this->status = EspStation::StatusReady;
  }
}

String EspStation::scanWifi() {
  String out = "";
  if (this->wifi.status() != WL_CONNECTED) {
    byte available_networks = this->wifi.scanNetworks();

    for (int network = 0; network < available_networks; network++) {
      out.concat(this->wifi.SSID(network));
      out.concat(" : ");
      out.concat(this->wifi.RSSI(network));
      out.concat("\n");
    }
  }
  return out;
}

void EspStation::serve() { this->serveWeb(); }

void EspStation::serveWeb() {
  // if (this->ready(EspStation::StatusReady) &&
  if (this->wifi.status() == WL_CONNECTED &&
      strlen(this->wifiInformation.ssid) > 0 &&
      strlen(this->wifiInformation.password) > 0) {
    this->initWebServer();
    this->webServer.begin();
  }
}
#ifdef ESP32
// ESP32
Esp32Station::Esp32Station(String name) : EspStation(name) {
  this->stationTypeName = "ESP32";
}
Esp32Station::Esp32Station(String name, WifiInformation wifiInformation)
    : EspStation(name, wifiInformation) {}
Esp32Station::~Esp32Station() {}
#elif defined(ESP8266)
// ESP8266
Esp8266Station::Esp8266Station(String name) : EspStation(name) {
  this->stationTypeName = "ESP8266";
}
Esp8266Station::Esp8266Station(String name, WifiInformation wifiInformation)
    : EspStation(name, wifiInformation) {}
Esp8266Station::~Esp8266Station() {}
#endif