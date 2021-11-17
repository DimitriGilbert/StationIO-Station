#include <Arduino.h>
#include "../Sensor.h"
 
#include <Adafruit_CCS811.h>  

#ifndef StationIOSensor_ccs811_h
#define StationIOSensor_ccs811_h

class ccs811 : public Sensor
{
private: 
  Adafruit_CCS811 ccs;  
public: 
  ccs811();
  ~ccs811();

  static const size_t mesuresCount = 3;
  static const SensorMesure mesures[3];
  static const u_int *mesuresSampleRates[3];
  unsigned long mesuresSampleLast[3];

  SensorMesureData mesuresDatas[3];
  CircularBuffer<SensorMesureData,40> mesuresBuffers[3];
  
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