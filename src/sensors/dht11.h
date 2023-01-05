#include <Arduino.h>
#include <DHT.h>

#include "../Sensor.h"
#include "../Station.h"

#ifndef StationIOSensor_dht11_h
#define StationIOSensor_dht11_h

class dht11 : public Sensor {
 private:
  DHT dht;

 public:
  dht11(int pin);
  ~dht11();

  static const size_t mesuresCount = 3;
  static const SensorMesure mesures[3];
  static const u_int *mesuresSampleRates[3];
  unsigned long mesuresSampleLast[3];

  SensorMesureData mesuresDatas[3];
  CircularBuffer<SensorMesureData, 40> mesuresBuffers[3];

  void onSetup(StationClass station, int index);
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