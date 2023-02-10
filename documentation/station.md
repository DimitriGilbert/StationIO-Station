# Station

The station classes are the base of StationIO. You instantiate a Station in your main file and it will provide a set of methods to simplify some things.
Depending on your MCU, different librairies will be loaded and methods will differ slightly to provide as much feature parity and common API as possible.
You have nothing to do and the correct class will be available as `StationClass` once you include `Station.h`.

## Station Classes

### BaseStation

It's the common base for every other station classes. It does not support wifi or web server as it is meant to provide as wide a compatibility as possible, but everything else is here.

___Properties___

* `String name` : station name
* `String stationTypeName` : Station type
* `HardwareSerial serial` : serial class
* `int status` : station status
* `int error` : error status
* `size_t sensorCount` : number of sensors
* `Sensor** sensors` : list of sensor
* `size_t timerCallbackCount` : number of timers
* `StationCallbackTimer_t* timerCallbacks` : list of timers
* `static const int StatusStarting` : Status Starting
* `static const int StatusReady` : Status Ready
* `static const int StatusError` : Status Error
* `static const int ErrorNone` : Error None
* `static const int ErrorUnknown` : Error Unknown

___Methods___

* `void setSerial(HardwareSerial sr)` : Set Serial class
* `void log(const String& data)` : print new line to serial
* `void logt(const String& data)` : print new line starting with a tab to serial
* `void setup()` : setup station
* `void setupSensors(Sensor** sensors, size_t sensorCount)` : setup sensors
* `void setupTimers(StationCallbackTimer_t* timerCallbacks,int timerCallbackCount)` : set up timers
* `void setupTimers(StationCallbackTimer_t* timerCallbacks,int timerCallbackCount,int delay)` : set up timers and delay the first run
* `void setupOTA()` : setup OTA class
* `bool ready()` : is the class ready ?
* `bool ready(int minStatus)` : is the class above a certain status
* `void loop()` : station loop
* `String toString()` : get station data as string.
* `String toString(int index)` : get station sensor at index data as string.
* `String toCsv()` : get station data as csv.
* `String toCsv(int index)` : get station sensor at index data as csv.
* `String toJson()` : get station data as json.
* `String toJson(int index)` : get station sensor at index data as json.
* `String toXml()` : get station data as xml.
* `String toXml(int index)` : get station sensor at index data as xml.
* `String toHtml()` : get station data as html.
* `String toHtml(int index)` : get station sensor at index data as html.
* `Sensor* getSensor(int index)` : get sensor at index
* `Sensor* getSensor(String name)` : get sensor by name
* `String getSensorName(int index)` : get sensor name

### EspStation

Common ESP class with all the WiFi and web servevr goodness.

___Properties___

* `AsyncWebServer webServer` : web server instance
* `WifiInformation wifiInformation` : ssid and password information
* `StationIOStationWifi wifi` : wifi class specific to esp type
* `static const int StatusConnecting` : Status Connecting
* `static const int StatusConnected` : Status Connected
* `static const int StatusServing` : Status Serving
* `static const int StatusReady` : Status Ready
* `static const int ErrorWifiConnection` : Error Wifi Connection
* `String openApiStr` : OpenApi documentation yaml string

___Methods___

* `void initWebServer()` : initialize web server
* `void addEndpoint(StationWebCallbackInfo_t endpoint)` : and a web server endpoint
* `void connect(String hostname)` : connect all networks with a specific hostname
* `void connect()` : connect all networks
* `void connectWifi()` : connect to the configured wifi network
* `void connectWifi(WifiInformation wifiInformation)` : connect to a wifi network
* `String scanWifi()` : scan available wifi networks
* `void setWifiInformation(WifiInformation wifiInformation)` : set ssid and password
* `void serve()` : start servers
* `void serveWeb()` : initialize and start the web server

### Esp8266Station and Esp32Station

A few tweeks to the station setup is all there is.
