#include <Arduino.h>
#include <LibTeleinfo.h>

#include "../Sensor.h"

#ifndef StationIOSensor_linky_h
#define StationIOSensor_linky_h

class linky : public Sensor {
 public:
  linky(TInfo &lnk);
  ~linky();

  TInfo lnk;
  static const size_t mesuresCount = 4;
  static const SensorMesure mesures[4];
  static const u_int *mesuresSampleRates[4];
  unsigned long mesuresSampleLast[4];

  SensorMesureData mesuresDatas[4];
  CircularBuffer<SensorMesureData, 40> mesuresBuffers[4];

  String getValue(String name);
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
  String jsUtils();
  String toHtml();
  String toHtml(int index);
  SensorMesureData *read_();
  SensorMesureData read_(int index);
  void loop();
};
#endif