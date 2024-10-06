// bmp085.h
#ifndef StationIOSensor_bmp085_h
#define StationIOSensor_bmp085_h

#include <Arduino.h>
#include "Sensor.h"
#include <Adafruit_BMP085.h>


class bmp085 : public Sensor {
 private:
  Adafruit_BMP085 bmp;
  static const size_t mesuresCount = 3;
  static const SensorMeasure mesures[mesuresCount];
  SensorMeasureData mesuresDatas[mesuresCount];
  CircularBuffer<SensorMeasureData, 40> mesuresBuffers[mesuresCount];

 public:
  bmp085();
  void begin() override;
  SensorMeasure getMeasure(uint8_t index) const override;
  String getMeasureName(uint8_t index) const override;
  size_t getMeasureCount() const override;
  SensorMeasureData read(int index) const override;
  SensorMeasureData readBuffer(int index, int bufferIndex) const override;
  void setMeasure(int index, float value) override;
  SensorMeasureData average(int last, int index) const override;
  String jsUtils() const override;
  void loop() override;
};

#endif