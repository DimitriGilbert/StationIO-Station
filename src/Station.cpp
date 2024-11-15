// Station.cpp
#include "Station.h"
#include <HardwareSerial.h>
#include "html.h"

BaseStation::BaseStation(const String& name)
    : name(name), stationTypeName("Arduino"), status(StatusStarting), error(ErrorNone) {
}

BaseStation::~BaseStation() = default;

void BaseStation::setup() {
    logger.begin(Serial);
    logger.log("\n");
    logger.log("StationIO " + name, SimpleLogger::INFO);
    logger.log("\t" + stationTypeName, SimpleLogger::INFO);
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
    logger.begin(sr, 9600);
}

void BaseStation::log(const String& data, SimpleLogger::LogLevel level) {
    logger.log(data, level);
}

void BaseStation::setupSensors(const std::vector<Sensor*>& newSensors) {
    sensors = newSensors;
    log("Sensors: " + String(sensors.size()));
    for (size_t i = 0; i < sensors.size(); ++i) {
        sensors[i]->begin();
        log("\t" + String(i) + ": " + sensors[i]->getName() + (sensors[i]->isReady() ? "" : " /!\\ error"));
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
        log("\t" + String(i) + ": every " + String(timerCallbacks[i].interval) + " ms");
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

String BaseStation::getName() const {
    return name;
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

int BaseStation::getSensorCount() const {
    return sensors.size();
}

void rootEndpoint(BaseStation* station, AsyncWebServerRequest* request) {
    String data;
    AsyncWebHeader* hd = request->getHeader("Accept");
    const char* accv;
    if (request->hasParam("format")) {
      accv = request->getParam("format")->value().c_str();
    } else {
      accv = hd->value().c_str();
    }

    const char* ofrmt;
    if (strcmp(accv, "application/json") == 0) {
      ofrmt = "json";
    } else if (strcmp(accv, "application/xml") == 0) {
      ofrmt = "xml";
    } else if (strcmp(accv, "text/csv") == 0) {
      ofrmt = "csv";
    } else if (strcmp(accv, "text/plain") == 0) {
      ofrmt = "plain";
    } else if (strcmp(accv, "text/raw") == 0) {
      ofrmt = "raw";
    } else if (strcmp(accv, "application/openmetrics-text") == 0 || strcmp(accv, "text/openmetrics") == 0) {
      ofrmt = "openmetrics";
      accv = "application/openmetrics-text";
    } else {
      ofrmt = "html";
      accv = "text/html";
    }

    if (request->hasParam("sensor")) {
      int sid = request->getParam("sensor")->value().toInt();
      if (sid < station->getSensorCount()) {
        if (request->hasParam("mesure")) {
          int mesureid = request->getParam("mesure")->value().toInt();
          data = station->toOutput(ofrmt, sid, mesureid);
        } else {
          data = station->toOutput(ofrmt, sid);
        }
      }
    } else {
      data = station->toOutput(ofrmt);
    }

    request->send(200, accv, data);;
}

void sensorsEndpoint(BaseStation* station, AsyncWebServerRequest* request) {
    request->send(200, "text/html", commonHtmlHeader(station->getName()) +
                  commonBody(station->toHtml()) + "</html>");
}

void averageEndpoint(BaseStation* station, AsyncWebServerRequest* request) {
    String data;
    if (!request->hasParam("sensor")) {
      request->send(400, "text/plain", "Bad Request, missing sensor id");
    } else if (!request->hasParam("mesure")) {
      request->send(400, "text/plain", "Bad Request, missing mesure id");
    } else if (!request->hasParam("count")) {
      request->send(400, "text/plain", "Bad Request, missing count");
    } else {
      data = String(
          station->getSensor(request->getParam("sensor")->value().toInt())->average(
              request->getParam("count")->value().toInt(),
              request->getParam("mesure")->value().toInt()
          )
      );
      request->send(200, "text/plain", data);
    }
}

void bufferEndpoint(BaseStation* station, AsyncWebServerRequest* request) {
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
          station->getSensor(request->getParam("sensor")->value().toInt());
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
}

void openapiEndpoint(BaseStation* station, AsyncWebServerRequest* request) {
    String out = "openapi: 3.0.3\n";
    out += "info:\n";
    out += "  title: Station  " + station->getName() + " Api\n";
    out += "  version: '1.0'\n";
    out += "servers:\n";
    // todo: .... meh, don't know how to fix...
    // out += "  - url: http://" + station->wifiManager.WiFi.softAPIP().toString() + "/\n";
    out += "components:\n";
    out += "  parameters:\n";
    out += "    sensor:\n";
    out += "      name: sensor\n";
    out += "      in: query\n";
    out += "      description: sensor id, starts at 0\n";
    out += "      schema:\n";
    out += "        type: integer\n";
    out += "    mesure:\n";
    out += "      name: mesure\n";
    out += "      in: query\n";
    out += "      schema:\n";
    out += "        type: integer\n";
    out += "      description: mesure id, starts at 0\n";
    out += "    format:\n";
    out += "      name: format\n";
    out += "      in: query\n";
    out += "      description: response format\n";
    out += "      schema:\n";
    out += "        type: string\n";
    out += "paths: \n";
    out += "  /:\n";
    out += "    get:\n";
    out += "      summary: Get station data\n";
    out += "      parameters:\n";
    out += "        - $ref: '#/components/parameters/sensor'\n";
    out += "        - $ref: '#/components/parameters/mesure'\n";
    out += "        - $ref: '#/components/parameters/format'\n";
    out += "      responses:\n";
    out += "        '200':\n";
    out += "          description: Station data\n";
    out += "          content:\n";
    out += "            application/json:{}\n";
    out += "            application/xml:{}\n";
    out += "            text/csv:{}\n";
    out += "            text/plain:{}\n";
    out += "            text/html:{}\n";
    out += "  /sensors:\n";
    out += "    get:\n";
    out += "      summary: Get sensor list\n";
    out += "      responses:\n";
    out += "        '200':\n";
    out += "          description: average value\n";
    out += "          content:\n";
    out += "            application/json:{}\n";
    out += "  /sensorMesures:\n";
    out += "    get:\n";
    out += "      summary: Get sensor mesure list\n";
    out += "      parameters:\n";
    out += "        - $ref: '#/components/parameters/sensor'\n";
    out += "      responses:\n";
    out += "        '200':\n";
    out += "          description: average value\n";
    out += "          content:\n";
    out += "            application/json:{}\n";
    out += "  /average:\n";
    out += "    get:\n";
    out += "      summary: Get average value\n";
    out += "      parameters:\n";
    out += "        - $ref: '#/components/parameters/sensor'\n";
    out += "        - $ref: '#/components/parameters/mesure'\n";
    out += "        - $ref: '#/components/parameters/format'\n";
    out += "      responses:\n";
    out += "        '200':\n";
    out += "          description: average value\n";
    out += "          content:\n";
    out += "            text/plain:{}\n";
    out += "  /buffer:\n";
    out += "    get:\n";
    out += "      summary: Get buffer value\n";
    out += "      parameters:\n";
    out += "        - $ref: '#/components/parameters/sensor'\n";
    out += "        - $ref: '#/components/parameters/mesure'\n";
    out += "        - $ref: '#/components/parameters/format'\n";
    out += "      responses:\n";
    out += "        '200':\n";
    out += "          description: buffer value\n";
    out += "          content:\n";
    out += "            text/plain:{}\n";
    out += "  /fancy:\n";
    out += "    get:\n";
    out += "      summary: A fancy station GUI\n";
    out += "      responses:\n";
    out += "        '200':\n";
    out += "          description: A fancy station GUI\n";
    out += "          content:\n";
    out += "            text/html:{}\n";
    // todo: create/add to openapistr on addEndpoint
    // request->send(200, "text/plain", out + station->openApiStr);
    request->send(200, "text/plain", out);
}

void fancyEndpoint(BaseStation* station, AsyncWebServerRequest* request) {
    request->send(200, "text/html", commonHtmlHeader(station->getName()) +
                  commonBody(station->toHtml()) + "</html>");
}

// EspStation implementation
EspStation::EspStation(const String& name) : BaseStation(name), webServer(80) {
    stationTypeName = "ESP";
}

EspStation::~EspStation() = default;

void EspStation::setup() {
    BaseStation::setup();
    wifiManager.logger = logger;
    log("\tAvailable Heap: " + String(ESP.getFreeHeap()));
}

void EspStation::setup(const String& configFile) {
    setup();
    if (wifiManager.loadConfig(configFile.c_str()) == WifiError::None) {
        connect();
    } else {
        log("Failed to load WiFi configuration", SimpleLogger::ERROR);
    }
}

void EspStation::setup(const String& ssid, const String& password) {
    setup();
    connect(ssid, password);
}

void EspStation::setup(const String& ssid, const String& password, const String& hostname) {
    setup();
    connect(ssid, password, hostname);
}

void EspStation::loop() {
    BaseStation::loop();
    if (mqttEnabled) {
        mqttClient.loop();
    }
    ArduinoOTA.handle();
}

void EspStation::connect() {
    wifiManager.connect();
}

void EspStation::connect(const String& ssid, const String& password) {
    connect(ssid.c_str(), password.c_str(), ("SIO_" + name).c_str());
}

void EspStation::connect(const String& ssid, const String& password, const String& hostname) {
    // log("Connecting to " + ssid + " with hostname " + hostname);
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
    addEndpoint({"/", "", "", rootEndpoint});
    addEndpoint({"/sensors", "", "", sensorsEndpoint});
    addEndpoint({"/average", "", "", averageEndpoint});
    addEndpoint({"/buffer", "", "", bufferEndpoint});
    addEndpoint({"/fancy", "", "", fancyEndpoint});
    addEndpoint({"/openapi", "", "", openapiEndpoint});
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
        webServer.serveStatic("/assets", LittleFS, "/assets");
        webServer.begin();
        log("Web server started");
    } else {
        log("Cannot start web server: WiFi not connected", SimpleLogger::ERROR);
    }
}

void EspStation::setupMqtt(const String& broker, int port, const String& username, const String& password) {
    mqttBroker = broker;
    mqttPort = port;
    mqttUsername = username;
    mqttPassword = password;
    mqttEnabled = true;
    
    mqttClient.setClient(wifiManager.getClient());
    mqttClient.setServer(mqttBroker.c_str(), mqttPort);
}

bool EspStation::connectMqtt() {
    if (!mqttEnabled) return false;
    
    String clientId = "ESP-" + name + "-" + String(random(0xffff), HEX);
    
    bool connected = false;
    if (mqttUsername.length() > 0) {
        connected = mqttClient.connect(clientId.c_str(), mqttUsername.c_str(), mqttPassword.c_str());
    } else {
        connected = mqttClient.connect(clientId.c_str());
    }
    
    if (connected) {
        log("Connected to MQTT broker", SimpleLogger::INFO);
    } else {
        log("Failed to connect to MQTT broker", SimpleLogger::ERROR);
    }
    
    return connected;
}

void EspStation::loopMqtt() {
    if (!mqttEnabled) return;
    
    if (!mqttClient.connected()) {
        connectMqtt();
    }
    mqttClient.loop();
}

void EspStation::publishMqtt(const String& topic, const String& payload) {
    if (!mqttEnabled || !mqttClient.connected()) return;
    
    mqttClient.publish(topic.c_str(), payload.c_str());
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

void Esp32Station::setup(const String& ssid, const String& password) {
    setup();
    connect(ssid, password);
}

void Esp32Station::setup(const String& ssid, const String& password, const String& hostname) {
    setup();
    connect(ssid, password, hostname);
}

#elif defined(ESP8266)
Esp8266Station::Esp8266Station(const String& name) : EspStation(name) {
    stationTypeName = "ESP8266";
}

Esp8266Station::~Esp8266Station() = default;
#endif
