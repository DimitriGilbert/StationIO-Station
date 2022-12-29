#include <Arduino.h>

#include "./Station.h"

// including sensors
#include "./sensors/mq2.h"

// initializing station
StationClass station(
    "DualSensorStation", {ssid : "MyWifiSSID", password : "null"}
);

// ___ sensors ___
Sensor* sensors[1] = {
    new mq2(A0),
};

void setup() {
  station.setup();

  // setting up sensors
  station.setupSensors(sensors, 1);

  // setting up OTA
  station.setupOTA();

  // ___ custom code goes here ___
}

void loop() {
  station.loop();
  // ___ custom code goes here ___
}
