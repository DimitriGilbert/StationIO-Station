#include <WifiManager.h>

WifiManager::WifiManager() {
  for (int16_t i = 0; i < StationIONetworkPoolSize; i++) {
    networks[i] = {"", ""};
  }
}

void WifiManager::begin() {
  begin(networks[0], NULL);
}

void WifiManager::begin(NetworkInformation network) {
  begin(network, NULL);
}

void WifiManager::begin(NetworkInformation network, const char* hostname) {
  begin(network.ssid, network.password, hostname);
}

void WifiManager::begin(const char* ssid, const char* password) {
  begin(ssid, password, NULL);
}

void WifiManager::begin(const char* ssid, const char* password, const char* hostname) {
  logger.log("Starting WifiManager",1);

  if (ssid != NULL) {
    networks[0] = {ssid, password};
    currentNetwork = 0;
  }

  wifi.persistent(false);
  wifi.setAutoConnect(false);
  wifi.setAutoReconnect(true);

  // wifi.mode(WIFI_AP_STA);

  if (hostname != NULL) {
    logger.logt("hostname : "+String(hostname),2);
    this->hostname = (char*)malloc(strlen(hostname)+1);
    strcpy(this->hostname, hostname);
  }
  if (this->hostname != NULL) {
    wifi.hostname(this->hostname);
  }
}

void WifiManager::scan() {
  logger.log("Scanning networks",1);
  if (wifi.scanNetworks() == WIFI_SCAN_RUNNING) {
    return;
  }

  reachableNetworkCount = wifi.scanComplete();
}

String WifiManager::scannedSSID(int16_t network) {
  if (network < 0 || network >= reachableNetworkCount) {
    return "";
  }

  return wifi.SSID(network);
}

int32_t WifiManager::scannedRSSI(int16_t network) {
  if (network < 0 || network >= reachableNetworkCount) {
    return 0;
  }

  return wifi.RSSI(network);
}

boolean WifiManager::isConnected() {
  return wifi.status() == WL_CONNECTED;
}

boolean WifiManager::connect() {
  return connect(currentNetwork);
}

boolean WifiManager::connect(int16_t network) {
  if (network < 0 || network >= reachableNetworkCount) {
    return false;
  }

  currentNetwork = network;
  return connect(networks[currentNetwork]);
}

boolean WifiManager::connect(NetworkInformation network) {
  if (network.ssid == NULL) {
    return false;
  }

  return connect(network.ssid, network.password);
}

boolean WifiManager::connect(const char* ssid, const char* password) {
  logger.log("WifiManager connecting to "+String(ssid));
  wifi.begin(ssid, password);

  if (wifi.waitForConnectResult() != WL_CONNECTED) {
    logger.log("WifiManager failed to connect to "+String(ssid), -2);
    return false;
  }
  logger.logt("connected", 2);
  return true;
}

boolean WifiManager::disconnect() {
  logger.log("WifiManager disconnecting");
  return wifi.disconnect();
}

boolean WifiManager::reconnect() {
  logger.log("WifiManager reconnecting");
  return wifi.reconnect();
}

boolean WifiManager::isAPEnabled() {
  WiFiMode_t mode = wifi.getMode();
  return mode == WIFI_AP || mode == WIFI_AP_STA;
}

boolean WifiManager::enableAP() {
  if (strlen(APInformation.ssid) > 0) {
    return enableAP(APInformation);
  }
  return false;
}

boolean WifiManager::enableAP(NetworkInformation network) {
  return enableAP(network.ssid, network.password);
}

boolean WifiManager::enableAP(const char* ssid, const char* password) {
  if (isAPEnabled()) {
    logger.log("WifiManager AP already enabled", -1);
    return true;
  }
  logger.log("WifiManager enabling AP");
  logger.logt("SSID : "+String(ssid), 2);
  logger.logt("Password : "+String(password), 3);
  wifi.mode(WIFI_AP_STA);
  wifi.softAP(ssid, password);
  return isAPEnabled();
}

boolean WifiManager::disableAP() {
  if (!isAPEnabled()) {
    logger.log("WifiManager AP already disabled", -1);
    return true;
  }
  logger.log("WifiManager disabling AP");
  wifi.softAPdisconnect(true);
  wifi.mode(WIFI_STA);
  return !isAPEnabled();
}
