# StationIO

## StationIO

```
StationIO CLI:
	target: target cmd [one of 'build-fs' 'create' 'create-main' 'list' 'read' 'read-all' 'set-conf' 'upload' 'build-fs' 'hub' 'up']
Usage :
	StationIO <target>
```

## StationIO build-fs

```
StationIO sub command help
	
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
	StationIO build-fs [--js <value>] [--css <value>] [--directory <value>] [--file <value>] [--file-no-gz <value>] [--node <value>] [--pio <value>] [--environement <value>] [--upload <value>] [--[no-]dry]
```

## StationIO create

```
StationIO sub command help
	
Create a station:
	name: station name
	--ip <ip>: station ip
	-b, --board <board>: PIO build environement name
	--config|--no-config: open configurator, on by default (use --no-config to turn it off)
	--edit|--no-edit: open editor, on by default (use --no-edit to turn it off)
	--force|--no-force: erase if exists
Usage :
	StationIO create <name> [--ip <value>] [--board <value>] [--[no-]config] [--[no-]edit] [--[no-]force]
```

## StationIO create-main

```
StationIO sub command help
	
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
	StationIO create-main <name> [--sensor <value>] [--timer <value>] [--endpoint <value>] [--wifi-ssid <value>] [--wifi-pass <value>] [--include <value>] [--declare <value>] [--setup <value>] [--loop <value>] [--node <value>] [--file <value>]
```

## StationIO list

```
StationIO sub command help
	
list stations:
	--ip|--no-ip: show ips
	--board|--no-board: show board
Usage :
	StationIO list [--[no-]ip] [--[no-]board]
```

## StationIO read

```
StationIO sub command help
	
Read station data:
	station: station name
	-s, --sensor <sensor>: sensor id, repeatable
	-f, --format <format>: output format [default: ' plain '] [one of 'json' 'openmetrics' 'csv' 'xml' 'plain' 'html']
	-m, --mesure <mesure>: mesure id, can conflict with sensor, repeatable
	-o, --output <output>: output file
Usage :
	StationIO read <station> [--sensor <value>] [--format <value>] [--mesure <value>] [--output <value>]
```

## StationIO read-all

```
StationIO sub command help
	
read all stations:
	-x, --exclude <exclude>: exclude a station, repeatable
	-f, --format <format>: responses format
	-o, --output <output>: output file
Usage :
	StationIO read-all [--exclude <value>] [--format <value>] [--output <value>]
```

## StationIO set-conf

```
StationIO sub command help
	
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
	StationIO set-conf <name> [--sensor <value>] [--ip <value>] [--board <value>] [--data-file <value>] [--data-directory <value>] [--js <value>] [--css <value>]
```

## StationIO upload

```
StationIO sub command help
	
Upload a station to the board:
	name: station name
	--ip <ip>: station ip
	-p, --port <port>: host port upload flags, avoid random for firewalls [default: ' 46667 ']
	-b, --board <board>: PIO build environement name
	--pio <pio>: pio executable [default: ' /home/didi/.platformio/penv/bin/pio ']
	--ota|--no-ota: OTA update, on by default (use --no-ota to turn it off)
	--fs|--no-fs: upload fs
Usage :
	StationIO upload <name> [--ip <value>] [--port <value>] [--board <value>] [--pio <value>] [--[no-]ota] [--[no-]fs]
```

## StationIO hub

```
StationIO sub command help
	
hub:
	target: hub subcommand [one of 'collect' 'exec' 'fake-data' 'influx' 'ingest' 'collect']
Usage :
	StationIO hub <target>
```
### StationIO hub collect

```
StationIO hub sub command help
	
collect data from StationIO stations:
	--stationio-directory|--stationio-dir|--sd <stationio-directory>: stationIO install directory
	-x, --exclude <exclude>: station to exclude, repeatable
	--exclude-mesure|--xm <exclude-mesure>: yq path for mesure to exclude, repeatable
	--collect-dir <collect-dir>: collected data directory
Usage :
	StationIO _hub/collect [--stationio-directory <value>] [--exclude <value>] [--exclude-mesure <value>] [--collect-dir <value>]
```

### StationIO hub exec

```
StationIO hub sub command help
	
```

### StationIO hub fake-data

```
StationIO hub sub command help
	
I send an SOS to the world:
	--room <room>: specify room [default: ' main ']
	--sensor <sensor>: specify sensor [default: ' sht21 ']
	--mesure <mesure>: specify mesure [default: ' temperature ']
Usage :
	StationIO _hub/fake-data [--room <value>] [--sensor <value>] [--mesure <value>]
```

### StationIO hub influx

```
StationIO hub sub command help
	
execute influxDB cli command in the influxdb container:
Usage :
	StationIO _hub/influx
```

### StationIO hub ingest

```
StationIO hub sub command help
	
ingest data file:
	-f, --file <file>: file to load, repeatable
	-d, --directory|--dir <directory>: directory to import file from, repeatable
	--exclude-file|--xf <exclude-file>: filename to exclude, repeatable
	--archive <archive>: load files and folder from an archive, repeatable
	--archive-format <archive-format>: archive format [default: ' gz ']
	-c|--concat|--no-concat: concatenate files into a single file
		aliases: --concatenate,
	--exec|--no-exec: execute concat file write and remove file after, useful for directory, on by default (use --no-exec to turn it off)
Usage :
	StationIO _hub/ingest [--file <value>] [--directory <value>] [--exclude-file <value>] [--archive <value>] [--archive-format <value>] [--[no-]concat] [--[no-]exec]
```


## StationIO up

```
StationIO sub command help
	
I send an SOS to the world:
	--pull|--no-pull: pull latest images
	--build|--no-build: build
	--update|--no-update: pull and build
	--restart|--no-restart: restart stack if it is running, on by default (use --no-restart to turn it off)
	--grafana|--no-grafana: start grafana service, force --influxdb, on by default (use --no-grafana to turn it off)
	--influxdb|--no-influxdb: start influxdb service, on by default (use --no-influxdb to turn it off)
	-d|--detach|--no-detach: detach
Usage :
	StationIO up [--[no-]pull] [--[no-]build] [--[no-]update] [--[no-]restart] [--[no-]grafana] [--[no-]influxdb] [--[no-]detach]
```
