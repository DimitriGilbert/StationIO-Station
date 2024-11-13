#include "mq135.h"

#ifdef ESP32
mq135::mq135(int pin) : Sensor("mq135"), mq(3.3, 12, pin) {
    for (int i = 0; i < measureCount; i++) {
        measureData[i] = 0;
    }
}
#else
mq135::mq135(int pin) : Sensor("mq135"), mq(3.3, 10, pin) {
    for (int i = 0; i < measureCount; i++) {
        measureData[i] = 0;
    }
}
#endif

void mq135::begin() {
    mq.init();
    status = StatusReady;
}

Sensor::SensorMeasure mq135::getMeasure(uint8_t index) const {
    return measures[index];
}

String mq135::getMeasureName(uint8_t index) const {
    return measures[index].name;
}

size_t mq135::getMeasureCount() const {
    return measureCount;
}

Sensor::SensorMeasureData mq135::read(int index) const {
    if (index >= 0 && index < measureCount) {
        return measureData[index];
    }
    return 0;
}

Sensor::SensorMeasureData mq135::readBuffer(int index, int bufferIndex) const {
    if (index >= 0 && index < measureCount && bufferIndex >= 0 && bufferIndex < measureBuffers[index].size()) {
        return measureBuffers[index][bufferIndex];
    }
    return 0;
}

void mq135::setMeasure(int index, float value) {
    if (index >= 0 && index < measureCount) {
        measureData[index] = value;
        measureBuffers[index].push(value);
    }
}

Sensor::SensorMeasureData mq135::average(int last, int index) const {
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

String mq135::jsUtils() const {
    return "const mq135_utils = {inChart: name => name === 'raw'};";
}

void mq135::loop() {
    measureData[0] = mq.readCO();
    measureData[1] = mq.readAlcohol();
    measureData[2] = mq.readCO2();
    measureData[3] = mq.readToluen();
    measureData[4] = mq.readNH4();
    measureData[5] = mq.readAceton();
    measureData[6] = mq.sensor_volt;

    for (int i = 0; i < measureCount; ++i) {
        measureBuffers[i].push(measureData[i]);
    }
}

// Define static member
const mq135::SensorMeasure mq135::measures[measureCount] = {
    {"CO", "ppm"},
    {"Alcohol", "ppm"},
    {"CO2", "ppm"},
    {"Toluen", "ppm"},
    {"NH4", "ppm"},
    {"Aceton", "ppm"},
    {"Raw", "v"}
};