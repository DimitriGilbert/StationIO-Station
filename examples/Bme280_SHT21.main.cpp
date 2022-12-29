#include <Arduino.h>

#include "./Station.h"

// including sensors
#include "./sensors/bme280.h"
#include "./sensors/sht21.h"

// initializing station
StationClass station(
    "DualSensorStation", {ssid : "MyWifiSSID", password : "null"}
);

// ___ sensors ___
Sensor* sensors[2] = {
    new bme280(),
    new sht21(),
};

void setup() {
  station.setup();

  // setting up sensors
  station.setupSensors(sensors, 2);

  // setting up OTA
  station.setupOTA();

  // ___ custom code goes here ___
}

void loop() {
  station.loop();
  // ___ custom code goes here ___
}
