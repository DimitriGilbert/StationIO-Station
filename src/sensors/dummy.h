#include <Arduino.h>
#include "../Sensor.h"

#ifndef StationIOSensorDummy_h
#define StationIOSensorDummy_h

class dummy : public Sensor
{
private:
public:
  dummy();
  ~dummy();

  String name;
  static const size_t mesuresCount = 4;
  static const SensorMesure mesures[4];
  static const u_int *mesuresSampleRates[4];

  SensorMesureData mesuresDatas[4];
  
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
  String toHtml();
  String toHtml(int index);
  SensorMesureData *read_();
  SensorMesureData read_(int index);
  void loop();
};

#endif