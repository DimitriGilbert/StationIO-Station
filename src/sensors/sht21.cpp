#include "sht21.h"

// Define static member (measure descriptions)
const Sensor::SensorMeasure sht21::measures[measureCount] = {
    {"Temperature", "°C"},
    {"Humidity", "%"}
};

// Constructor
sht21::sht21() : Sensor("sht21") {
    // Initialize measure data and buffers
    for (int i = 0; i < measureCount; i++) {
        measureData[i] = 0;
    }
}

// Initialize the SHT21 sensor
void sht21::begin() {
    if (sht.begin()) {
        status = StatusReady;
    } else {
        status = StatusError;
        error = ErrorNotFound;
    }
}

// Return the measure information based on the index
Sensor::SensorMeasure sht21::getMeasure(uint8_t index) const {
    return measures[index];
}

// Return the name of a measure (Temperature or Humidity)
String sht21::getMeasureName(uint8_t index) const {
    return measures[index].name;
}

// Return the total number of measures (Temperature and Humidity)
size_t sht21::getMeasureCount() const {
    return measureCount;
}

// Read a specific measure (Temperature or Humidity) from the sensor
Sensor::SensorMeasureData sht21::read(int index) const {
    if (index >= 0 && index < measureCount) {
        return measureData[index];
    }
    return 0;
}

// Read buffered data for a specific measure
Sensor::SensorMeasureData sht21::readBuffer(int index, int bufferIndex) const {
    if (index >= 0 && index < measureCount && bufferIndex >= 0 && bufferIndex < measureBuffers[index].size()) {
        return measureBuffers[index][bufferIndex];
    }
    return 0;
}

// Set a specific measure value and store it in the buffer
void sht21::setMeasure(int index, float value) {
    if (index >= 0 && index < measureCount) {
        measureData[index] = value;
        measureBuffers[index].push(value);
    }
}

// Calculate the average of the last 'n' values for a specific measure
Sensor::SensorMeasureData sht21::average(int last, int index) const {
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

// Main loop to continuously read sensor data
void sht21::loop() {
    // Update the sensor data
    measureData[0] = sht.readTemperature();
    measureData[1] = sht.readHumidity();

    // Push the new data into the buffers
    for (int i = 0; i < measureCount; ++i) {
        measureBuffers[i].push(measureData[i]);
    }
}

// Provide JavaScript utilities for web-based visualization
String sht21::jsUtils() const {
    return "const sht21_utils = {inChart: name => ['Temperature', 'Humidity'].includes(name)};";
}
