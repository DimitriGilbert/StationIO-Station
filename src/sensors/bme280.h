#include <Arduino.h>
#include "../Sensor.h"
 
#include <Adafruit_BME280.h>  

#ifndef StationIOSensor_bme280_h
#define StationIOSensor_bme280_h

class bme280 : public Sensor
{
private: 
  Adafruit_BME280 bme;  
public: 
  bme280();
  ~bme280();

  static const size_t mesuresCount = 4;
  static const SensorMesure mesures[4];
  static const u_int *mesuresSampleRates[4];
  unsigned long mesuresSampleLast[4];

  SensorMesureData mesuresDatas[4];
  CircularBuffer<SensorMesureData,40> mesuresBuffers[4];
  
  SensorMesureData *read();
  SensorMesureData read(int index);
  SensorMesureData *average(int last);
  SensorMesureData average(int last, int index);
  String toString();
  String toString(int index);
  String toCsv();
  String toCsv(int index);
  String toJson();
  String toJson(int index);
  String toXml();
  String toXml(int index);
  SensorMesureData *read_();
  SensorMesureData read_(int index);
  void loop();
};

#endif
