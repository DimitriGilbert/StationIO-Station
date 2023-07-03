# StationIO Documentation

StationIO is a "framework" for your arduino project. It abstract both a lot and not much, so you can get started very quickly but it's still arduino. Depending on used boards it has support for :
  
* wifi
* a web server (with default endpoints to get data from sensors)
* timer based execution (run something every n seconds)

## Getting started

The easiest way to get started is to use the [configuration tool](https://dimitrigilbert.github.io/StationIO-Station/) that will guide you through the install process by generating custom install directive (in bash) and the station main file.

## Architecture

### Station

In your main file, you declare a "StationClass".
This is were most the "magic" happens initializing capabilities depending on your MCU (esp8266, esp32 or others).

If using ESP based board, you will have a webserver up and running with default endpoints, more information in [station](documentation/station.md).

Each station you create lives in the `station/` directory, the utility scripts take care of copying everything where it needs before compiling.

### Sensors

The rest of the magic comes from sensor classes that implement a common interface (less barbaric: you interact with all sensors in the same way). It's still great and maintained librairies under the hood, but you do not have to learn each and every one of them to get going.

That allowed me to create generic data structure that I output whatever my sensor is, and therefore could create all the generic fancy things.
More info in [there](documentation/sensors.md).

### Utils

To cap everything, I have a set of utility scripts written in bash to do the heavy lifting.
All heve a `--help` option if you do not feel like going through the [documentation](documentation/utils.md)

If you are wondering, what do I use to add options and flags to my scripts, it is a small bash programm I created called [ParseArger](https://github.com/DimitriGilbert/parseArger). It will create (or update) all the parsing code based of information you provide (with options and flags and...).

## Background and Philosophy

Even for seasoned developers, the arduino world can seem daunting :

* C++ based wit a steep learning curve for advanced project
* tight hardware constraint as ram is counted in KB and storage in MB (if you are lucky)
* existing frameworks don't let me use arduino code easily and are mostly for ESP based boards

What I wanted was something that would :

* generic arduino firmware for cross compatibility
  * developed on a D1 mini and a nodemcu board
  * run awesome on esp32
  * compiles and runs uno and nano (no webserver though)
* handle the basics :
  * sensor management and mesurements
  * custom arduino code integration with or without a timer
  * basic WiFi and web server if possible
* be extensible, it's a parsec away from being great (first time using c++...) but supporting a new sensor is easy, so is adding routes (that call standard arduino functions) to the web server

I could not find that, so instead of whinning, I tried to build a thing : StationIO

## Examples

You can find examples in the [example directory](documentation/examples/) but once again you are better off generating your own using the configuration tool.
