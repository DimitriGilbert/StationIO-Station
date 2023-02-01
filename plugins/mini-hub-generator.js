const minihubStationDefault = {
  name: "station name",
  ip: "station's ip",
};
const miniHubConfig = {
  stations: [],
  endpointBase: "hub",
  login: "",
  password: "",
};

const miniHubIncludeTpl = `
// mini hub
#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include <HTTPClient.h>
#endif
#include <WiFiClient.h>

`;

const miniHubDeclarationTpl = `
// mini hub
HTTPClient httpClient;
WiFiClient wifiClient;

// configuration
String <%= it.endpointBase %>EndpointBase = "/<%= it.endpointBase %>";
u_int8_t <%= it.endpointBase %>StationsCount = <%= it.stations.length %>;
String *<%= it.endpointBase %>StationNames[<%= it.stations.length %>] = {<%= it.stations.map(s => '"' + s.name + '"').join(',') %>};
String *<%= it.endpointBase %>StationIps[<%= it.stations.length %>] = {<%= it.stations.map(s => '"' + s.ip + '"').join(',') %>};

// helpers
String <%= it.endpointBase %>GetStationIp(String stationName) {
  for (int i = 0; i < <%= it.endpointBase %>StationsCount; i++) {
    if (<%= it.endpointBase %>StationNames[i] == stationName) {
      return <%= it.endpointBase %>StationIps[i];
    }
  }
  return "";
}
String <%= it.endpointBase %>GetStationData(String stationName, String format, u_int8_t &code) {
  String data = "{error:\"not begun\"}";
  String stationIp = <%= it.endpointBase %>GetStationIp(stationName);
  if (stationIp != "") {
    data = "{error:\"station "+stationName+" not found\"}";
    code = 404;
  }
  else {
    if (httpClient.begin(
            wifiClient,
            "http://" + stationIp + "/?format=" + format
        )) {
      httpClient.addHeader("Accept", format);
      code = httpClient.GET();
      if (code >= 200 && code < 300) {
        data = httpClient.getString();
      }
    }
  }
  return data;
}

// endpoints
EspStation::StationWebCallbackInfo_t <%= it.endpointBase %>StationsListEndpoint = {"/<%= it.endpointBase %>/stations", "<%= it.login %>", "<%= it.password %>", 
  [](BaseStation* station, AsyncWebServerRequest* request) {
    String out = "";
    for (int i = 0; i < <%= it.endpointBase %>StationsCount; i++) {
      out += <%= it.endpointBase %>StationNames[i]+",";
    }
    request->send(200, "application/json", "["+out+"]");
  }};
EspStation::StationWebCallbackInfo_t <%= it.endpointBase %>ProxyEndpoint = {"/<%= it.endpointBase %>/proxy", "<%= it.login %>", "<%= it.password %>", 
  [](BaseStation* station, AsyncWebServerRequest* request) {
    int code = 500;
    String format = request->getHeader("Accept")->value();
    if (request->hasParam("format")) {
      format = request->getParam("format")->value();
    }
    if(format == "") {
      format = "application/json";
    }
    String data = <%= it.endpointBase %>GetStationData(request->getParam("station")->value(), format, code)
    request->send(code, format, data);
  }};
EspStation::StationWebCallbackInfo_t <%= it.endpointBase %>HubEndpoint = {"/<%= it.endpointBase %>/hub", "<%= it.login %>", "<%= it.password %>", 
  [](BaseStation* station, AsyncWebServerRequest* request) {
    String out = "TODO";
    request->send(200, "text/html", out);
  }};
EspStation::StationWebCallbackInfo_t <%= it.endpointBase %>HubJsEndpoint = {"/<%= it.endpointBase %>/hub-js", "<%= it.login %>", "<%= it.password %>", 
  [](BaseStation* station, AsyncWebServerRequest* request) {
    String out = "// TODO";
    request->send(200, "text/javascript", out);
  }};
EspStation::StationWebCallbackInfo_t <%= it.endpointBase %>HubCssEndpoint = {"/<%= it.endpointBase %>/hub-css", "<%= it.login %>", "<%= it.password %>", 
  [](BaseStation* station, AsyncWebServerRequest* request) {
    String out = "// TODO";
    request->send(200, "text/css", out);
  }};
`;

const miniHubSetupTpl = ` // mini hub
  station.addEndpoint(<%= it.endpointBase %>StationsListEndpoint);
  station.addEndpoint(<%= it.endpointBase %>ProxyEndpoint);
  station.addEndpoint(<%= it.endpointBase %>HubEndpoint);
  station.addEndpoint(<%= it.endpointBase %>HubJsEndpoint);
  station.addEndpoint(<%= it.endpointBase %>HubCssEndpoint);
  
`;

const miniHubFormTpl = `<div id="mini-hub-config" class="row">
  <h2 class="text-center">Mini Hub</h2>
  <div>
    <label for="mini-hub-endpoint-base">Endpoint Base</label>
    <input type="text" class="form-control" name="mini-hub-endpoint-base" id="mini-hub-endpoint-base">
  </div>
  <div class="col">
    <label for="mini-hub-login">login</label>
    <input type="text" class="form-control" name="mini-hub-login" id="mini-hub-login">
  </div>
  <div class="col">
    <label for="mini-hub-password">password</label>
    <input type="text" class="form-control" name="mini-hub-password" id="mini-hub-password">
  </div>
  <div id="mini-hub-stations">
    <input type="hidden" name="mini-hub-stations-count" id="mini-hub-stations-count" value="0">
    <button type="button" class="btn btn-primary" id="mini-hub-stations-add" onclick="miniHubAddStation()">Add Station</button>
  </div>
</div>`;

const miniHubStationFrmTpl = `<div class="row">
  <div class="col">
    <label for="mini-hub-station-name-<%= it.id %>">station name <%= it.id %></label>
    <input type="text" class="form-control" name="mini-hub-station-name-<%= it.id %>" id="mini-hub-station-name-<%= it.id %>">
  </div>
  <div class="col">
    <label for="mini-hub-station-ip-<%= it.id %>">station ip <%= it.id %></label>
    <input type="text" class="form-control" name="mini-hub-station-ip-<%= it.id %>" id="mini-hub-station-ip-<%= it.id %>">
  </div>
</div>`;

function miniHubAddStation() {
  document.getElementById("mini-hub-stations-count").value++;
  let id = document.getElementById("mini-hub-stations-count").value;
  let stationFrm = eta.render(miniHubStationFrmTpl, { id: id });
  document
    .getElementById("mini-hub-stations")
    .appendChild(htmlToElements(stationFrm));
}

function miniHubBuildData(frmData) {
  // TODO : get stations names and ips from form and buid stations object
  let stations = [];

  let cfg = {
    ...miniHubConfig,
    ...{
      endpointBase: frmData.get("mini-hub-endpoint-base"),
      login: frmData.get("mini-hub-login"),
      password: frmData.get("mini-hub-password"),
      stations: stations,
    },
  };
  return cfg;
}

if (StationIOPlugins !== undefined) {
  StationIOPlugins.include.push((frmData) => {
    return miniHubIncludeTpl;
  });
  StationIOPlugins.declaration.push((frmData) => {
    return eta.render(miniHubDeclarationTpl, miniHubBuildData(frmData));
  });
  StationIOPlugins.setup.push((frmData) => {
    return eta.render(miniHubSetupTpl, miniHubBuildData(frmData));
  });
  document
    .getElementById("endpoints-config")
    .parentElement.appendChild(htmlToElements(miniHubFormTpl));
}
