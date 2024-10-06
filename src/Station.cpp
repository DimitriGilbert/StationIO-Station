// Station.cpp
#include "Station.h"

BaseStation::BaseStation(const String& name)
    : name(name), stationTypeName("Arduino"), status(StatusStarting), error(ErrorNone) {
}

BaseStation::~BaseStation() = default;

void BaseStation::setup() {
    logger.log("Welcome to StationIO", SimpleLogger::INFO);
    logger.log("Station: " + name, SimpleLogger::INFO);
    status = StatusReady;
}

void BaseStation::loop() {
    for (auto& sensor : sensors) {
        sensor->loop();
    }

    for (const auto& callback : loopCallbacks) {
        callback(this);
    }

    unsigned long currentTime = millis();
    for (auto& timer : timerCallbacks) {
        if (currentTime >= timer.next) {
            timer.callback(this);
            timer.next = currentTime + timer.interval;
        }
    }
}

void BaseStation::setSerial(HardwareSerial& sr) {
    logger.begin(sr, 115200);
}

void BaseStation::log(const String& data, SimpleLogger::LogLevel level) {
    logger.log(data, level);
}

void BaseStation::setupSensors(const std::vector<Sensor*>& newSensors) {
    sensors = newSensors;
    log("Sensors: " + String(sensors.size()));
    for (size_t i = 0; i < sensors.size(); ++i) {
        sensors[i]->begin();
        log(String(i) + " -> " + sensors[i]->getName() + (sensors[i]->isReady() ? " ready" : " error"));
    }
}

void BaseStation::setupLoopCallback(const std::vector<StationCallback_t>& callbacks) {
    loopCallbacks = callbacks;
    log("Loop callbacks: " + String(loopCallbacks.size()));
}

void BaseStation::setupTimers(const std::vector<StationCallbackTimer_t>& newTimerCallbacks) {
    timerCallbacks = newTimerCallbacks;
    log("Timer callbacks: " + String(timerCallbacks.size()));
    for (size_t i = 0; i < timerCallbacks.size(); ++i) {
        log(String(i) + " -> every " + String(timerCallbacks[i].interval) + " ms");
    }
}

void BaseStation::setupOTA() {
    ArduinoOTA.onStart([this]() {
        log("Starting OTA update");
    });
    ArduinoOTA.onEnd([this]() {
        log("OTA update finished");
    });
    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        log("OTA Progress: " + String(progress / (total / 100)) + "%");
    });
    ArduinoOTA.onError([this](ota_error_t error) {
        log("OTA Error: " + String(error), SimpleLogger::ERROR);
    });
    ArduinoOTA.begin();
}

bool BaseStation::isReady() const {
    return isReady(StatusReady);
}

bool BaseStation::isReady(int minStatus) const {
    return status >= minStatus;
}

String BaseStation::toOutput(const String& format) const {
    if (format == "json") return toJson();
    if (format == "csv") return toCsv();
    if (format == "xml") return toXml();
    if (format == "openmetrics") return toOpenMetrics();
    if (format == "html") return toHtml();
    return toString();
}

String BaseStation::toOutput(const String& format, int8_t sensorIndex) const {
    if (sensorIndex >= 0 && sensorIndex < sensors.size()) {
        Sensor* sensor = sensors[sensorIndex];
        if (format == "json") return sensor->toJson();
        if (format == "csv") return sensor->toCsv();
        if (format == "xml") return sensor->toXml();
        if (format == "html") return sensor->toHtml();
        return sensor->toString();
    }
    return "";
}

String BaseStation::toOutput(const String& format, int8_t sensorIndex, int8_t measureIndex) const {
    if (sensorIndex >= 0 && sensorIndex < sensors.size()) {
        Sensor* sensor = sensors[sensorIndex];
        if (measureIndex >= 0 && measureIndex < sensor->getMeasureCount()) {
            if (format == "json") return sensor->toJson(measureIndex);
            if (format == "csv") return sensor->toCsv(measureIndex);
            if (format == "xml") return sensor->toXml(measureIndex);
            if (format == "html") return sensor->toHtml(measureIndex);
            return sensor->toString(measureIndex);
        }
    }
    return "";
}

Sensor* BaseStation::getSensor(int index) const {
    if (index >= 0 && index < sensors.size()) {
        return sensors[index];
    }
    return nullptr;
}

Sensor* BaseStation::getSensor(const String& name) const {
    for (auto& sensor : sensors) {
        if (sensor->getName() == name) {
            return sensor;
        }
    }
    return nullptr;
}

String BaseStation::getSensorName(int index) const {
    if (Sensor* sensor = getSensor(index)) {
        return sensor->getName();
    }
    return "";
}

String BaseStation::toString() const {
    String result = "Station: " + name + "\n";
    for (const auto& sensor : sensors) {
        result += sensor->toString() + "\n";
    }
    return result;
}

String BaseStation::toCsv() const {
    String result = "Station,Sensor,Measure,Value,Unit\n";
    for (const auto& sensor : sensors) {
        result += sensor->toCsv();
    }
    return result;
}

String BaseStation::toJson() const {
    String result = "{\"name\":\"" + name + "\",\"sensors\":{";
    for (size_t i = 0; i < sensors.size(); ++i) {
        if (i > 0) result += ",";
        result += sensors[i]->toJson();
    }
    result += "}}";
    return result;
}

String BaseStation::toXml() const {
    String result = "<station name=\"" + name + "\">";
    for (const auto& sensor : sensors) {
        result += sensor->toXml();
    }
    result += "</station>";
    return result;
}

String BaseStation::toOpenMetrics() const {
    String result;
    for (const auto& sensor : sensors) {
        for (size_t i = 0; i < sensor->getMeasureCount(); ++i) {
            Sensor::SensorMeasure measure = sensor->getMeasure(i);
            result += name + "_" + sensor->getName() + "_" + measure.name + "{unit=\"" + measure.unit + "\"} "
                    + String(sensor->read(i)) + "\n";
        }
    }
    return result;
}

String BaseStation::toHtml() const {
    String result = "<h1>Station: " + name + "</h1>";
    for (const auto& sensor : sensors) {
        result += sensor->toHtml();
    }
    return result;
}

// EspStation implementation
EspStation::EspStation(const String& name) : BaseStation(name), webServer(80) {
    stationTypeName = "ESP";
}

EspStation::~EspStation() = default;

void EspStation::setup() {
    BaseStation::setup();
    log("Available Heap: " + String(ESP.getFreeHeap()));
}

void EspStation::setup(const String& configFile) {
    setup();
    if (wifiManager.loadConfig(configFile.c_str()) == WifiError::None) {
        connect();
    } else {
        log("Failed to load WiFi configuration", SimpleLogger::ERROR);
    }
}

void EspStation::loop() {
    BaseStation::loop();
    ArduinoOTA.handle();
}

void EspStation::connect() {
    wifiManager.connect();
}

void EspStation::connect(const String& ssid, const String& password) {
    wifiManager.connect(ssid.c_str(), password.c_str());
}

void EspStation::connect(const String& ssid, const String& password, const String& hostname) {
    wifiManager.begin(ssid.c_str(), password.c_str(), hostname.c_str());
    wifiManager.connect();
}

void EspStation::connectAsync(const String& ssid, const String& password) {
    wifiManager.connectAsync(ssid.c_str(), password.c_str());
}

void EspStation::initWebServer() {
    log("Initializing Web Server");
    webServer.onNotFound([](AsyncWebServerRequest* request) {
        request->send(404, "text/plain", "Not found");
    });

    // Add default endpoints here
    addEndpoint({"/"});
    addEndpoint({"/sensors"});
    addEndpoint({"/average"});
    addEndpoint({"/buffer"});
    addEndpoint({"/fancy"});
    addEndpoint({"/openapi"});
}

void EspStation::setupWebServer(const std::vector<StationWebCallbackInfo_t>& routes) {
    if (wifiManager.isConnected()) {
        initWebServer();
        for (const auto& route : routes) {
            addEndpoint(route);
        }
    }
}

void EspStation::addEndpoint(const StationWebCallbackInfo_t& endpoint) {
    webServer.on(endpoint.route.c_str(), HTTP_GET, [this, endpoint](AsyncWebServerRequest* request) {
        if (endpoint.login.length() > 0) {
            if (!request->authenticate(endpoint.login.c_str(), endpoint.password.c_str())) {
                log("Authentication failed on " + endpoint.route, SimpleLogger::WARNING);
                return request->requestAuthentication();
            }
        }
        if (endpoint.callback) {
            endpoint.callback(this, request);
        } else {
            request->send(200, "text/plain", "Endpoint: " + endpoint.route);
        }
    });

    openApiStr += "  " + endpoint.route + ":\n";
    openApiStr += "    x-stio: true\n";
}

void EspStation::serveWeb() {
    if (wifiManager.isConnected()) {
        webServer.begin();
        log("Web server started");
    } else {
        log("Cannot start web server: WiFi not connected", SimpleLogger::ERROR);
    }
}

#ifdef ESP32
Esp32Station::Esp32Station(const String& name) : EspStation(name) {
    stationTypeName = "ESP32";
}

Esp32Station::~Esp32Station() = default;

void Esp32Station::setup() {
    EspStation::setup();
    if (!LittleFS.begin(false)) {
        log("Failed to mount LittleFS", SimpleLogger::ERROR);
        if (!LittleFS.begin(true)) {
            log("Failed to format LittleFS", SimpleLogger::ERROR);
        } else {
            log("LittleFS formatted successfully", SimpleLogger::INFO);
        }
    }
}

void Esp32Station::setup(const String& configFile) {
    setup();
    EspStation::setup(configFile);
}

#elif defined(ESP8266)
Esp8266Station::Esp8266Station(const String& name) : EspStation(name) {
    stationTypeName = "ESP8266";
}

Esp8266Station::~Esp8266Station() = default;
#endif
