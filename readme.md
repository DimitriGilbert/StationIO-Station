# StationIO

An Arduino framework-ish thingy that takes care of the basics !

## TLDR

### Basic Station

Include the station and sensors header files, initialize and voila a connected arduino station !

Here is an example for a DHT11 sensor :

```arduino
#include <Arduino.h>
#include "./Station.h"
#include "./sensors/dht11.h"

StationClass station("StationIO",
                     {ssid : "your ssid", password : "your password"});
Sensor* sens[1] = {
  new dht11(D4)
};

void setup() {
  station.setup();
  station.setupSensors(sens, 1);
}

void loop() {
  station.loop();
}

```

### main.cpp generator

A tool to generate main.cpp file from json configs exists, configs are located in ./config/*.json and an example is available, to run it :

```bash
npx ts-node generator/main.ts
```

to run on a different config edit line 4 of generator/main.ts.

### sensor generator generator

A tool to generate main.cpp file from json configs exists, configs are located in ./generator/*.json and examples are available, to run it :

```bash
npx ts-node generator/sensor.ts
```

to run on a different config edit line 4 of generator/sensor.ts.

## Background and Philosophy

Even for seasoned developers, the arduino world can seem daunting :

* C++ based wit a steep learning curve for advanced project
* tight hardware constraint as ram is counted in KB and storage in MB (if you are lucky)
* existing frameworks don't let me use arduino code easily and are mostly for ESP based boards

What I wanted was something that would :

* generic arduino firmware for cross compatibility
  * developed on a D1 mini and a nodemcu board
  * compiles-ish for uno and esp32
* handle the basics :
  * sensor management and mesurements
  * custom arduino code integration with or without a timer
  * basic WiFi and web server if possible
* be extensible, it's a parsec away from being great (first time using c++...) but supporting a new sensor is easy, so is adding routes (that call standard arduino functions) to the web server

I could not find that, so instead of whinning, I tried to build a thing : StationIO

## Features and Wishmap

### board support

full :

* esp8266

kinda :

* esp32
* Arduino Uno

wish :

* Moooooar
* EVERYTHIIIIING !

### Sensors

I added support for all the sensors I have, using standard libraries (like Adafruit's DHTx or BMPx libs) in a class implementing basic common functionnalities (mesurement, loging, output formating, ...)

support :

* BME280
* BMP280
* DHT11
* MQ2
* SHT21

I miss the sensor to test and enable support :

* BMxx80
* MQx
* DHTx

wish :

* Moooooar
* EVERYTHIIIIING !

### Network

Support :

* WiFi client (ESPx boards)
* web server (ESPx boards)

wish :

* OTA
* RF
* bluetooth
* MQTT
* WiFi AP/relay
* me not kno, wat U uzin ?

### Custom code

As a developer, I wanted to be able to easily add custom code to add functionnality to a project only when needed. I used callbacks, which is just a normal arduino function that takes a fixed set of parameters and they can run either on :

* each loop
* with an interval
* respond to a url on the web server.

wish :

* hardware interrupts
* timer interrupts (cleaner than current method maybe)

## Getting started

### Install

For now you have to clone (or download) the repo, it is not yet packaged as a project or library.

```bash

cd <path to parent directory>
git clone git@github.com:DimitriGilbert/StationIO-Station.git <project name>
cd <project name>
cp ./templates/main.cpp.dist ./src/main.cpp
```

Open the directory and select src/main.cpp in your favorite code editor so we can dissect it together.

### Includes

The only mandatory includes are the arduino librairy and the station header file.
You'll have to include every sensor header file you will use (duh!).

```arduino
#include <Arduino.h>
#include "./Station.h"
#include "./sensors/dht11.h"
#include "./sensors/bme280.h"
#include "./sensors/sht21.h"
# ......

# ___ Custom includes are welcomed ___

```

### Initializers

They take place between includes and the setup function, here you will initialize the station, sensors and every callbacks you wish.

```arduino
# ___ station ___

StationClass station(
  "Station name",
  # WiFi, optional, assumes you have an esp
  {
    ssid : "your ssid",
    password : "your password"
  }
);

# ___ sensors ___

Sensor* sensors[3] = {
  new dht11(D4),
  new bme280(),
  new sht21()
};

# ___ Callback ___

# callback as a declared function
void cb1(BaseStation* station) {
  station->log(station->toString());
}

# ___ loop callback ___

# just for example sake, do not do that, or do but don't tell me it's spitting tons of stuff in Serial ^^
StationCallback_t* loopCallbacks[2] = {
  # using the declared function as a callback
  cb1,
  # using a lambda function as a callback
  [](BaseStation * station){station->log(station->toString());}
};

# ___ timer callback ___

# just for example sake, do not do that, or do but don't tell me it's spitting tons of stuff in Serial ^^
BaseStation::StationCallbackTimer_t timerCallbacks[2] = {
  # using the declared function as a callback every 5 seconds
  {cb1, 5000},
  # using a lambda function as a callback every 12 seconds
  {[](BaseStation * station){station->log(station->toString());}, 12000}
};

# ___ web server callback ___

void wcb(BaseStation* station, AsyncWebServerRequest* request) {
  cb1(station);
}

EspStation::StationWebCallbackInfo_t webRoutes[3] = {
  # using the declared function as a callback
  {wcb, "/route1", "", ""},
  # using a password
  {wcb, "/route2", "my_login", "my_password"},
  # using a lambda function AND a password #GoCrazy #YOLO
  {[](BaseStation * station, AsyncWebServerRequest* request){station->log("#YOLO\n"+station->toString());}, "/go_crazy", "my_login", "my_password"}
};

```

### setup and loop

Just as regular Arduino code (as it is regular Arduino code), you have a setup and a lopp function.
So we are going to do just that, setup then handle the loop :

```arduino
void setup() {
  # main station setup
  station.setup();

  # sensors setup
  station.setupSensors(sensors, 3);

  # setup loop
  station.setupLoopCallback(loopCallbacks, 2);

  # setup timers
  station.setupTimers(timerCallbacks, 2);

  # setup web server
  station.setupWebServer(webRoutes, 3);

  # ___ custom setup code goes here ___

}

void loop() {
  station.loop();

  # ___ custom loop code goes here ___

}

```
