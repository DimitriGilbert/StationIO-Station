#include <Arduino.h>
#include <CircularBuffer.h>

#ifndef StationIOSensor_h
#define StationIOSensor_h

// #include "./Station.h"

class Sensor
{
private:
public:
  typedef struct {
    String name;
    String unit;
  } SensorMesure;
  typedef float SensorMesureData;

  String name;

  static const size_t mesuresCount;
  static const SensorMesure *mesures[];
  static const u_int *mesuresSampleRates[];

  static String *getMesureName(u_int index);

  // status codes
  static const int StatusInitializing = 0;
  static const int StatusReady = 1;
  static const int StatusError = -1;
  // error codes
  static const int ErrorNotFound = 1;

  int status;
  int error;

  bool ready();
  SensorMesureData __read(int index, size_t count, SensorMesureData *datas);
  SensorMesureData __average(int last, int index, size_t count, CircularBuffer<SensorMesureData, 20>buffers[]);
  virtual size_t getMesuresCount() = 0;
  virtual SensorMesureData *read() = 0;
  virtual SensorMesureData read(int index) = 0;
  virtual void setMesure(int index, float value) = 0;
  virtual SensorMesureData *average(int last) = 0;
  virtual SensorMesureData average(int last, int index) = 0;
  virtual String toString() = 0;
  virtual String toString(int index) = 0;
  virtual String toCsv() = 0;
  virtual String toCsv(int index) = 0;
  virtual String toJson() = 0;
  virtual String toJson(int index) = 0;
  virtual String toXml() = 0;
  virtual String toXml(int index) = 0;
  virtual String toHtml() = 0;
  virtual String toHtml(int index) = 0;

  virtual SensorMesureData *read_() = 0;
  virtual SensorMesureData read_(int index) = 0;
  // virtual void loop(BaseStation *station) = 0;
  virtual void loop() = 0;

  Sensor(String name);
  ~Sensor();
  CircularBuffer<SensorMesureData, 20> *mesuresBuffers;
};

#endif
