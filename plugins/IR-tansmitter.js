const IRTInccludeTpl = `#include <IRremote.h>`;
const IRTDeclarationTpl = `uint8_t IRPin = <%= it.irpin %>;
EspStation::StationWebCallbackInfo_t IRSenderEndpoint = {
    "/<%= it.irendpoint!==undefined?it.irendpoint:"ir-remote" %>", "<%= it.irLogin!==undefined?it.irLogin:"" %>", "<%= it.irPassword!==undefined?it.irPassword:"" %>",
    [](BaseStation* station, AsyncWebServerRequest* request) {
      <%~ it.irsendcode %>
    }};
<% if (it.ir21button) { %>// 21 buttons remote
u_int32_t IRRemote21[21] = {
  0xFFA25D,
  0xFF629D,
  0xFFE21D,
  0xFF22DD,
  0xFF02FD,
  0xFFC23D,
  0xFFE01F,
  0xFFA857,
  0xFF906F,
  0xFF6897,
  0xFF9867,
  0xFFB04F,
  0xFF30CF,
  0xFF18E7,
  0xFF7A85,
  0xFF10EF,
  0xFF38C7,
  0xFF5AA5,
  0xFF42BD,
  0xFF4AB5,
  0xFF52AD
};
EspStation::StationWebCallbackInfo_t IR21RemoteEndpoint = {
    "/<%= it.irendpoint!==undefined?it.irendpoint:"ir-remote" %>/remote/21", "<%= it.irLogin!==undefined?it.irLogin:"" %>", "<%= it.irPassword!==undefined?it.irPassword:"" %>",
    [](BaseStation* station, AsyncWebServerRequest* request) {
      u_int32_t code = 0;
      if (request->hasParam("send")) {
        u_int8_t btn = request->getParam("send")->value().toInt();
        if (btn < 21) {
          code = Remote21[btn];
        }
      }
      if (code != 0) {
        IrSender.sendNEC(code, 32);
      }
      request->send(200, "text/plain", String(code));
    }};
<% } %>
<% if (it.ir24button) { %>// 24 buttons remote
u_int32_t IRRemote24[24] = {
  0xF700FF,
  0xF7807F,
  0xF740BF,
  0xF7C03F,
  0xF720DF,
  0xF7A05F,
  0xF7609F,
  0xF7E01F,
  0xF710EF,
  0xF7906F,
  0xF750AF,
  0xF7D02F,
  0xF730CF,
  0xF7B04F,
  0xF7708F,
  0xF7F00F,
  0xF708F7,
  0xF78877,
  0xF748B7,
  0xF7C837,
  0xF728D7,
  0xF7A857,
  0xF76897,
  0xF7E817
};
EspStation::StationWebCallbackInfo_t IR24RemoteEndpoint = {
    "/<%= it.irendpoint!==undefined?it.irendpoint:"ir-remote" %>/remote/24", "<%= it.irLogin!==undefined?it.irLogin:"" %>", "<%= it.irPassword!==undefined?it.irPassword:"" %>",
    [](BaseStation* station, AsyncWebServerRequest* request) {
      u_int32_t code = 0;
      if (request->hasParam("send")) {
        u_int8_t btn = request->getParam("send")->value().toInt();
        if (btn < 24) {
          code = Remote24[btn];
        }
      }
      if (code != 0) {
        IrSender.sendNEC(code, 32);
      }
      request->send(200, "text/plain", String(code));
    }};
<% } %>
<% if (it.ir44button) { %>// 44 buttons remote
u_int32_t IRRemote44[44] = {
  0xFF3AC5,
  0xFFBA45,
  0xFF827D,
  0xFF02FD,
  0xFF1AE5,
  0xFF9A65,
  0xFFA25D,
  0xFF22DD,
  0xFF2AD5,
  0xFFAA55,
  0xFF926D,
  0xFF12ED,
  0xFF0AF5,
  0xFF8A75,
  0xFFB24D,
  0xFF32CD,
  0xFF38C7,
  0xFFB847,
  0xFF7887,
  0xFFF807,
  0xFF18E7,
  0xFF9867,
  0xFF58A7,
  0xFFD827,
  0xFF28D7,
  0xFFA857,
  0xFF6897,
  0xFFE817,
  0xFF08F7,
  0xFF8877,
  0xFF48B7,
  0xFFC837,
  0xFF30CF,
  0xFFB04F,
  0xFF708F,
  0xFFF00F,
  0xFF10EF,
  0xFF906F,
  0xFF50AF,
  0xFFD02F,
  0xFF20DF,
  0xFFA05F,
  0xFF609F,
  0xFFE01F
};
EspStation::StationWebCallbackInfo_t IR44RemoteEndpoint = {
    "/<%= it.irendpoint!==undefined?it.irendpoint:"ir-remote" %>/remote/44", "<%= it.irLogin!==undefined?it.irLogin:"" %>", "<%= it.irPassword!==undefined?it.irPassword:"" %>",
    [](BaseStation* station, AsyncWebServerRequest* request) {
      u_int32_t code = 0;
      if (request->hasParam("send")) {
        u_int8_t btn = request->getParam("send")->value().toInt();
        if (btn < 44) {
          code = Remote44[btn];
        }
      }
      if (code != 0) {
        IrSender.sendNEC(code, 32);
      }
      request->send(200, "text/plain", String(code));
    }};
<% } %>
`;
const IRTSetupTpl = ` // IR Remote
  IrSender.begin(IRPin);
  station.addEndpoint(IRSenderEndpoint);
<% if (it.ir21button) { %>
  station.addEndpoint(IR21RemoteEndpoint);
<% } if (it.ir24button) { %>
  station.addEndpoint(IR24RemoteEndpoint);
<% } if (it.ir44button) { %>
  station.addEndpoint(IR44RemoteEndpoint);
<% } %>
`;
const IRTFrmTpl = `<div id="ir-remote">
  <h2 class="text-center">IR Remote</h2>
  <div class="row">
    <div class="col">
      <label for="ir-remote-pin">IR pin</label>
      <input type="text" class="form-control" name="ir-remote-pin" id="ir-remote-pin">
    </div>
    <div class="col">
      <label for="ir-remote-endpoint">IR endpoint</label>
      <input type="text" class="form-control" name="ir-remote-endpoint" id="ir-remote-endpoint" value="ir-remote">
    </div>
  </div>
  <div class="row">
    <div class="col">
      <label for="ir-endpoint-login">IR endpoint login</label>
      <input type="text" class="form-control" name="ir-endpoint-login" id="ir-endpoint-login">
    </div>
    <div class="col">
      <label for="ir-endpoint-password">IR endpoint password</label>
      <input type="text" class="form-control" name="ir-endpoint-password" id="ir-endpoint-password">
    </div>
  </div>
  <div class="row">
    <div class="col">
      <label for="ir-remote-send-fn">IR endpoint code</label>
      <code-input type="text" class="form-control" name="ir-remote-send-fn" id="ir-remote-send-fn" value=""></code-input>
    </div>
  </div>
  <div class="row">
    <div class="col">
      <input type="checkbox" class="form-check-input" name="ir-21-button" id="ir-21-button" />
      <label for="ir-21-button" class="form-check-label">21 buttons remote support</label>
    </div>
    <div class="col">
      <input type="checkbox" class="form-check-input" name="ir-24-button" id="ir-24-button" />
      <label for="ir-24-button" class="form-check-label">24 buttons remote support</label>
    </div>
    <div class="col">
      <input type="checkbox" class="form-check-input" name="ir-44-button" id="ir-44-button" />
      <label for="ir-44-button" class="form-check-label">44 buttons remote support</label>
    </div>
  </div>
</div>`;

function IRTBuildData(frmData) {
  return {
    irpin: frmData.get("ir-remote-pin"),
    irendpoint: frmData.get("ir-remote-endpoint"),
    irLogin: frmData.get("ir-endpoint-login"),
    irPassword: frmData.get("ir-endpoint-password"),
    ir21button: frmData.get("ir-21-button") === "on",
    ir24button: frmData.get("ir-24-button") === "on",
    ir44button: frmData.get("ir-44-button") === "on",
    irsendcode: frmData.get("ir-remote-send-fn")
  };
}

if (StationIOPlugins !== undefined) {
  StationIOPlugins.include.push((frmData) => {
    return IRTInccludeTpl;
  });
  StationIOPlugins.declaration.push((frmData) => {
    return eta.render(IRTDeclarationTpl, IRTBuildData(frmData));
  });
  StationIOPlugins.setup.push((frmData) => {
    return eta.render(IRTSetupTpl, IRTBuildData(frmData));
  });
  document
    .getElementById("timers-config")
    .parentElement.appendChild(htmlToElements(IRTFrmTpl));
  document.getElementById("ir-remote-send-fn").value = `int code = -1;
if (request->hasParam("code")) {
  code = request->getParam("code")->value().toInt();
}

if (code != -1) {
  IrSender.sendNEC(code, 32);
}
request->send(200, "text/plain", String(code));`;
}
