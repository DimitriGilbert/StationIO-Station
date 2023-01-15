#include "./Sensor.h"
#include "./html.h"

Sensor::Sensor(String name) { this->name = name; }
Sensor::~Sensor() {}

bool Sensor::ready() { return this->status > 0; }

Sensor::SensorMesureData Sensor::__read(int index, size_t count,
                                        SensorMesureData *datas) {
  if (size_t(index) > count) {
    return 0;
  }
  return datas[index];
}

Sensor::SensorMesureData Sensor::__average(
    int last, int index, size_t count,
    CircularBuffer<SensorMesureData, 20>buffers[]) {
  if (size_t(index) > count) {
    return 0;
  }
  SensorMesureData data = 0;

  if (last > buffers[index].size()) {
    last = buffers[index].size();
  }

  for (size_t i = 0; i < size_t(last); i++) {
    data = data + buffers[index][i];
  }
  return data / float(last);
}

String SensorMesureToHtml(Sensor::SensorMesure mesure, float value) {
  return HtmlDiv(
      HtmlElt("span", mesure.name, HtmlClass("snMs-name")) + " : " +
          HtmlElt("span", String(value), HtmlClass("snMs-value")) +
          HtmlElt("span", mesure.unit, HtmlClass("snMs-unit")),
      HtmlClass("col snMs " + mesure.name)
  );
}
String SensorToHtml(Sensor *sensor) {
  String html = "";
  size_t mct = sensor->getMesuresCount();
  for (size_t i = 0; i < mct; i++) {
    html += SensorMesureToHtml(sensor->getMesure(i), sensor->read(i));
  }
  return HtmlDiv(
             HtmlDiv(sensor->getName(), HtmlClass("snName")) +
                 HtmlDiv(html, HtmlClass("snMss row")),
             HtmlClass("text-center sensor " + sensor->getName())
         ) +
         sensor->jsUtils();
}

String SensorMesureToXml(Sensor::SensorMesure mesure, float value) {
  return HtmlElt(
      mesure.name, String(value), HtmlAttribute("unit", mesure.unit)
  );
}
String SensorToXml(Sensor *sensor) {
  String xml = "";
  size_t mct = sensor->getMesuresCount();
  for (size_t i = 0; i < mct; i++) {
    xml += SensorMesureToXml(sensor->getMesure(i), sensor->read(i));
  }
  return HtmlElt(sensor->getName(), xml);
}

String SensorMesureToJson(Sensor::SensorMesure mesure, float value) {
  return "\"" + mesure.name + "\": " + String(value);
}
String SensorToJson(Sensor *sensor) {
  String json = "";
  size_t mct = sensor->getMesuresCount();
  for (size_t i = 0; i < mct; i++) {
    json += SensorMesureToJson(sensor->getMesure(i), sensor->read(i));
    if (i < mct - 1) {
      json += ", ";
    }
  }
  return "\"" + sensor->getName() + "\": {" + json + "}";
}

String SensorMesureToCsv(Sensor::SensorMesure mesure, float value) {
  return "\"" + mesure.name + "\", \"" + String(value) + "\", \"" +
         mesure.unit + "\"\n";
}
String SensorToCsv(Sensor *sensor) {
  String csv = "";
  size_t mct = sensor->getMesuresCount();
  for (size_t i = 0; i < mct; i++) {
    csv += sensor->getName() + "," + (sensor->getMesure(i), sensor->read(i));
  }
  return csv;
}

String SensorMesureToString(Sensor::SensorMesure mesure, float value) {
  return mesure.name + " : " + String(value) + " " + mesure.unit + "\n";
}
String SensorToString(Sensor *sensor) {
  String str = sensor->getName() + "\n";
  size_t mct = sensor->getMesuresCount();
  for (size_t i = 0; i < mct; i++) {
    str += "\t" + SensorMesureToString(sensor->getMesure(i), sensor->read(i));
  }
  return str;
}
