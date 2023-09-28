#include <Arduino.h>
#include <CircularBuffer.h>
// #include "./Station.h"

#ifndef StationIOSensor_h
#define StationIOSensor_h

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
  SensorMesureData __average(
      int last, int index, size_t count,
      CircularBuffer<SensorMesureData, 20> buffers[]
  );
  // virtual void onSetup(StationClass station, int index) = 0;
  virtual void begin() = 0;
  virtual String getName() = 0;
  virtual SensorMesure getMesure(u_int index) = 0;
  virtual String getMesureName(u_int index) = 0;
  virtual size_t getMesuresCount() = 0;
  virtual SensorMesureData *read() = 0;
  virtual SensorMesureData read(int index) = 0;
  virtual SensorMesureData readBuffer(int index, int bufferIndex) = 0;
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
  virtual String jsUtils() = 0;
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

String SensorMesureToHtml(Sensor::SensorMesure mesure, float value);
String SensorToHtml(Sensor *sensor);
String SensorMesureToXml(Sensor::SensorMesure mesure, float value);
String SensorToXml(Sensor *sensor);
String SensorMesureToJson(Sensor::SensorMesure mesure, float value);
String SensorToJson(Sensor *sensor);
String SensorMesureToCsv(Sensor::SensorMesure mesure, float value);
String SensorToCsv(Sensor *sensor);
String SensorMesureToOpenmetrics(Sensor::SensorMesure mesure, float value);
String SensorToOpenmetrics(Sensor *sensor);
String SensorMesureToString(Sensor::SensorMesure mesure, float value);
String SensorToString(Sensor *sensor);
#endif
