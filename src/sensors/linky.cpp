#include "linky.h"
#include <Arduino.h>

// Define static member for measure information (e.g., hp, hc, tarif, power)
const Sensor::SensorMeasure Linky::measures[measureCount] = {
    {"hp", "W"}, {"hc", "W"}, {"tarif", ""}, {"power", "VA"}};

// Constructor
Linky::Linky(TInfo &lnk) : Sensor("linky"), lnk(lnk)
{
    // Initialize the last sample times
    for (size_t i = 0; i < measureCount; i++)
    {
        measureSampleLast[i] = 10000;
    }
}

// Destructor
Linky::~Linky() {}

// Initialize the Linky sensor
void Linky::begin()
{
    // Initialization logic, if needed
}

// Get the measure name (e.g., "hp", "hc", "tarif", "power")
String Linky::getMeasureName(uint8_t index) const
{
    return measures[index].name;
}

// Get the full measure structure (name and unit)
Sensor::SensorMeasure Linky::getMeasure(uint8_t index) const
{
    return measures[index];
}

// Get the total number of measures
size_t Linky::getMeasureCount() const
{
    return measureCount;
}

// Read a specific measure by index
Sensor::SensorMeasureData Linky::read(int index) const
{
    if (index >= 0 && index < measureCount)
    {
        return measureData[index];
    }
    return 0;
}

// Read a value from the circular buffer
Sensor::SensorMeasureData Linky::readBuffer(int index, int bufferIndex) const
{
    if (index >= 0 && index < measureCount && bufferIndex >= 0 && bufferIndex < measureBuffers[index].size())
    {
        return measureBuffers[index][bufferIndex];
    }
    return 0;
}

// Set a measure value and store it in the buffer
void Linky::setMeasure(int index, float value)
{
    if (index >= 0 && index < measureCount)
    {
        measureData[index] = value;
        measureBuffers[index].unshift(value);
    }
}

// Calculate the average value over the last 'n' samples for a specific measure
Sensor::SensorMeasureData Linky::average(int last, int index) const
{
    if (index < 0 || index >= measureCount)
    {
        return 0;
    }

    float sum = 0;
    int count = min(last, static_cast<int>(measureBuffers[index].size()));
    for (int i = 0; i < count; ++i)
    {
        sum += measureBuffers[index][i];
    }
    return sum / count;
}

// Main loop to read and update sensor data
void Linky::loop()
{
    // Read values from the teleinfo system
    measureData[0] = getValue("HCHP").toFloat();            // hp
    measureData[1] = getValue("HCHC").toFloat();            // hc
    measureData[2] = (getValue("PTEC") == "HC..") ? 1 : 57; // tarif
    measureData[3] = getValue("PAPP").toFloat();            // power

    // Store the data in the circular buffers
    for (int i = 0; i < measureCount; ++i)
    {
        measureBuffers[i].unshift(measureData[i]);
    }
}

// Retrieve a specific value from the teleinfo system
String Linky::getValue(String name)
{
    const char *tgt = name.c_str();
    int tgtl = strlen(tgt);
    ValueList *me = lnk.getList();
    String data = "does not exist";

    while (me->next)
    {
        me = me->next;

        // Check if the name matches
        if (tgtl == strlen(me->name) && strcmp(me->name, tgt) == 0)
        {
            // If a value exists, return it
            if (me->value)
            {
                data = String(me->value);
            }
        }
    }
    return data;
}

// Read all measures
Sensor::SensorMeasureData *Linky::read()
{
    for (size_t i = 0; i < measureCount; i++)
    {
        measureData[i] = read(i);
        measureBuffers[i].unshift(measureData[i]);
    }
    return measureData;
}

// Calculate the average for all measures
Sensor::SensorMeasureData *Linky::average(int last)
{
    for (size_t i = 0; i < measureCount; i++)
    {
        measureData[i] = average(last, i);
    }
    return measureData;
}

String Linky::jsUtils() const
{
    return "const linky_utils = "
           "{format_hp:val=>{linky_utils.conso.hp.push(val);return "
           "parseFloat(val)/1000;}"
           ",format_hc:val=>{linky_utils.conso.hc.push(val);return "
           "parseFloat(val)/1000;}"
           ",format_unit_hp:val=>' KWh'"
           ",format_unit_hc:val=>' KWh'"
           ",format_tarif:val=>parseInt(val)==1?'heures creuses':'heures pleines'"
           ",inChart:name=>name==='power'"
           ",conso:{hp:[], hc:[]}};setTimeout(()=>{let lscr="
           "document.createElement('script');lscr.src='assets/js/linky.js';"
           "document.head.appendChild(lscr);}, 500);";
}

void LinkyOnData(BaseStation *station, ValueList *vallnk, uint8_t flag)
{
#ifdef ESP8266
    digitalWrite(LED_BUILTIN_AUX, LOW);
#endif
    int index = -1;
    float value = -1;
    if (strcmp(vallnk->name, "PAPP") == 0)
    {
        index = 3;
        value = atof(vallnk->value);
#ifdef ESP8266
        digitalWrite(LED_BUILTIN, LOW);
#endif
    }
    else if (strcmp(vallnk->name, "HCHP") == 0)
    {
        index = 0;
        value = atof(vallnk->value);
#ifdef ESP8266
        digitalWrite(LED_BUILTIN, LOW);
#endif
    }
    else if (strcmp(vallnk->name, "HCHC") == 0)
    {
        index = 1;
        value = atof(vallnk->value);
#ifdef ESP8266
        digitalWrite(LED_BUILTIN, LOW);
#endif
    }
    else if (strcmp(vallnk->name, "PTEC") == 0)
    {
        index = 2;
        value = strcmp(vallnk->value, "HC..") == 0 ? 1 : 0;
#ifdef ESP8266
        digitalWrite(LED_BUILTIN, LOW);
#endif
    }
    if (index != -1)
    {
        station->getSensor("linky")->setMeasure(index, value);
    }
#ifdef ESP8266
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN_AUX, HIGH);
#endif
}
// from hallard/teleinfo library example
String LinkyJsonData(ValueList *me) {
  bool firstdata = true;
  String out = "";
  if (me) {
    out.concat("{");
    while (me->next) {
      me = me->next;
      if (!firstdata) {
        out.concat(", ");
      } else {
        firstdata = false;
      }

      out.concat("\"");
      out.concat(me->name);
      out.concat("\":");

      // we have at least something ?
      if (me->value && strlen(me->value)) {
        boolean isNumber = true;
        // uint8_t c;
        char *p = me->value;

        // check if value is number
        while (*p && isNumber) {
          if (*p < '0' || *p > '9') isNumber = false;
          p++;
        }

        // this will add "" on not number values
        if (!isNumber) {
          out.concat("\"");
          out.concat(me->value);
          out.concat("\"");
        }
        // this will remove leading zero on numbers
        else {
          out.concat(atol(me->value));
        }
      }
    }
    // Json end
    out.concat("}");
  }
  return out;
}