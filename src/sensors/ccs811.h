#ifndef StationIOSensor_ccs811_h
#define StationIOSensor_ccs811_h

#include <Adafruit_CCS811.h>
#include <Arduino.h>
#include "../Sensor.h"
#include "../Station.h"

class ccs811 : public Sensor {
public:
  // Constructor and Destructor
  ccs811();
  ~ccs811() override;

  // Required overrides from Sensor class
  void begin() override;                                                   // Initialize the sensor
  String getMeasureName(uint8_t index) const override;                     // Get the name of a measure
  SensorMeasure getMeasure(uint8_t index) const override;                  // Get a specific measure
  size_t getMeasureCount() const override;                                 // Get total number of measures
  SensorMeasureData read(int index) const override;                        // Read a specific measure
  SensorMeasureData readBuffer(int index, int bufferIndex) const override; // Read from buffer
  void setMeasure(int index, float value) override;                        // Set measure data
  SensorMeasureData average(int last, int index) const override;           // Calculate average
  String jsUtils() const override;                                         // Get JavaScript utilities
  void loop() override;                                                    // Main loop

private:
  Adafruit_CCS811 ccs;
  static const size_t measureCount = 3;                               // Number of measures
  SensorMeasureData measureData[measureCount];                        // Current measure data
  CircularBuffer<SensorMeasureData, 40> measureBuffers[measureCount]; // Historical data buffer
  static const SensorMeasure measures[measureCount];                  // Measure definitions
  unsigned long measureSampleLast[measureCount];                      // Last sample timestamps
};

#endif // StationIOSensor_ccs811_h