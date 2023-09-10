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
    logger.log("Scanned networks index unknown",-1);
    return "";
  }

  return wifi.SSID(network);
}

int32_t WifiManager::scannedRSSI(int16_t network) {
  if (network < 0 || network >= reachableNetworkCount) {
    logger.log("Scanned networks index unknown",-1);
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
  if (network < 0 || network >= StationIONetworkPoolSize) {
    logger.log("network index does not exists", -2);
    return false;
  }

  currentNetwork = network;
  return connect(networks[currentNetwork].ssid, networks[currentNetwork].password);
}

boolean WifiManager::connect(NetworkInformation network) {
  if (network.ssid == NULL) {
    logger.log("network ssid is NULL", -2);
    return false;
  }
  addNetwork(network);

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
  logger.logt("SSID : "+String(ssid), 1);
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

boolean WifiManager::enableRepetitor() {
  if (strlen(APInformation.ssid) > 0) {
    return enableRepetitor(APInformation);
  }
  return false;
}

boolean WifiManager::enableRepetitor(NetworkInformation network) {
  return enableRepetitor(network.ssid, network.password);
}

boolean WifiManager::enableRepetitor(const char* ssid, const char* password) {
  if (!isAPEnabled()) {
    enableAP(ssid, password);
  }
  IPAddress apIP = wifi.localIP();
  IPAddress gatewayIP = wifi.gatewayIP();
  IPAddress subnetMask = wifi.subnetMask();
  wifi.softAPConfig(apIP, gatewayIP, subnetMask);
  // TODO : NAT routing stuff...
  
  return true;
}

boolean WifiManager::addNetwork(NetworkInformation network) {
  for (size_t neti = 0; neti < StationIONetworkPoolSize; neti++) {
    if (networks[neti].ssid == NULL) {
      networks[neti] = network;
      return true;
    }
  }
  networks[StationIONetworkPoolSize - 1] = network;
  return true;
}
boolean WifiManager::setNetwork(NetworkInformation network, int16_t index) {
  if (index < 0 || index >= StationIONetworkPoolSize) {
    return false;
  }

  networks[index] = network;
  return true;
}
boolean WifiManager::removeNetwork(int16_t network) {
  if (network < 0 || network >= StationIONetworkPoolSize) {
    return false;
  }

  networks[network] = {"", ""};
  return true;
}

