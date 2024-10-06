// Sensor.h
#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <CircularBuffer.h>

class Sensor {
public:
    typedef struct {
        String name;
        String unit;
    } SensorMeasure;
    
    typedef float SensorMeasureData;

    Sensor(const String& name);
    virtual ~Sensor() = default;

    virtual void begin() = 0;
    String getName() const;
    virtual SensorMeasure getMeasure(uint8_t index) const = 0;
    virtual String getMeasureName(uint8_t index) const = 0;
    virtual size_t getMeasureCount() const = 0;
    virtual SensorMeasureData read(int index) const = 0;
    virtual SensorMeasureData readBuffer(int index, int bufferIndex) const = 0;
    virtual void setMeasure(int index, float value) = 0;
    virtual SensorMeasureData average(int last, int index) const = 0;
    virtual void loop() = 0;

    // Common implementations
    String toString() const;
    String toString(int index) const;
    String toCsv() const;
    String toCsv(int index) const;
    String toJson() const;
    String toJson(int index) const;
    String toXml() const;
    String toXml(int index) const;
    virtual String jsUtils() const = 0;
    String toHtml() const;
    String toHtml(int index) const;

    bool isReady() const;

protected:
    String name;
    int status;
    int error;

    static constexpr int StatusInitializing = 0;
    static constexpr int StatusReady = 1;
    static constexpr int StatusError = -1;

    static constexpr int ErrorNotFound = 1;
};

#endif // SENSOR_H
