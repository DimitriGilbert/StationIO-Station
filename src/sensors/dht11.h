#ifndef DHT11_H
#define DHT11_H

#include "Sensor.h"
#include <DHT.h>

class dht11 : public Sensor {
public:
    dht11(int pin);
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
    DHT dht;
    static const size_t measureCount = 3;
    static const SensorMeasure measures[measureCount];
    mutable SensorMeasureData measureData[measureCount];
    CircularBuffer<SensorMeasureData, 40> measureBuffers[measureCount];
};

#endif // DHT11_H