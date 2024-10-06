// Sensor.cpp
#include "Sensor.h"

Sensor::Sensor(const String& name) : name(name), status(StatusInitializing), error(0) {}

String Sensor::getName() const {
    return name;
}

bool Sensor::isReady() const {
    return status == StatusReady;
}

String Sensor::toString() const {
    String result = name + "\n";
    for (size_t i = 0; i < getMeasureCount(); ++i) {
        result += "\t" + toString(i) + "\n";
    }
    return result;
}

String Sensor::toString(int index) const {
    SensorMeasure measure = getMeasure(index);
    return measure.name + " : " + String(read(index)) + " " + measure.unit;
}

String Sensor::toCsv() const {
    String result;
    for (size_t i = 0; i < getMeasureCount(); ++i) {
        result += toCsv(i) + "\n";
    }
    return result;
}

String Sensor::toCsv(int index) const {
    SensorMeasure measure = getMeasure(index);
    return name + "," + measure.name + "," + String(read(index)) + "," + measure.unit;
}

String Sensor::toJson() const {
    String result = "\"" + name + "\": {";
    for (size_t i = 0; i < getMeasureCount(); ++i) {
        if (i > 0) result += ",";
        result += toJson(i);
    }
    result += "}";
    return result;
}

String Sensor::toJson(int index) const {
    SensorMeasure measure = getMeasure(index);
    return "\"" + measure.name + "\": " + String(read(index));
}

String Sensor::toXml() const {
    String result = "<" + name + ">";
    for (size_t i = 0; i < getMeasureCount(); ++i) {
        result += toXml(i);
    }
    result += "</" + name + ">";
    return result;
}

String Sensor::toXml(int index) const {
    SensorMeasure measure = getMeasure(index);
    return "<" + measure.name + " unit=\"" + measure.unit + "\">" + String(read(index)) + "</" + measure.name + ">";
}

String Sensor::toHtml() const {
    String result = "<div class=\"sensor " + name + "\">";
    result += "<div class=\"sensor-name\">" + name + "</div>";
    result += "<div class=\"sensor-measures\">";
    for (size_t i = 0; i < getMeasureCount(); ++i) {
        result += toHtml(i);
    }
    result += "</div></div>";
    return result;
}

String Sensor::toHtml(int index) const {
    SensorMeasure measure = getMeasure(index);
    return "<div class=\"measure\">"
           "<span class=\"measure-name\">" + measure.name + "</span>: "
           "<span class=\"measure-value\">" + String(read(index)) + "</span> "
           "<span class=\"measure-unit\">" + measure.unit + "</span>"
           "</div>";
}