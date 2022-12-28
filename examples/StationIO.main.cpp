#include <Arduino.h>
#include "./Station.h"

// including sensors
#include "./sensors/dht11.h"

// initializing station
StationClass station(
  "StationIO" ,
  {
    ssid : "ssid",
    password : "pass"
  }
);

// ___ sensors ___
Sensor* sensors[1] = { 
  new dht11(D4), 
};

// ___ callbacks ___
void stringLog(BaseStation * station){station->log(station->toString());} 

// ___ timers ___
BaseStation::StationCallbackTimer_t timers[1] = { 
  {stringLog, 10000}, 
};


void setup() {
  station.setup();

  // setting up sensors
  station.setupSensors(sensors, 1);

  // setting up timers
  station.setupTimers(timers, 1);

  // ___ custom code goes here ___
}

void loop() {
  station.loop();
  // ___ custom code goes here ___
}
