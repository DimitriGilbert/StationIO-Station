#include "bmp280.h"

// Define static member (measure descriptions)
const Sensor::SensorMeasure bmp280::measures[measureCount] = {
    {"Temperature", "°C"},
    {"Pressure", "Pa"},
    {"Altitude", "m"}
};

// Constructor
bmp280::bmp280() : Sensor("bmp280") {
    // Initialize measure data and buffers
    for (int i = 0; i < measureCount; i++) {
        measureData[i] = 0;
    }
}

// Initialize the BMP280 sensor
void bmp280::begin() {
    if (bmp.begin()) {
        status = StatusReady;
    } else {
        status = StatusError;
        error = ErrorNotFound;
    }
}

// Return the measure information based on the index
Sensor::SensorMeasure bmp280::getMeasure(uint8_t index) const {
    return measures[index];
}

// Return the name of a measure (Temperature, Pressure, Altitude)
String bmp280::getMeasureName(uint8_t index) const {
    return measures[index].name;
}

// Return the total number of measures (Temperature, Pressure, Altitude)
size_t bmp280::getMeasureCount() const {
    return measureCount;
}

// Read a specific measure (Temperature, Pressure, or Altitude) from the sensor
Sensor::SensorMeasureData bmp280::read(int index) const {
    if (index >= 0 && index < measureCount) {
        return measureData[index];
    }
    return 0;
}

// Read buffered data for a specific measure
Sensor::SensorMeasureData bmp280::readBuffer(int index, int bufferIndex) const {
    if (index >= 0 && index < measureCount && bufferIndex >= 0 && bufferIndex < measureBuffers[index].size()) {
        return measureBuffers[index][bufferIndex];
    }
    return 0;
}

// Set a specific measure value and store it in the buffer
void bmp280::setMeasure(int index, float value) {
    if (index >= 0 && index < measureCount) {
        measureData[index] = value;
        measureBuffers[index].push(value);
    }
}

// Calculate the average of the last 'n' values for a specific measure
Sensor::SensorMeasureData bmp280::average(int last, int index) const {
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
void bmp280::loop() {
    // Update the sensor data
    measureData[0] = bmp.readTemperature();
    measureData[1] = bmp.readPressure();
    measureData[2] = bmp.readAltitude(1013.25);  // Standard sea-level pressure

    // Push the new data into the buffers
    for (int i = 0; i < measureCount; ++i) {
        measureBuffers[i].push(measureData[i]);
    }
}

// Provide JavaScript utilities for web-based visualization
String bmp280::jsUtils() const {
    return "const bmp280_utils = {inChart: name => ['Temperature', 'Pressure', 'Altitude'].includes(name)};";
}
