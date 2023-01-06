#include <Arduino.h>
#include <MQ2.h>

#include "../Sensor.h"
#include "../Station.h"

#ifndef StationIOSensor_mq2_h
#define StationIOSensor_mq2_h

class mq2 : public Sensor {
 private:
  MQ2 mq;

 public:
  mq2(int pin);
  ~mq2();

  static const size_t mesuresCount = 6;
  static const SensorMesure mesures[6];
  static const u_int *mesuresSampleRates[6];
  unsigned long mesuresSampleLast[6];

  SensorMesureData mesuresDatas[6];
  CircularBuffer<SensorMesureData, 40> mesuresBuffers[6];

  // void onSetup(StationClass station, int index);
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