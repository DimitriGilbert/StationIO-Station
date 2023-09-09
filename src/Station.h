#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <WiFiUdp.h>
#include <Wire.h>
#ifndef StationIOStation_h
#define StationIOStation_h

// Hardware specific definitions
#ifdef ESP32
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#define StationIOStationConnectable = true
#define StationIOStationConnectableWifi = true
typedef WiFiClass StationIOStationWifi;
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#define StationIOStationConnectable = true
#define StationIOStationConnectableWifi = true
typedef ESP8266WiFiClass StationIOStationWifi;
#else
#define StationIOStationConnectable = false
typedef null StationIOStationWifi;
#endif

#ifndef StationIOLogger_h
#include <SimpleLogger.h>
#endif
#ifndef StationIOWifiManager_h
#include <WifiManager.h>
#endif
#include "./Sensor.h"

typedef struct {
  const char* ssid;
  const char* password;
} WifiInformation;

typedef struct {
  int nDevice;
  String devices;
  String errors;
} I2CScan;

class BaseStation {
 private:
 public:
  using StationCallback_t = void (*)(BaseStation*);
  typedef struct {
    BaseStation::StationCallback_t callback;
    u_int interval;
    u_int next;
  } StationCallbackTimer_t;

  String name;
  String stationTypeName;

  HardwareSerial serial = Serial;
  SimpleLogger logger = SimpleLogger();

  int status;
  int error;
  size_t sensorCount = 0;
  Sensor** sensors;
  size_t loopCallbackCount = 0;
  StationCallback_t* loopCallbacks;
  size_t timerCallbackCount = 0;
  StationCallbackTimer_t* timerCallbacks;

  static const int StatusStarting = 0;
  static const int StatusReady = 1;
  static const int StatusError = -1;

  static const int ErrorNone = 0;
  static const int ErrorUnknown = 1;

  BaseStation(String name);
  ~BaseStation();

  void setSerial(HardwareSerial sr);
  void log(const String& data);
  void logt(const String& data);
  void log(const String& data, int8_t level);
  void logt(const String& data, int8_t level);
  void setup();
  void setupSensors(Sensor** sensors, size_t sensorCount);
  void setupLoopCallback(StationCallback_t* loopCallbacks,
                         int loopCallbackCount);
  void setupTimers(StationCallbackTimer_t* timerCallbacks,
                          int timerCallbackCount);
  void setupTimers(StationCallbackTimer_t* timerCallbacks,
                          int timerCallbackCount,
                          int delay);
  void setupOTA();
  bool ready();
  bool ready(int minStatus);
  void loop();
  String toOutput(const char* format);
  String toOutput(const char* format, int8_t sensorIndex);
  String toOutput(const char* format, int8_t sensorIndex, int8_t mesureIndex);
  String toString();
  String toString(int index);
  String toCsv();
  String toCsv(int index);
  String toJson();
  String toJson(int index);
  String toXml();
  String toXml(int index);
  String toHtml();
  String toHtml(int index);
  I2CScan scanI2C();
  Sensor* getSensor(int index);
  Sensor* getSensor(String name);
  String getSensorName(int index);
};

class EspStation : public BaseStation {
 private:
 public:
  using StationWebCallback_t = void (*)(BaseStation* station_, AsyncWebServerRequest* req);
  typedef struct {
    const char* route;
    const char* login;
    const char* password;
    EspStation::StationWebCallback_t callback;
  } StationWebCallbackInfo_t;
  AsyncWebServer webServer;
  WifiInformation wifiInformation;
  StationIOStationWifi wifi;
  WifiManager wifiManager = WifiManager();

  static const int StatusConnecting = 1;
  static const int StatusConnected = 2;
  static const int StatusServing = 3;
  static const int StatusReady = 4;

  static const int ErrorWifiConnection = 2;
  String openApiStr;


  EspStation(String name);
  ~EspStation();

  void setup();
  void setup(NetworkInformation wifiInformation);

  void initWebServer();
  void setupWebServer(StationWebCallbackInfo_t** routes, int callbackCount);
  void addEndpoint(StationWebCallbackInfo_t endpoint);
  void connect();
  void connect(String hostname);
  void connect(NetworkInformation wifiInformation);
  void connect(NetworkInformation wifiInformation, String hostname);

  void serve();
  void serveWeb();
};

#ifdef ESP32
class Esp32Station : public EspStation {
 private:
 public:
  Esp32Station(String name);
  ~Esp32Station();
  void setup();
  void setup(NetworkInformation wifiInformation);
};
typedef Esp32Station StationClass;
#elif defined(ESP8266)
class Esp8266Station : public EspStation {
 private:
 public:
  Esp8266Station(String name);
  ~Esp8266Station();
};
typedef Esp8266Station StationClass;
#else
typedef BaseStation StationClass;
#endif

#endif
