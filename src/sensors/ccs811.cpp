#include "./ccs811.h"
#include <Arduino.h>

const Sensor::SensorMeasure ccs811::measures[measureCount] = {
    {"eCO2", ""},
    {"VOC", "ppm"}, 
    {"temperature", "°C"}
};

ccs811::ccs811() : Sensor("ccs811") {
    for(int i = 0; i < measureCount; i++) {
        measureSampleLast[i] = 0;
    }
}

ccs811::~ccs811() {}

void ccs811::begin() {
    if (!ccs.begin()) {
        this->status = Sensor::StatusError;
        this->error = Sensor::ErrorNotFound;
        return;
    }
    
    while (!ccs.available()) {
        Serial.println("\t waiting for ccs811");
        delay(100);
    }
    
    this->status = Sensor::StatusReady;
}

String ccs811::getMeasureName(uint8_t index) const {
    if (index < measureCount) {
        return measures[index].name;
    }
    return "";
}

Sensor::SensorMeasure ccs811::getMeasure(uint8_t index) const {
    if (index < measureCount) {
        return measures[index];
    }
    return measures[0];
}

size_t ccs811::getMeasureCount() const {
    return measureCount;
}

Sensor::SensorMeasureData ccs811::read(int index) const {
    if (index >= measureCount) {
        return SensorMeasureData();
    }

    Adafruit_CCS811* sensor = const_cast<Adafruit_CCS811*>(&ccs);
    if (!sensor->available() || !sensor->readData()) {
        return SensorMeasureData();
    }

    float value = -999;
    switch (index) {
        case 0:
            value = sensor->geteCO2();
            break;
        case 1:
            value = sensor->getTVOC();
            break;
        case 2:
            value = sensor->calculateTemperature();
            break;
    }

    return SensorMeasureData(value);
}

Sensor::SensorMeasureData ccs811::readBuffer(int index, int bufferIndex) const {
    if (index >= measureCount || bufferIndex >= 40) {
        return SensorMeasureData();
    }
    return measureBuffers[index][bufferIndex];
}

void ccs811::setMeasure(int index, float value) {
    if (index < measureCount) {
        measureData[index] = SensorMeasureData(value);
        measureBuffers[index].unshift(measureData[index]);
    }
}

Sensor::SensorMeasureData ccs811::average(int last, int index) const {
    if (index >= measureCount) {
        return SensorMeasureData();
    }

    if (last > measureBuffers[index].size()) {
        last = measureBuffers[index].size();
    }

    float sum = 0;
    for (int i = 0; i < last; i++) {
        sum += measureBuffers[index][i];
    }
    return SensorMeasureData(sum / last);
}

String ccs811::jsUtils() const {
    return "";
}

void ccs811::loop() {
    if (!ccs.available()) {
        return;
    }

    unsigned long now = millis();
    bool dataRead = false;

    for (size_t i = 0; i < measureCount; i++) {
        if (now - measureSampleLast[i] >= 5000) {
            if (!dataRead) {
                ccs.readData();
                dataRead = true;
            }
            setMeasure(i, read(i));
            measureSampleLast[i] = now;
        }
    }
}