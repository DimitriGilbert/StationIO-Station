#include <Arduino.h>
#include <MQ135.h>

#include "../Sensor.h"
#include "../Station.h"

#ifndef StationIOSensor_mq135_h
#define StationIOSensor_mq135_h

class mq135 : public Sensor {
 private:
  MQ135 mq;

 public:
  mq135(int pin);
  ~mq135();

  static const size_t mesuresCount = 7;
  static const SensorMesure mesures[7];
  static const u_int *mesuresSampleRates[7];
  unsigned long mesuresSampleLast[7];

  SensorMesureData mesuresDatas[7];
  CircularBuffer<SensorMesureData, 40> mesuresBuffers[7];

  // void onSetup(StationClass station, int index);
  void begin();
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