#include "bme280.h"

bme280::bme280() : Sensor("BME280") {}

void bme280::begin() {
    if (bme.begin()) {
        status = StatusReady;
    } else {
        status = StatusError;
        error = ErrorNotFound;
    }
}

Sensor::SensorMeasure bme280::getMeasure(uint8_t index) const {
    return measures[index];
}

String bme280::getMeasureName(uint8_t index) const {
    return measures[index].name;
}

size_t bme280::getMeasureCount() const {
    return measureCount;
}

Sensor::SensorMeasureData bme280::read(int index) const {
    if (index >= 0 && index < measureCount) {
        return measureData[index];
    }
    return 0;
}

Sensor::SensorMeasureData bme280::readBuffer(int index, int bufferIndex) const {
    if (index >= 0 && index < measureCount && bufferIndex >= 0 && bufferIndex < measureBuffers[index].size()) {
        return measureBuffers[index][bufferIndex];
    }
    return 0;
}

void bme280::setMeasure(int index, float value) {
    if (index >= 0 && index < measureCount) {
        measureData[index] = value;
        measureBuffers[index].push(value);
    }
}

Sensor::SensorMeasureData bme280::average(int last, int index) const {
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

String bme280::jsUtils() const {
    return "const bme280_utils = {inChart: name => ['temperature', 'humidity', 'pressure'].includes(name)};";
}

void bme280::loop() {
    measureData[0] = bme.readTemperature();
    measureData[1] = bme.readHumidity();
    measureData[2] = bme.readPressure();
    measureData[3] = bme.readAltitude(1013.25);

    for (int i = 0; i < measureCount; ++i) {
        measureBuffers[i].push(measureData[i]);
    }
}

// Define static member
const bme280::SensorMeasure bme280::measures[measureCount] = {
    {"Temperature", "°C"},
    {"Humidity", "%"},
    {"Pressure", "Pa"},
    {"Altitude", "m"}
};