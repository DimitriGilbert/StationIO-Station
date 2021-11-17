#include <Arduino.h>
#include "../Sensor.h"
 
#include <HTU2xD_SHT2x_Si70xx.h>  

#ifndef StationIOSensor_sht21_h
#define StationIOSensor_sht21_h

class sht21 : public Sensor
{
private: 
  HTU2xD_SHT2x_SI70xx sht;  
public: 
  sht21();
  ~sht21();

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