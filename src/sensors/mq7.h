#include <Arduino.h>
#include <MQ7.h>

#include "../Sensor.h"
#include "../Station.h"

#ifndef StationIOSensor_mq7_h
#define StationIOSensor_mq7_h

class mq7 : public Sensor {
 private:
  MQ7 mq;

 public:
  mq7(int pin);
  ~mq7();

  static const size_t mesuresCount = 2;
  static const SensorMesure mesures[2];
  static const u_int *mesuresSampleRates[2];
  unsigned long mesuresSampleLast[2];

  SensorMesureData mesuresDatas[2];
  CircularBuffer<SensorMesureData, 40> mesuresBuffers[2];

  // void onSetup(StationClass station, int index);
  void begin();
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

#endif