// Station.h
#ifndef STATION_H
#define STATION_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <vector>
#include "Sensor.h"
#include "WifiManager.h"
#include "SimpleLogger.h"

#ifdef ESP32
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>

class BaseStation {
public:
    using StationCallback_t = void (*)(BaseStation*);

    struct StationCallbackTimer_t {
        StationCallback_t callback;
        unsigned int interval;
        unsigned int next;
    };

    BaseStation(const String& name);
    virtual ~BaseStation();

    virtual void setup();
    virtual void loop();

    void setSerial(HardwareSerial& sr);
    void log(const String& data, SimpleLogger::LogLevel level = SimpleLogger::INFO);
    void setupSensors(const std::vector<Sensor*>& sensors);
    void setupLoopCallback(const std::vector<StationCallback_t>& callbacks);
    void setupTimers(const std::vector<StationCallbackTimer_t>& timerCallbacks);
    void setupOTA();

    bool isReady() const;
    bool isReady(int minStatus) const;

    String toOutput(const String& format) const;
    String toOutput(const String& format, int8_t sensorIndex) const;
    String toOutput(const String& format, int8_t sensorIndex, int8_t measureIndex) const;

    Sensor* getSensor(int index) const;
    Sensor* getSensor(const String& name) const;
    String getSensorName(int index) const;

protected:
    String name;
    String stationTypeName;
    SimpleLogger logger;
    int status;
    int error;

    std::vector<Sensor*> sensors;
    std::vector<StationCallback_t> loopCallbacks;
    std::vector<StationCallbackTimer_t> timerCallbacks;

    static constexpr int StatusStarting = 0;
    static constexpr int StatusReady = 1;
    static constexpr int StatusError = -1;

    static constexpr int ErrorNone = 0;
    static constexpr int ErrorUnknown = 1;

    String toString() const;
    String toCsv() const;
    String toJson() const;
    String toXml() const;
    String toOpenMetrics() const;
    String toHtml() const;
};

class EspStation : public BaseStation {
public:
    using StationWebCallback_t = void (*)(BaseStation*, AsyncWebServerRequest*);

    struct StationWebCallbackInfo_t {
        String route;
        String login;
        String password;
        StationWebCallback_t callback;
    };

    EspStation(const String& name);
    ~EspStation() override;

    void setup() override;
    void setup(const String& configFile);
    void loop() override;

    void connect();
    void connect(const String& ssid, const String& password);
    void connect(const String& ssid, const String& password, const String& hostname);
    void connectAsync(const String& ssid, const String& password);

    void initWebServer();
    void setupWebServer(const std::vector<StationWebCallbackInfo_t>& routes);
    void addEndpoint(const StationWebCallbackInfo_t& endpoint);
    void serveWeb();

    WifiManager wifiManager;

protected:
    AsyncWebServer webServer;
    String openApiStr;

    static constexpr int StatusConnecting = 1;
    static constexpr int StatusConnected = 2;
    static constexpr int StatusServing = 3;
    static constexpr int StatusReady = 4;

    static constexpr int ErrorWifiConnection = 2;
};

#ifdef ESP32
class Esp32Station : public EspStation {
public:
    Esp32Station(const String& name);
    ~Esp32Station() override;

    void setup() override;
    void setup(const String& configFile);
};
using StationClass = Esp32Station;
#elif defined(ESP8266)
class Esp8266Station : public EspStation {
public:
    Esp8266Station(const String& name);
    ~Esp8266Station() override;
};
using StationClass = Esp8266Station;
#else
using StationClass = BaseStation;
#endif

#endif // STATION_H
