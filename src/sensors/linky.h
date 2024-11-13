#ifndef StationIOSensor_linky_h
#define StationIOSensor_linky_h

#include <Arduino.h>
#include <LibTeleinfo.h>
#include "Sensor.h"
#include "Station.h"

class Linky : public Sensor
{
public:
  // Constructor and Destructor
  Linky(TInfo &lnk);
  ~Linky();

  // Required overrides from Sensor class
  void begin() override;                                                   // Initialize the sensor
  String getMeasureName(uint8_t index) const override;                     // Get the name of a measure
  SensorMeasure getMeasure(uint8_t index) const override;                  // Get a specific measure (struct)
  size_t getMeasureCount() const override;                                 // Get the total number of measures
  SensorMeasureData read(int index) const override;                        // Read a specific measure by index
  SensorMeasureData readBuffer(int index, int bufferIndex) const override; // Read from buffer
  void setMeasure(int index, float value) override;                        // Set measure data
  SensorMeasureData average(int last, int index) const override;           // Calculate average for a specific measure
  String jsUtils() const override;
  void loop() override; // Main loop

  // Additional methods specific to the Linky sensor
  String getValue(String name);         // Retrieve a specific value from the teleinfo system
  SensorMeasureData *read();            // Read all measures
  SensorMeasureData *average(int last); // Calculate average for all measures

private:
  // Teleinfo instance and measures data
  TInfo lnk;
  static const size_t measureCount = 4;                               // Number of measures
  SensorMeasureData measureData[measureCount];                        // Array to store current measure data
  CircularBuffer<SensorMeasureData, 40> measureBuffers[measureCount]; // Buffer for historical data
  static const SensorMeasure measures[measureCount];                  // Definition of measures (e.g., hp, hc, etc.)
  unsigned long measureSampleLast[measureCount];                      // Timestamps of the last samples
};
void LinkyOnData(BaseStation *station, ValueList *vallnk, uint8_t flag);
// from hallard/teleinfo library example
String LinkyJsonData(ValueList *me);
#endif // StationIOSensor_linky_h
