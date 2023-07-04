# Usage

## ./StationIO

```
StationIO CLI:
	target: target cmd [one of 'build-fs' 'create' 'createStation-cli' 'set-conf' 'upload' 'build-fs' 'read']
Usage :
	./StationIO <target>
```

## ./StationIO build-fs

```
./StationIO sub command help
	
Build station filesystem:
	-j, --js <js>: path to js file, ':' to separate target directory (/assets/js by default), repeatable
	-c, --css <css>: path to css file, ':' to separate target directory (/assets/css by default), repeatable
	-d, --directory <directory>: additional directory path, all files will be gzipped, ':' to separate target directory (/ by default), repeatable
	-f, --file <file>: file to add, will be gzipped, ':' to separate target directory (/ by default), repeatable
	-F, --file-no-gz <file-no-gz>: add a file without gzip, ':' to separate target directory (/ by default), repeatable
	--node <node>: node executable [default: ' node ']
	--pio <pio>: pio executable [default: ' /home/didi/.platformio/penv/bin/pio ']
	-e, --environement <environement>: build environment [default: ' nodemcuv2 ']
	-u, --upload <upload>: station to upload to, will set --environment
	-D|--dry|--no-dry: do not upload (just build for a station)
Usage :
	/home/didi/worskpace/Code/StationIO/StationIO-station/utils/build-fs [--js <value>] [--css <value>] [--directory <value>] [--file <value>] [--file-no-gz <value>] [--node <value>] [--pio <value>] [--environement <value>] [--upload <value>] [--[no-]dry]
```

## ./StationIO create

```
./StationIO sub command help
	
Create a station:
	name: station name
	--ip <ip>: station ip
	-b, --board <board>: PIO build environement name
	--config|--no-config: open configurator, on by default (use --no-config to turn it off)
	--edit|--no-edit: open editor, on by default (use --no-edit to turn it off)
	--force|--no-force: erase if exists
Usage :
	/home/didi/worskpace/Code/StationIO/StationIO-station/utils/create <name> [--ip <value>] [--board <value>] [--[no-]config] [--[no-]edit] [--[no-]force]
```

## ./StationIO createStation-cli

```
./StationIO sub command help
	
[WIP] create a station main.cpp:
	name: station name
	-s, --sensor <sensor>: sensor to add, repeatable
	-t, --timer <timer>: timer to add, repeatable
	-e, --endpoint <endpoint>: endpoint to add, repeatable
	-w, --wifi-ssid <wifi-ssid>: wifi ssid
	-p, --wifi-pass <wifi-pass>: wifi password
	--include <include>: include string
	--declare <declare>: declare string
	--setup <setup>: setup string
	--loop <loop>: loop string
	--node <node>: node executable [default: ' node ']
	--file <file>: file to output to
Usage :
	/home/didi/worskpace/Code/StationIO/StationIO-station/utils/createStation-cli <name> [--sensor <value>] [--timer <value>] [--endpoint <value>] [--wifi-ssid <value>] [--wifi-pass <value>] [--include <value>] [--declare <value>] [--setup <value>] [--loop <value>] [--node <value>] [--file <value>]
```

## ./StationIO set-conf

```
./StationIO sub command help
	
Set station configuration:
	name: station name
	-s, --sensor <sensor>: sensor to add, repeatable
	-i, --ip <ip>: ip address [default: ' 0 ']
	-b, --board <board>: pio env [default: ' nodemcuv2 ']
	-f, --data-file <data-file>: data-file to add, repeatable
	-d, --data-directory <data-directory>: data-directory to add, repeatable
	-j, --js <js>: js to add, repeatable
	-c, --css <css>: css to add, repeatable
Usage :
	/home/didi/worskpace/Code/StationIO/StationIO-station/utils/set-conf <name> [--sensor <value>] [--ip <value>] [--board <value>] [--data-file <value>] [--data-directory <value>] [--js <value>] [--css <value>]
```

## ./StationIO upload

```
./StationIO sub command help
	
Upload a station to the board:
	name: station name
	--ip <ip>: station ip
	-p, --port <port>: host port upload flags, avoid random for firewalls [default: ' 46667 ']
	-b, --board <board>: PIO build environement name
	--pio <pio>: pio executable [default: ' /home/didi/.platformio/penv/bin/pio ']
	--ota|--no-ota: OTA update, on by default (use --no-ota to turn it off)
	--fs|--no-fs: upload fs
Usage :
	/home/didi/worskpace/Code/StationIO/StationIO-station/utils/upload <name> [--ip <value>] [--port <value>] [--board <value>] [--pio <value>] [--[no-]ota] [--[no-]fs]
```

## ./StationIO read

```
./StationIO sub command help
	
Read station data:
	station: station name
	-s, --sensor <sensor>: sensor id, repeatable
	-f, --format <format>: output format [default: ' plain '] [one of 'json' 'csv' 'xml' 'plain' 'html']
	-m, --mesure <mesure>: mesure id, can conflict with sensor, repeatable
	-o, --output <output>: output file
Usage :
	/home/didi/worskpace/Code/StationIO/StationIO-station/utils/read <station> [--sensor <value>] [--format <value>] [--mesure <value>] [--output <value>]
```
