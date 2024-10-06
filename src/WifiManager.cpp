
// WifiManager.cpp
#include "WifiManager.h"

WifiManager::WifiManager() : currentNetwork(-1), hostname(nullptr) {}

void WifiManager::begin() {
    initWiFi();
}

void WifiManager::begin(NetworkInformation network) {
    begin(network.ssid.c_str(), network.password.c_str());
}

void WifiManager::begin(NetworkInformation network, const char* hostname) {
    begin(network.ssid.c_str(), network.password.c_str(), hostname);
}

void WifiManager::begin(const char* ssid, const char* password) {
    begin(ssid, password, nullptr);
}

void WifiManager::begin(const char* ssid, const char* password, const char* hostname) {
    initWiFi();
    addNetwork({ssid, password});
    setHostname(hostname);
}

void WifiManager::initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.persistent(false);
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(true);
}

WifiError WifiManager::loadConfig(const char* filename) {
    if (!LittleFS.begin()) {
        logger.log("Failed to mount file system", SimpleLogger::ERROR);
        return WifiError::ConfigLoadFailed;
    }

    File configFile = LittleFS.open(filename, "r");
    if (!configFile) {
        logger.log("Failed to open config file", SimpleLogger::ERROR);
        return WifiError::ConfigLoadFailed;
    }

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
        logger.log("Failed to parse config file", SimpleLogger::ERROR);
        return WifiError::ConfigLoadFailed;
    }

    networks.clear();
    JsonArray wifiNetworks = doc["wifi"]["networks"];
    for (JsonObject network : wifiNetworks) {
        const char* ssid = network["ssid"];
        const char* password = network["password"];
        addNetwork({ssid, password});
    }

    const char* newHostname = doc["wifi"]["hostname"];
    setHostname(newHostname);

    return WifiError::None;
}

void WifiManager::scan() {
    scannedNetworks.clear();
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
        scannedNetworks.push_back({WiFi.SSID(i), WiFi.RSSI(i)});
    }
}

std::vector<ScannedNetwork> WifiManager::getScannedNetworks() {
    return scannedNetworks;
}

bool WifiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

WifiError WifiManager::connect() {
    if (networks.empty()) {
        logger.log("No networks configured", SimpleLogger::ERROR);
        return WifiError::ConnectionFailed;
    }
    return connect(0);
}

WifiError WifiManager::connect(int16_t network) {
    if (network < 0 || network >= networks.size()) {
        logger.log("Invalid network index", SimpleLogger::ERROR);
        return WifiError::ConnectionFailed;
    }
    return connect(networks[network]);
}

WifiError WifiManager::connect(NetworkInformation network) {
    return connect(network.ssid.c_str(), network.password.c_str());
}

WifiError WifiManager::connect(const char* ssid, const char* password) {
    logger.log("Connecting to " + String(ssid), SimpleLogger::INFO);
    WiFi.begin(ssid, password);

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_CONNECT_TIMEOUT) {
        delay(100);
    }

    if (WiFi.status() != WL_CONNECTED) {
        logger.log("Failed to connect to " + String(ssid), SimpleLogger::ERROR);
        return WifiError::ConnectionFailed;
    }

    logger.log("Connected to " + String(ssid), SimpleLogger::INFO);
    logger.log("IP: " + WiFi.localIP().toString(), SimpleLogger::INFO);
    return WifiError::None;
}

void WifiManager::connectAsync(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
}

bool WifiManager::disconnect() {
    return WiFi.disconnect();
}

bool WifiManager::reconnect() {
    return WiFi.reconnect();
}

bool WifiManager::isAPEnabled() {
    return WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA;
}

WifiError WifiManager::enableAP() {
    if (APInformation.ssid.length() > 0) {
        return enableAP(APInformation.ssid.c_str(), APInformation.password.c_str());
    }
    logger.log("No AP information set", SimpleLogger::ERROR);
    return WifiError::APStartFailed;
}

WifiError WifiManager::enableAP(NetworkInformation network) {
    return enableAP(network.ssid.c_str(), network.password.c_str());
}

WifiError WifiManager::enableAP(const char* ssid, const char* password) {
    logger.log("Enabling AP: " + String(ssid), SimpleLogger::INFO);
    if (WiFi.softAP(ssid, password)) {
        APInformation = {ssid, password};
        logger.log("AP IP address: " + WiFi.softAPIP().toString(), SimpleLogger::INFO);
        return WifiError::None;
    }
    logger.log("Failed to start AP", SimpleLogger::ERROR);
    return WifiError::APStartFailed;
}

WifiError WifiManager::disableAP() {
    if (isAPEnabled()) {
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);
        return WifiError::None;
    }
    return WifiError::None; // AP was already disabled
}

void WifiManager::setHostname(const char* newHostname) {
    if (newHostname) {
        if (hostname) {
            free(hostname);
        }
        hostname = strdup(newHostname);
        WiFi.setHostname(hostname);
    }
}

String WifiManager::getLocalIP() {
    return WiFi.localIP().toString();
}

int32_t WifiManager::getRSSI() {
    return WiFi.RSSI();
}

bool WifiManager::addNetwork(NetworkInformation network) {
    for (const auto& existingNetwork : networks) {
        if (existingNetwork.ssid == network.ssid) {
            return false; // Network already exists
        }
    }
    networks.push_back(network);
    return true;
}

bool WifiManager::removeNetwork(int16_t network) {
    if (network < 0 || network >= networks.size()) {
        return false;
    }
    networks.erase(networks.begin() + network);
    return true;
}
