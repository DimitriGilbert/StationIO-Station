#include "mq2.h"

#ifdef ESP32
mq2::mq2(int pin) : Sensor("mq2"), mq(3.3, 12, pin) {
    for (int i = 0; i < measureCount; i++) {
        measureData[i] = 0;
    }
}
#else
mq2::mq2(int pin) : Sensor("mq2"), mq(3.3, 10, pin) {
    for (int i = 0; i < measureCount; i++) {
        measureData[i] = 0;
    }
}
#endif

void mq2::begin() {
    mq.init();
    status = StatusReady;
}

Sensor::SensorMeasure mq2::getMeasure(uint8_t index) const {
    return measures[index];
}

String mq2::getMeasureName(uint8_t index) const {
    return measures[index].name;
}

size_t mq2::getMeasureCount() const {
    return measureCount;
}

Sensor::SensorMeasureData mq2::read(int index) const {
    if (index >= 0 && index < measureCount) {
        return measureData[index];
    }
    return 0;
}

Sensor::SensorMeasureData mq2::readBuffer(int index, int bufferIndex) const {
    if (index >= 0 && index < measureCount && bufferIndex >= 0 && bufferIndex < measureBuffers[index].size()) {
        return measureBuffers[index][bufferIndex];
    }
    return 0;
}

void mq2::setMeasure(int index, float value) {
    if (index >= 0 && index < measureCount) {
        measureData[index] = value;
        measureBuffers[index].push(value);
    }
}

Sensor::SensorMeasureData mq2::average(int last, int index) const {
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

String mq2::jsUtils() const {
    return "const mq2_utils = {inChart: name => name === 'raw'};";
}

void mq2::loop() {
    measureData[0] = mq.readLPG();
    measureData[1] = mq.readH2();
    measureData[2] = mq.readCO();
    measureData[3] = mq.readAlcohol();
    measureData[4] = mq.readPropane();
    measureData[5] = mq.sensor_volt;

    for (int i = 0; i < measureCount; ++i) {
        measureBuffers[i].push(measureData[i]);
    }
}

// Define static member
const mq2::SensorMeasure mq2::measures[measureCount] = {
    {"LPG", "ppm"},
    {"H2", "ppm"},
    {"CO", "ppm"},
    {"Alcohol", "ppm"},
    {"Propane", "ppm"},
    {"Raw", "v"}
};