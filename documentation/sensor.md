# Sensor

The Sensor class is more of an interface, this is where you really see how much of nothing I know about C++...
When I started, I was confident and like "yeah, you know, I'm gonna make a base class with all the common stuff and then, just, you know, implement the difference, easy, peazy, everything before lunchy"

Yeah, Right !

Now, self loathing apart, it still kind of does what I set out to do, too much has to be reimplemented in the actual sensor class, but it's going to get there at some point.

___Properties___

* `String name` : name of the sensor
* `static const size_t mesuresCount` : number of mesures
* `static const SensorMesure *mesures[]` : array of mesure definitions
* `static const u_int*mesuresSampleRates[]` : array of mesure sample rates
* `static const int StatusReady` : Status Ready
* `static const int StatusError` : Status Error
* `static const int ErrorNotFound` : Error Sensor Not Found
* `int status` : station status
* `int error` : error status

___Methods___

* `virtual void begin()` : sensor start. To be reimplemented in the sensor class.
* `virtual String getName()` : get sensor name. To be reimplemented in the sensor class.
* `virtual SensorMesure getMesure(u_int index)` : get mesure at index. To be reimplemented in the sensor class.
* `virtual String getMesureName(u_int index)` : get mesure name at index. To be reimplemented in the sensor class.
* `virtual size_t getMesuresCount()` : get the number of mesures. To be reimplemented in the sensor class.
* `virtual SensorMesureData *read()` : To be reimplemented in the sensor class.
* `virtual SensorMesureData read(int index)` : read mesures from the value buffer. To be reimplemented in the sensor class.
* `virtual SensorMesureData readBuffer(int index, int bufferIndex)` : read mesure from the value buffer at index. To be reimplemented in the sensor class.
* `virtual void setMesure(int index, float value)` : set mesure value at index.To be reimplemented in the sensor class.
* `virtual SensorMesureData*average(int last)` : average value of mesures for the last n read. To be reimplemented in the sensor class.
* `virtual SensorMesureData average(int last, int index)` : average value of mesure at index for the last n read. To be reimplemented in the sensor class.
* `virtual String toString()` : sensor data to string .To be reimplemented in the sensor class.
* `virtual String toString(int index)` : sensor mesure data at index to string. To be reimplemented in the sensor class.
* `virtual String toCsv()` : sensor data to csv .To be reimplemented in the sensor class.
* `virtual String toCsv(int index)` : sensor mesure data at index to csv. To be reimplemented in the sensor class.
* `virtual String toJson()` : sensor data to json .To be reimplemented in the sensor class.
* `virtual String toJson(int index)` : sensor mesure data at index to json. To be reimplemented in the sensor class.
* `virtual String toXml()` : sensor data to xml .To be reimplemented in the sensor class.
* `virtual String toXml(int index)` : sensor mesure data at index to xml. To be reimplemented in the sensor class.
* `virtual String jsUtils()` : To be reimplemented in the sensor class.
* `virtual String toHtml()` : sensor data to html .To be reimplemented in the sensor class.
* `virtual String toHtml(int index)` : sensor mesure data at index to html. To be reimplemented in the sensor class.
* `virtual SensorMesureData *read_()` : read all sensor mesures. To be reimplemented in the sensor class.
* `virtual SensorMesureData read_(int index)` : read sensor mesure at index. To be reimplemented in the sensor class.
* `virtual void loop()` : sensor loop callback. To be reimplemented in the sensor class.

## Sensor implementations

* bme280
  * temperature
  * humidity
  * pressure
  * altitude
* bmp085
  * temperature
  * pressure
* bmp280
  * temperature
  * pressure
* ccs811
  * volatile organic counpound
  * CO
  * CO2
* dht11
  * temperature
  * humidity
* linky
* mq2
  * air quality
* mq135
  * air quality
* sht21
  * temperature
  * humidity
