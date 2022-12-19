#include <Arduino.h>
#include "../Sensor.h"
 
#include <Adafruit_BMP280.h>  

#ifndef StationIOSensor_bmp280_h
#define StationIOSensor_bmp280_h

class bmp280 : public Sensor
{
private: 
  Adafruit_BMP280 bmp;  
public: 
  bmp280();
  ~bmp280();

  static const size_t mesuresCount = 3;
  static const SensorMesure mesures[3];
  static const u_int *mesuresSampleRates[3];
  unsigned long mesuresSampleLast[3];

  SensorMesureData mesuresDatas[3];
  CircularBuffer<SensorMesureData,40> mesuresBuffers[3];

  size_t getMesuresCount();
  SensorMesureData *read();
  SensorMesureData read(int index);
  SensorMesureData readBuffer(int index, int bufferIndex);
  void setMesure(int index, float value);
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
  String toHtml();
  String toHtml(int index);
  SensorMesureData *read_();
  SensorMesureData read_(int index);
  void loop();
};

#endif
