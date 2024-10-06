// WifiManager.h
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <vector>
#include "SimpleLogger.h"

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#error "Unsupported platform"
#endif

#include <LittleFS.h>
#include <ArduinoJson.h>

#define WIFI_CONNECT_TIMEOUT 30000 // 30 seconds

struct NetworkInformation {
    String ssid;
    String password;
};

struct ScannedNetwork {
    String ssid;
    int32_t rssi;
};

enum class WifiError {
    None,
    ConnectionFailed,
    ConfigLoadFailed,
    APStartFailed
};

class WifiManager {
public:
    WifiManager();
    void begin();
    void begin(NetworkInformation network);
    void begin(NetworkInformation network, const char* hostname);
    void begin(const char* ssid, const char* password);
    void begin(const char* ssid, const char* password, const char* hostname);

    WifiError loadConfig(const char* filename);
    void scan();
    std::vector<ScannedNetwork> getScannedNetworks();

    bool isConnected();
    WifiError connect();
    WifiError connect(int16_t network);
    WifiError connect(NetworkInformation network);
    WifiError connect(const char* ssid, const char* password);
    void connectAsync(const char* ssid, const char* password);
    bool disconnect();
    bool reconnect();

    bool isAPEnabled();
    WifiError enableAP();
    WifiError enableAP(NetworkInformation network);
    WifiError enableAP(const char* ssid, const char* password);
    WifiError disableAP();

    void setHostname(const char* hostname);
    String getLocalIP();
    int32_t getRSSI();

    SimpleLogger logger;

private:
    std::vector<NetworkInformation> networks;
    int16_t currentNetwork;
    NetworkInformation APInformation;
    char* hostname;
    std::vector<ScannedNetwork> scannedNetworks;

    void initWiFi();
    bool addNetwork(NetworkInformation network);
    bool removeNetwork(int16_t network);
};

#endif // WIFI_MANAGER_H
