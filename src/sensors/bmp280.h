#ifndef BMP280_H
#define BMP280_H

#include "Sensor.h"
#include <Adafruit_BMP280.h>

class bmp280 : public Sensor {
 public:
  bmp280();  // Constructor
  void begin() override;  // Initialize the sensor
  SensorMeasure getMeasure(uint8_t index) const override;  // Get specific measure
  String getMeasureName(uint8_t index) const override;  // Get measure name by index
  size_t getMeasureCount() const override;  // Get the number of measures
  SensorMeasureData read(int index) const override;  // Read a specific measure
  SensorMeasureData readBuffer(int index, int bufferIndex) const override;  // Read buffer data
  void setMeasure(int index, float value) override;  // Set measure data
  SensorMeasureData average(int last, int index) const override;  // Get average value
  void loop() override;  // Main loop to update the sensor

  String jsUtils() const override;  // JavaScript utilities for data visualization

 private:
  Adafruit_BMP280 bmp;
  static const size_t measureCount = 3;  // Number of measures (Temperature, Pressure, Altitude)
  SensorMeasureData measureData[measureCount];  // Array to hold the sensor data
  CircularBuffer<SensorMeasureData, 40> measureBuffers[measureCount];  // Buffers to store data

  static const SensorMeasure measures[measureCount];  // Measures definition
};

#endif // BMP280_H
