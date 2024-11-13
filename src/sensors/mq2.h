#ifndef MQ2_H
#define MQ2_H

#include "Sensor.h"
#include <MQ2.h>

class mq2 : public Sensor {
public:
    mq2(int pin);
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
    MQ2 mq;
    static const size_t measureCount = 6;
    static const SensorMeasure measures[measureCount];
    mutable SensorMeasureData measureData[measureCount];
    CircularBuffer<SensorMeasureData, 40> measureBuffers[measureCount];
};

#endif // MQ2_H