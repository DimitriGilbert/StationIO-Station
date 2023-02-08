#include <Arduino.h>
#include <LibTeleinfo.h>

#include "../Sensor.h"
#include "../Station.h"

#ifndef StationIOSensor_linky_h
#define StationIOSensor_linky_h

class Linky : public Sensor {
 public:
  Linky(TInfo &lnk);
  ~Linky();

  TInfo lnk;
  static const size_t mesuresCount = 4;
  static const SensorMesure mesures[4];
  static const u_int *mesuresSampleRates[4];
  unsigned long mesuresSampleLast[4];

  SensorMesureData mesuresDatas[4];
  CircularBuffer<SensorMesureData, 40> mesuresBuffers[4];

  // void onSetup(StationClass station, int index);
  void begin();
  String getValue(String name);
  String getName();
  String getMesureName(u_int index);
  SensorMesure getMesure(u_int index);
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

String LinkyJsonData(ValueList *me);
void LinkyOnData(BaseStation *station, ValueList *vallnk, uint8_t flag);

#endif