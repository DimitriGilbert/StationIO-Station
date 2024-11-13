#include "dht11.h"

dht11::dht11(int pin) : Sensor("dht11"), dht(pin, DHT11) {
    for (int i = 0; i < measureCount; i++) {
        measureData[i] = 0;
    }
}

void dht11::begin() {
    dht.begin();
    status = StatusReady;
}

Sensor::SensorMeasure dht11::getMeasure(uint8_t index) const {
    return measures[index];
}

String dht11::getMeasureName(uint8_t index) const {
    return measures[index].name;
}

size_t dht11::getMeasureCount() const {
    return measureCount;
}

Sensor::SensorMeasureData dht11::read(int index) const {
    if (index >= 0 && index < measureCount) {
        return measureData[index];
    }
    return 0;
}

Sensor::SensorMeasureData dht11::readBuffer(int index, int bufferIndex) const {
    if (index >= 0 && index < measureCount && bufferIndex >= 0 && bufferIndex < measureBuffers[index].size()) {
        return measureBuffers[index][bufferIndex];
    }
    return 0;
}

void dht11::setMeasure(int index, float value) {
    if (index >= 0 && index < measureCount) {
        measureData[index] = value;
        measureBuffers[index].push(value);
    }
}

Sensor::SensorMeasureData dht11::average(int last, int index) const {
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

String dht11::jsUtils() const {
    return "const dht11_utils = {inChart: name => ['temperature', 'humidity'].includes(name)};";
}

void dht11::loop() {
    measureData[0] = dht.readTemperature();
    measureData[1] = dht.readHumidity();
    measureData[2] = dht.computeHeatIndex(measureData[0], measureData[1], false);

    for (int i = 0; i < measureCount; ++i) {
        measureBuffers[i].push(measureData[i]);
    }
}

// Define static member
const dht11::SensorMeasure dht11::measures[measureCount] = {
    {"temperature", "°C"},
    {"humidity", "%"},
    {"heat_index", "°C"}
};