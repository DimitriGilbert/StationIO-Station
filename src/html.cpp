#include <Arduino.h>

#include "./Sensor.h"
#include "./Station.h"

String HtmlElt(
    String tagName, String inner, String attributes, bool selfClosing
) {
  return "<" + tagName + (attributes != "" ? " " + attributes : "") +
         (selfClosing ? "/>" : ">" + inner + "</" + tagName + ">");
}

String HtmlElt(String tagName, String inner, String attributes) {
  return HtmlElt(tagName, inner, attributes, false);
}

String HtmlElt(String tagName, String inner) {
  return HtmlElt(tagName, inner, "", false);
}

String HtmlAttribute(String name) { return name + " "; }
String HtmlAttribute(String name, String value, bool noSpace = false) {
  return name + "=\"" + value + "\"" + (!noSpace ? " " : "");
}
String HtmlClass(String value) { return HtmlAttribute("class", value, false); }
String HtmlId(String value) { return HtmlAttribute("id", value, false); }

String HtmlDiv(String inner, String attributes) {
  return HtmlElt("div", inner, attributes, false);
}
String HtmlDiv(String inner) { return HtmlElt("div", inner, "", false); }

String HtmlInput(String attributes) {
  return HtmlElt("input", "", attributes, true);
}
String HtmlInput() { return HtmlElt("input", "", "", true); }

String commonCss() {
  // commoncss-start
  return "body{background-color:#0d1117;color:#c9d1d9}.snMs-value,.snName{font-weight:700}.welcome{font-size:2rem;text-align:center}.welcome>.station-name{font-weight:700;font-style:italic}.st{margin-bottom:1.5rem}.sensor{background-color:var(--bs-gray-dark);border-radius:.5rem;padding-bottom:.5rem}.snName{font-size:1.5rem}.stSn{width:33%;margin:0 auto}.snMs{margin-bottom:.5rem}#station-chart{margin-bottom:1rem}";
  // commoncss-end
}

String commonJs() {
  // commonjs-start
  return "const ShrinkCount=[];var chartDataLength=Math.ceil(3600/updateTimer),dataShrinkBy=3,updateTimer=22;const chartLabels=[],chartDatas=[];var chartDisplayFrom=0,chartDisplayTo=0;function setUpdatable(){document.querySelectorAll(\".stSn\").forEach((t=>{t.firstElementChild.firstElementChild.onclick=t=>{let e=t.target.parentNode.parentNode;fetch(\"/?sensor=\"+e.getAttribute(\"sn-i\"),{headers:{Accept:\"text/html\"},method:\"GET\"}).then((t=>{t.text().then((t=>{e.innerHTML=t,setUpdatable()}))}))},t.firstElementChild.lastElementChild.childNodes.forEach(((t,e)=>{t.onclick=t=>{let a=t.target.parentNode.parentNode.parentNode.parentNode;fetch(\"/?sensor=\"+a.getAttribute(\"sn-i\")+\"&mesure=\"+e,{headers:{Accept:\"text/html\"},method:\"GET\"}).then((e=>{e.text().then((e=>{t.target.parentNode.innerHTML=e,setUpdatable()}))}))}}))}))}function mkDateLabel(){let t=new Date(Date.now());return(t.getHours()<10?\"0\":\"\")+t.getHours()+\":\"+(t.getMinutes()<10?\"0\":\"\")+t.getMinutes()+\":\"+(t.getSeconds()<10?\"0\":\"\")+t.getSeconds()}function getStationData(){return fetch(\"/?format=application/json\",{headers:{Accept:\"application/json\"},method:\"GET\"})}function existsSensorUtils(sensorName){let utils=!1;return eval(\"if(typeof \"+sensorName+'_utils !== \"undefined\"){utils='+sensorName+\"_utils}\"),utils}function formatSensorMesure(t,e,a){let s=existsSensorUtils(t);return s&&Object.hasOwnProperty.call(s,\"format_\"+e)?s[\"format_\"+e](a):a}function sensorMesureInChart(t,e){let a=existsSensorUtils(t);return!(a&&Object.hasOwnProperty.call(a,\"inChart\")&&!a.inChart(e))}function initChart(){let t=document.getElementsByTagName(\"body\")[0],e=document.createElement(\"script\");e.setAttribute(\"src\",\"https://cdn.jsdelivr.net/npm/chart.js\"),t.appendChild(e);let a=document.createElement(\"canvas\");a.setAttribute(\"id\",\"station-chart\"),a=t.appendChild(a);let s=document.createElement(\"form\");s.setAttribute(\"id\",\"chart-control\"),s.innerHTML='<div class=\"row\"><div class=\"col-6\"><div class=\"row\"><div class=\"form-group col-6\"><label for=\"chart-display-from\">From</label><input type=\"range\" class=\"form-range\" id=\"chart-display-from\" value=\"0\" min=\"0\"></div><div class=\"form-group col-6\"><label for=\"chart-display-to\">To (same as from for last) </label><input type=\"range\" class=\"form-range\" id=\"chart-display-to\" value=\"0\" min=\"0\"></div></div></div><div class=\"col-6\"><div class=\"form-group\"><label for=\"data-update-timer\">Update Every</label><input type=\"number\" class=\"form-control\" id=\"data-update-timer\" value=\"'+updateTimer+'\" min=\"5\" max=\"600\"></div></div><div class=\"col-12\"><button type=\"button\" class=\"btn btn-primary\" id=\"upd-cht-btn\">Update</button></div></div>',s=t.appendChild(document.createElement(\"div\")).appendChild(s),s.parentElement.classList.add(\"container\"),document.getElementById(\"chart-display-from\").addEventListener(\"change\",(t=>{let e=document.getElementById(\"chart-display-to\");e.setAttribute(\"min\",t.target.value),e.value<t.target.value&&(e.value=t.target.value)})),document.getElementById(\"upd-cht-btn\").addEventListener(\"click\",(t=>{updateTimer=parseInt(document.getElementById(\"data-update-timer\").value),chartDisplayFrom=document.getElementById(\"chart-display-from\").valueAsNumber,chartDisplayTo=document.getElementById(\"chart-display-to\").valueAsNumber,buildChart(Chart.getChart(\"station-chart\"))}));const r={labels:[],type:\"line\",data:{}};getStationData().then((t=>{t.json().then((t=>{let e=[],s=[],n={},l=\"left\";for(const a in t.sensors)if(Object.hasOwnProperty.call(t.sensors,a)){const r=t.sensors[a];for(const t in r)if(Object.hasOwnProperty.call(r,t)&&sensorMesureInChart(a,t)){let o=formatSensorMesure(a,t,r[t]);e.push({label:a+\"::\"+t,data:[o],yAxisID:t,pointRadius:1,pointHoverRadius:3}),chartDatas.push([o]),s.includes(t)||(s.push(t),n[t]={id:t,type:\"linear\",position:l},l=\"left\"===l?\"right\":\"left\")}}r.data.datasets=e,r.options={responsive:!0,scales:n};const o=new Chart(a,r);o.data.labels.push(mkDateLabel()),chartLabels.push(mkDateLabel()),o.update(),setTimeout((()=>{updateDatas(o,updateTimer)}),1e4)})).catch((t=>{console.error(t)}))})).catch((t=>{console.error(t)}))}function updateDatas(t){getStationData().then((e=>{e.json().then((e=>{let a=0;for(const t in e.sensors)if(Object.hasOwnProperty.call(e.sensors,t)){const s=e.sensors[t];for(const e in s)if(Object.hasOwnProperty.call(s,e)){const r=formatSensorMesure(t,e,s[e]);sensorMesureInChart(t,e)&&(chartDatas[a].push(r),a++),document.querySelectorAll(\".sensor.\"+t+\">.snMss>.snMs.\"+e+\">.snMs-value\")[0].innerHTML=r;let n=existsSensorUtils(t);if(n&&Object.hasOwnProperty.call(n,\"format_unit_\"+e)){let a=document.querySelectorAll(\".sensor.\"+t+\">.snMss>.snMs.\"+e+\">.snMs-unit\")[0];a.innerHTML=n[\"format_unit_\"+e](a.innerHTML)}}}chartLabels.push(mkDateLabel()),document.getElementById(\"chart-display-from\").setAttribute(\"max\",chartLabels.length);let s=document.getElementById(\"chart-display-to\");s.value==s.getAttribute(\"max\")&&(s.value=chartLabels.length),s.setAttribute(\"max\",chartLabels.length),0===chartDisplayTo&&(t=buildChart(t)),updateTimer>0&&setTimeout((()=>{updateDatas(t,updateTimer)}),1e3*updateTimer)})).catch((t=>{console.error(t)}))})).catch((t=>{console.error(t)}))}function buildChart(t){let e=chartDisplayTo<chartDisplayFrom?chartDisplayTo:chartLabels.length;t.data.labels=chartLabels.slice(chartDisplayFrom,e);for(let a=0;a<t.data.datasets.length;a++)t.data.datasets[a].data=chartDatas[a].slice(chartDisplayFrom,e);return t.update(\"active\"),t}setTimeout(setUpdatable,250),setTimeout((()=>{initChart()}),500);";
  // commonjs-end
}

String commonBody(String body) {
  return "<body>" + body + "</body><script src=\"/js\"></script>";
}

String commonHtmlHeader(String title) {
  return "<!DOCTYPE html><html>" +
         HtmlElt(
             "head",
             HtmlElt(
                 "meta", "",
                 HtmlAttribute(
                     "content", "width=device-width, initial-scale=1"
                 ),
                 true
             ) + HtmlElt("title", title) +
                 HtmlElt(
                     "link", "",
                     HtmlAttribute("rel", "stylesheet") +
                         HtmlAttribute(
                             "href",
                             "https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/"
                             "dist/css/bootstrap.min.css"
                         ) +
                         HtmlAttribute("crossorigin", "anonymous")
                 ) +
                 HtmlElt(
                     "link", "",
                     HtmlAttribute("rel", "stylesheet") +
                         HtmlAttribute("href", "/css")
                 )
         );
}
