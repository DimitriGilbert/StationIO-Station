#ifndef MQ135_H
#define MQ135_H

#include "Sensor.h"
#include <MQ135.h>

class mq135 : public Sensor {
public:
    mq135(int pin);
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
    MQ135 mq;
    static const size_t measureCount = 7;
    static const SensorMeasure measures[measureCount];
    mutable SensorMeasureData measureData[measureCount];
    CircularBuffer<SensorMeasureData, 40> measureBuffers[measureCount];
};

#endif // MQ135_H