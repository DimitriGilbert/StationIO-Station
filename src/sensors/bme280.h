#ifndef BME280_H
#define BME280_H

#include "Sensor.h"
#include <Adafruit_BME280.h>

class bme280 : public Sensor {
public:
    bme280();
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

private:
    Adafruit_BME280 bme;
    static const size_t measureCount = 4;
    static const SensorMeasure measures[measureCount];
    mutable SensorMeasureData measureData[measureCount];
    CircularBuffer<SensorMeasureData, 40> measureBuffers[measureCount];
};

#endif // BME280_H