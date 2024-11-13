#include "mq7.h"

mq7::mq7(int pin) : Sensor("mq7"), mq(3.3, 12, pin) {
    for (int i = 0; i < measureCount; i++) {
        measureData[i] = 0;
    }
}

void mq7::begin() {
    mq.init();
    status = StatusReady;
}

Sensor::SensorMeasure mq7::getMeasure(uint8_t index) const {
    return measures[index];
}

String mq7::getMeasureName(uint8_t index) const {
    return measures[index].name;
}

size_t mq7::getMeasureCount() const {
    return measureCount;
}

Sensor::SensorMeasureData mq7::read(int index) const {
    if (index >= 0 && index < measureCount) {
        return measureData[index];
    }
    return 0;
}

Sensor::SensorMeasureData mq7::readBuffer(int index, int bufferIndex) const {
    if (index >= 0 && index < measureCount && bufferIndex >= 0 && bufferIndex < measureBuffers[index].size()) {
        return measureBuffers[index][bufferIndex];
    }
    return 0;
}

void mq7::setMeasure(int index, float value) {
    if (index >= 0 && index < measureCount) {
        measureData[index] = value;
        measureBuffers[index].push(value);
    }
}

Sensor::SensorMeasureData mq7::average(int last, int index) const {
    if (index >= 0 && index < measureCount) {
        float sum = 0;
        int count = min(last, static_cast<int>(measureBuffers[index].size()));
        for (int i = 0; i < count; ++i) {
            sum += measureBuffers[index][i];
        }
        return sum / count;
    }
    return 0;
}

String mq7::jsUtils() const {
    return "const mq7_utils = {inChart: name => name === 'raw'};";
}

void mq7::loop() {
    measureData[0] = mq.readCO();
    measureData[1] = mq.sensor_volt;

    for (int i = 0; i < measureCount; ++i) {
        measureBuffers[i].push(measureData[i]);
    }
}

// Define static member
const mq7::SensorMeasure mq7::measures[measureCount] = {
    {"CO", "ppm"},
    {"Raw", "v"}
};