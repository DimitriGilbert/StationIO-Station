const WRPTInccludeTpl = `// WIFI repeater
#include <lwip/dns.h>
#include <lwip/napt.h>`;
const WRPTSetupTpl = `// WIFI Repeater
  // setting dns server
  station.wifi.softAPDhcpServer().setDns(station.wifi.dnsIP(0));
  // configuring AP
  station.wifi.softAPConfig(
      IPAddress(<%= it.wifirepIP.split(".").join(",") %>), IPAddress(<%= it.wifirepIP.split(".").join(",") %>),
      IPAddress(<%= it.wifirepNetMask.split(".").join(",") %>)
  );
  // starting AP
  station.wifi.softAP("<%= it.wifirepAP%>", "<%= it.wifirepPass%>");
  // starting NAPT
  err_t ret = ip_napt_init(IP_NAPT_MAX, IP_PORTMAP_MAX);
  if (ret == ERR_OK) {
    ret = ip_napt_enable_no(SOFTAP_IF, 1);
    if (ret == ERR_OK) {
      station.log("WiFi extender enabled:");
      station.logt("<%= it.wifirepAP%>");
      station.logt("<%= it.wifirepPass%>");
      
      station.log("");
      station.log("Free heap :");
      station.logt(String(ESP.getFreeHeap()));
    }
  }
`;
const WRPTFrmTpl = `<div id="wifi-repeater">
  <h2 class="text-center">Wifi Repeater</h2>
  <div class="row">
    <div class="col">
      <label for="wifi-repeater-ssid">Access point SSID</label>
      <input type="text" class="form-control" name="wifi-repeater-ssid" id="wifi-repeater-ssid">
    </div>
    <div class="col">
      <label for="wifi-repeater-password">Access point password</label>
      <input type="text" class="form-control" name="wifi-repeater-password" id="wifi-repeater-password">
    </div>
  </div>
  <div class="row">
    <div class="col">
      <label for="wifi-repeater-ip">Access Point/gateway IP Address</label>
      <input type="text" class="form-control" name="wifi-repeater-ip" id="wifi-repeater-ip" value="192.168.100.1">
    </div>
    <div class="col">
      <label for="wifi-repeater-network-mask">subnet mask</label>
      <input type="text" class="form-control" name="wifi-repeater-network-mask" id="wifi-repeater-network-mask" value="255.255.255.0">
    </div>
  </div>
</div>`;

function WRPTBuildData(frmData) {
  return {
    wifirepIP: frmData.get("wifi-repeater-ip"),
    wifirepNetMask: frmData.get("wifi-repeater-network-mask"),
    wifirepAP: frmData.get("wifi-repeater-ssid")!==""?frmData.get("wifi-repeater-ssid"):`${frmData.get("station-name")}_AP`,
    wifirepPass: frmData.get("wifi-repeater-password"),
  };
}

if (StationIOPlugins !== undefined) {
  StationIOPlugins.include.push((frmData) => {
    return WRPTInccludeTpl;
  });
  StationIOPlugins.setup.push((frmData) => {
    return eta_.renderString(WRPTSetupTpl, WRPTBuildData(frmData));
  });
  document
    .getElementById("wifi-config")
    .appendChild(htmlToElements(WRPTFrmTpl));
}    
