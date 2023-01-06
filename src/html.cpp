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
  return "body{background-color:#0d1117;color:#c9d1d9}.snMs-value,.snName{font-weight:700}.welcome{font-size:2rem;text-align:center}.welcome>.station-name{font-weight:700;font-style:italic}.snName{font-size:1.5rem}.stSn{width:33%;margin:0 auto}.snMs{margin-left:1rem}";
  // commoncss-end
}

String commonJs() {
  // commonjs-start
  return "function setUpdatable(){document.querySelectorAll(\".stSn\").forEach((t=>{t.firstElementChild.firstElementChild.onclick=t=>{let e=t.target.parentNode.parentNode;fetch(\"/?sensor=\"+e.getAttribute(\"sn-i\"),{headers:{Accept:\"text/html\"},method:\"GET\"}).then((t=>{t.text().then((t=>{e.innerHTML=t,setUpdatable()}))}))},t.firstElementChild.lastElementChild.childNodes.forEach(((t,e)=>{t.onclick=t=>{let s=t.target.parentNode.parentNode.parentNode.parentNode;fetch(\"/?sensor=\"+s.getAttribute(\"sn-i\")+\"&mesure=\"+e,{headers:{Accept:\"text/html\"},method:\"GET\"}).then((e=>{e.text().then((e=>{t.target.parentNode.innerHTML=e,setUpdatable()}))}))}}))}))}function mkDateLabel(){let t=new Date(Date.now());return(t.getHours()<10?\"0\":\"\")+t.getHours()+\":\"+(t.getMinutes()<10?\"0\":\"\")+t.getMinutes()+\":\"+(t.getSeconds()<10?\"0\":\"\")+t.getSeconds()}function getStationData(){return fetch(\"/?format=application/json\",{headers:{Accept:\"application/json\"},method:\"GET\"})}function existsSensorUtils(sensorName){let utils=!1;return eval(\"if(typeof \"+sensorName+'_utils !== \"undefined\"){utils='+sensorName+\"_utils}\"),utils}function formatSensorMesure(t,e,s){let n=existsSensorUtils(t);return n&&Object.hasOwnProperty.call(n,\"format_\"+e)?n[\"format_\"+e](s):s}function sensorMesureInChart(t,e){let s=existsSensorUtils(t);return!(s&&Object.hasOwnProperty.call(s,\"inChart\")&&!s.inChart(e))}function initChart(){let t=document.getElementsByTagName(\"body\")[0],e=document.createElement(\"script\");e.setAttribute(\"src\",\"https://cdn.jsdelivr.net/npm/chart.js\"),t.appendChild(e);let s=document.createElement(\"canvas\");s.setAttribute(\"id\",\"station-chart\"),s=t.appendChild(s);const n={labels:[],type:\"line\",data:{}};getStationData().then((t=>{n.labels.push(mkDateLabel()),t.json().then((t=>{let e=[],a=[],r={},o=\"left\";for(const s in t.sensors)if(Object.hasOwnProperty.call(t.sensors,s)){const n=t.sensors[s];for(const t in n)Object.hasOwnProperty.call(n,t)&&sensorMesureInChart(s,t)&&(e.push({label:s+\"::\"+t,data:[formatSensorMesure(s,t,n[t])],yAxisID:t,pointRadius:1,pointHoverRadius:3}),a.includes(t)||(a.push(t),r[t]={id:t,type:\"linear\",position:o},o=\"left\"===o?\"right\":\"left\"))}n.data.datasets=e,n.options={responsive:!0,scales:r};const i=new Chart(s,n);setTimeout((()=>{updateChart(i,22)}),1e4)}))}))}function updateChart(t,e){getStationData().then((s=>{t.data.labels.push(mkDateLabel()),s.json().then((s=>{let n=0;for(const e in s.sensors)if(Object.hasOwnProperty.call(s.sensors,e)){const a=s.sensors[e];for(const s in a)if(Object.hasOwnProperty.call(a,s)){const r=formatSensorMesure(e,s,a[s]);sensorMesureInChart(e,s)&&(t.data.datasets[n].data.push(r),n++),document.querySelectorAll(\".sensor.\"+e+\">.snMss>.snMs.\"+s+\">.snMs-value\")[0].innerHTML=r}}t.update(\"active\"),e>0&&setTimeout((()=>{updateChart(t,e)}),1e3*e)}))}))}setTimeout(setUpdatable,250),setTimeout((()=>{initChart()}),500);";
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
