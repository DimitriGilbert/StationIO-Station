#include "./Station.h"

#include <Arduino.h>
#include <Wire.h>

// Base Station
BaseStation::BaseStation(String name) {
  this->status = BaseStation::StatusStarting;
  this->sensorCount = 0;
  this->name = name;
  this->stationTypeName = "Arduino";
  this->log("starting...");
}
BaseStation::~BaseStation() {}

void BaseStation::log(const String& data) {
  Serial.println(data);
}
void BaseStation::logt(const String& data) {
  Serial.println("\t" + data);
}

void BaseStation::setup() {
  Serial.begin(9600);
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

void BaseStation::setupLoopCallback(StationCallback_t* loopCallbacks,
                                    int loopCallbackCount) {
  this->loopCallbacks = loopCallbacks;
  this->loopCallbackCount = loopCallbackCount;
  this->log("Loop calback :" + String(loopCallbackCount));
}

void BaseStation::setupTimerCallback(StationCallbackTimer_t* timerCallbacks,
                                     int timerCallbackCount) {
  this->setupTimerCallback(timerCallbacks, timerCallbackCount, 0);
}

void BaseStation::setupTimerCallback(StationCallbackTimer_t* timerCallbacks,
                                     int timerCallbackCount,
                                     int delay) {
  this->timerCallbacks = timerCallbacks;
  this->timerCallbackCount = timerCallbackCount;
  this->log("Timer callbacks :" + String(timerCallbackCount));
  for (size_t i = 0; i < timerCallbackCount; i++) {
    this->logt(String(i) + " -> every " +
               String(this->timerCallbacks[i].interval) + " in " +
               String(this->timerCallbacks[i].next));
  }
}

void BaseStation::setup(Sensor** sensors, int sensorCount) {
  this->setup();
  this->setupSensors(sensors, sensorCount);
}

void BaseStation::setup(StationCallback_t* loopCallbacks,
                        int loopCallbackCount) {
  this->setup();
  this->setupLoopCallback(loopCallbacks, loopCallbackCount);
}

void BaseStation::setup(StationCallbackTimer_t* timerCallbacks,
                        int timerCallbackCount) {
  this->setup();
  this->setupTimerCallback(timerCallbacks, timerCallbackCount);
}

bool BaseStation::ready() {
  return this->ready(BaseStation::StatusReady);
}

bool BaseStation::ready(int minStatus) {
  return this->status >= minStatus;
}

void BaseStation::loop() {
    // this->logt(String(this->sensorCount));
    // this->logt("loop sensors");
    for (size_t i = 0; i < this->sensorCount; i++) {
    // this->logt(String(i));
    // this->logt(this->sensors[i]->name);
    this->sensors[i]->loop();
  }
  // this->logt("loop callbacks");
  for (size_t i = 0; i < this->loopCallbackCount; i++) {
    this->loopCallbacks[i](this);
  }
  // this->logt("loop timers");
  for (size_t i = 0; i < this->timerCallbackCount; i++) {
    int ti = millis();
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
      if (address < 16)
        devices.concat("0");
      devices.concat(String(address, HEX));
      devices.concat("\n");
      nDevices++;
    } else if (error == 4) {
      errors.concat("error @ 0x");
      if (address < 16)
        errors.concat("0");
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
  return this->sensors[index]->toString();
}
String BaseStation::toCsv() {
  String out = "";
  for (size_t i = 0; i < this->sensorCount; i++) {
    out.concat(this->toCsv(i));
  }
  return out;
}
String BaseStation::toCsv(int index) {
  return this->sensors[index]->toCsv();
}
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
  return this->sensors[index]->toJson();
}
String BaseStation::toXml() {
  String out = "";
  for (size_t i = 0; i < this->sensorCount; i++) {
    out.concat(this->toXml(i));
  }
  return out;
}
String BaseStation::toXml(int index) {
  return this->sensors[index]->toXml();
}

// ESP Staion
EspStation::EspStation(String name) : BaseStation(name), webServer(80) {
  this->stationTypeName = "ESP";
}
EspStation::EspStation(String name, WifiInformation wifiInformation)
    : BaseStation(name), webServer(80) {
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

void EspStation::connect() {
  this->connectWifi();
}

void EspStation::initWebServer() {
  this->log("Web Server :");
  this->webServer.onNotFound([](AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not found");
  });
  this->webServer.on("/", [this](AsyncWebServerRequest* request) {
    String data;
    AsyncWebHeader* hd = request->getHeader("Accept");
    const char * accv = hd->value().c_str();
    if (strcmp(accv, "application/json") == 0) {
      data = this->toJson();
    } else if (strcmp(accv, "application/xml") == 0) {
      data = this->toXml();
    } else if (strcmp(accv, "text/csv") == 0) {
      data = this->toCsv();
    } else {
      data = this->toString();
      accv = "text/plain";
    }

    request->send(200, accv, data);
  });
}

void EspStation::setupWebServer(StationWebCallbackInfo_t** routes,
                                int callbackCount) {
  if (this->wifi.status() == WL_CONNECTED) {
    this->initWebServer();
    for (size_t i = 0; i < callbackCount; i++) {
      this->webServer.on(routes[i]->route, [this, routes,
                                            i](AsyncWebServerRequest* request) {
        if (strcmp(routes[i]->login, "") != 0) {
          if (!request->authenticate(routes[i]->login, routes[i]->password)) {
            this->log("Web Server : \n\tAuthentication failed on " +
                      String(routes[i]->route));
            request->requestAuthentication();
          }
        }
        routes[i]->callback(this, request);
      });
    }
  }
}

void EspStation::connectWifi(WifiInformation wifiInformation) {
  this->setWifiInformation(wifiInformation);
  this->connectWifi();
}

void EspStation::connectWifi() {
  if (this->ready(EspStation::StatusConnecting) &&
      this->wifi.status() != WL_CONNECTED &&
      strlen(this->wifiInformation.ssid) > 0 &&
      strlen(this->wifiInformation.password) > 0) {
    this->log("Wifi :");
    this->logt("connecting to " + String(this->wifiInformation.ssid));
    this->wifi.begin(this->wifiInformation.ssid,
                     this->wifiInformation.password);
    if (this->wifi.waitForConnectResult() != WL_CONNECTED) {
      this->status = EspStation::StatusError;
      this->error = EspStation::ErrorWifiConnection;
      this->logt("connection failed ! error : " + String(this->wifi.status()));
    } else {
      this->status = EspStation::StatusConnected;
      this->logt("connected as " + String(this->wifi.localIP().toString()));
    }
  }
}

void EspStation::serve() {
  this->serveWeb();
}

void EspStation::serveWeb() {
  this->initWebServer();
  this->webServer.begin();
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