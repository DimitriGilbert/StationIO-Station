#ifndef MQ7_H
#define MQ7_H

#include "Sensor.h"
#include <MQ7.h>

class mq7 : public Sensor {
public:
    mq7(int pin);
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
    MQ7 mq;
    static const size_t measureCount = 2;
    static const SensorMeasure measures[measureCount];
    mutable SensorMeasureData measureData[measureCount];
    CircularBuffer<SensorMeasureData, 40> measureBuffers[measureCount];
};

#endif // MQ7_H