# Bash utility scripts

I created a bunch of bash script to help with station management build and upload.

## buildFS

Builds the station filesystem.

```bash
Usage :
utils/buildFS [--js <value>] [--css <value>] [--directory <value>] [--file <value>] [--file-no-gz <value>] [--node <value>] [--pio <value>] [--environement <value>] [--upload <value>] [--[no-]dry]
 
 -j, --js <js>: path to js file, ':' to separate target directory (/assets/js by default), repeatable
 -c, --css <css>: path to css file, ':' to separate target directory (/assets/css by default), repeatable
 -d, --directory <directory>: additional directory path, all files will be gzipped, ':' to separate target directory (/ by default), repeatable
 -f, --file <file>: file to add, will be gzipped, ':' to separate target directory (/ by default), repeatable
 -F, --file-no-gz <file-no-gz>: add a file without gzip, ':' to separate target directory (/ by default), repeatable
 --node <node>: node executable [default: ' node ']
 --pio <pio>: pio executable [default: ' $HOME/.platformio/penv/bin/pio ']
 -e, --environement <environement>: build environment [default: ' nodemcuv2 ']
 -u, --upload <upload>: station to upload to, will set --environment
 -D|--dry|--no-dry: do not upload (just build for a station)
```

## createStation

Helps you create a station.

```bash
Usage :
 ./utils/createStation <name> [--ip <value>] [--board <value>] [--[no-]config] [--[no-]edit]  
 
 name: station name
 --ip <ip>: station ip
 -b, --board <board>: PIO build environement name
 --config|--no-config: open configurator, on by default (use --no-config to turn it off)
 --edit|--no-edit: open editor, on by default (use --no-edit to turn it off)
```

## createStation-cli

Create a station just from cli.

```bash
Usage :
 ./utils/createStation-cli <name> [--sensor <value>] [--timer <value>] [--endpoint <value>] [--wifi-ssid <value>] [--wifi-pass <value>] [--include <value>] [--declare <value>] [--setup <value>] [--loop <value>] [--node <value>] [--file <value>]

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
```

## setStation

set station config

```bash
Usage :
 ./utils/setStation <name> [--sensor <value>] [--ip <value>] [--board <value>] [--data-file <value>] [--data-directory <value>] [--js <value>] [--css <value>]

 name: station name
 -s, --sensor <sensor>: sensor to add, repeatable
 -i, --ip <ip>: ip address [default: ' 0 ']
 --board <board>: pio env [default: ' nodemcuv2 ']
 --data-file <data-file>: data-file to add, repeatable
 --data-directory <data-directory>: data-directory to add, repeatable
 --js <js>: js to add, repeatable
 --css <css>: css to add, repeatable
```

## uploadStation

upload compiled sketch or filesystem to a station

```bash
Usage :
 ./utils/uploadStation <name> [--ip <value>] [--port <value>] [--board <value>] [--pio <value>] [--[no-]ota] [--[no-]fs]

 name: station name
 --ip <ip>: station ip
 --port <port>: host port upload flags, avoid random for firewalls [default: ' 46667 ']
 -b, --board <board>: PIO build environement name
 --pio <pio>: pio executable [default: ' $HOME/.platformio/penv/bin/pio ']
 --ota|--no-ota: OTA update, on by default (use --no-ota to turn it off)
 --fs|--no-fs: upload fs
```
