#ifndef SHT21_H
#define SHT21_H

#include "Sensor.h"
#include <HTU2xD_SHT2x_Si70xx.h>

class sht21 : public Sensor {
 public:
  sht21();  // Constructor
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
  HTU2xD_SHT2x_SI70xx sht;
  static const size_t measureCount = 2;  // Measures: Temperature, Humidity
  SensorMeasureData measureData[measureCount];  // Array to hold the sensor data
  CircularBuffer<SensorMeasureData, 40> measureBuffers[measureCount];  // Buffers to store data

  static const SensorMeasure measures[measureCount];  // Measures definition
};

#endif // SHT21_H
