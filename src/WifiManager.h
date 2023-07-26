#include <Arduino.h>

#ifndef StationIONetworkPoolSize
#define StationIONetworkPoolSize 3
#endif
#ifndef StationIOLogger_h
#include <SimpleLogger.h>
#endif
#ifndef StationIOWifiManager_h
#define StationIOWifiManager_h
#endif

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
#define StationIOStationConnectable = true
#define StationIOStationConnectableWifi = true
typedef ESP8266WiFiClass StationIOStationWifi;
#else
#define StationIOStationConnectable = false
typedef null StationIOStationWifi;
#endif

typedef struct {
  const char* ssid;
  const char* password;
} NetworkInformation;

typedef struct {
  const char* ssid;
  const int32_t rssi;
} ScannedNetwork;

class WifiManager {
  public:
  static const int8_t StatusError = -1;
  static const int8_t StatusStarting = 0;
  static const int8_t StatusConnecting = 1;
  static const int8_t StatusConnected = 2;
  static const int8_t StatusReady = 3;

  int8_t status = 0;

  StationIOStationWifi wifi;

  SimpleLogger logger;

  NetworkInformation networks[StationIONetworkPoolSize];
  int16_t currentNetwork = 0;
  NetworkInformation APInformation  = {"", ""};
  char* hostname = NULL;
  int16_t reachableNetworkCount = 0;

  WifiManager();
  void begin();
  void begin(NetworkInformation network);
  void begin(NetworkInformation network, const char* hostname);
  void begin(const char* ssid, const char* password);
  void begin(const char* ssid, const char* password, const char* hostname);

  void scan();
  String scannedSSID(int16_t network);
  int32_t scannedRSSI(int16_t network);
  
  boolean isConnected();
  boolean connect();
  boolean connect(int16_t network);
  boolean connect(NetworkInformation network);
  boolean connect(const char* ssid, const char* password);
  boolean disconnect();
  boolean reconnect();

  boolean isAPEnabled();
  boolean enableAP();
  boolean enableAP(NetworkInformation network);
  boolean enableAP(const char* ssid, const char* password);
  boolean disableAP();

};
