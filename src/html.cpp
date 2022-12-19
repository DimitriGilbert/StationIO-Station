#include <Arduino.h>

#include "./Sensor.h"
#include "./Station.h"

String HtmlElt(
    String tagName, String inner, String attributes, bool selfClosing = false
) {
  return "<" + tagName + (attributes != "" ? " " + attributes : "") +
         (selfClosing ? "/>" : ">" + inner + "</" + tagName + ">");
}

String HtmlElt(String tagName, bool selfClosing = false) {
  return HtmlElt(tagName, "", "", selfClosing);
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
  return "body{background-color:rgb(13, 17, 23);color:rgb(201, 209, "
         "217)}.snMs-value,.snName{font-weight:700}."
         "welcome{font-size:2rem;text-align:center;}.welcome>.station-"
         "name{font-weight::bold;font-style:italic}.snName{font-"
         "size:1.5rem}.stSn{width:33%;margin:0 "
         "auto;}.snMs{margin-left:1rem}";
}

String commonJs() {
  return "function "
         "setUpdatable(){document.querySelectorAll(\".stSn\").forEach(e=>{e."
         "firstElementChild.firstElementChild.onclick=e=>{let "
         "t=e.target.parentNode.parentNode;fetch(\"/"
         "?sensor=\"+t.getAttribute(\"sn-i\"),{headers:{Accept:\"text/"
         "html\"},method:\"GET\"}).then(e=>{e.text().then(e=>{t.innerHTML=e,"
         "setUpdatable()})})};e.firstElementChild.lastElementChild.childNodes."
         "forEach((e,t)=>{e.onclick=e=>{fetch(\"/"
         "?sensor=\"+e.target.parentNode.parentNode.parentNode.parentNode."
         "getAttribute(\"sn-i\")+\"&mesure=\"+t,{headers:{Accept:\"text/"
         "html\"},method:\"GET\"}).then(t=>{t.text().then(t=>{e.target."
         "parentNode.innerHTML=t,setUpdatable()})})}})})}function "
         "mkDateLabel(){let e=new "
         "Date(Date.now());return(10>e.getHours()?\"0\":\"\")+e.getHours()+\":"
         "\"+(10>e.getMinutes()?\"0\":\"\")+e.getMinutes()+\":\"+(10>e."
         "getSeconds()?\"0\":\"\")+e.getSeconds()}function "
         "getStationData(){return "
         "fetch(\"/?format=application/json\",{headers:{Accept:\"application/"
         "json\"},method:\"GET\"})}function existsSensorUtils(sensorName){let "
         "utils=!1;return eval(\"if(typeof \"+sensorName+'_utils !== "
         "\"undefined\"){utils='+sensorName+\"_utils}\"),utils}function "
         "formatSensorMesure(e,t,n){let s=existsSensorUtils(e);return "
         "s&&Object.hasOwnProperty.call(s,\"format_\"+t)?s[\"format_\"+t](n):n}"
         "function sensorMesureInChart(e,t){let "
         "n=existsSensorUtils(e);return!(n&&Object.hasOwnProperty.call(n,"
         "\"inChart\")&&!n.inChart(t))}function initChart(){let "
         "e=document.getElementsByTagName(\"body\")[0],t=document."
         "createElement(\"script\");t.setAttribute(\"src\",\"https://"
         "cdn.jsdelivr.net/npm/chart.js\"),e.appendChild(t);let "
         "n=document.createElement(\"canvas\");n.setAttribute(\"id\",\"station-"
         "chart\"),n=e.appendChild(n);let "
         "s={labels:[],type:\"line\",data:{}};getStationData().then(e=>{s."
         "labels.push(mkDateLabel()),e.json().then(e=>{let "
         "t=[],a=[],r={},l=\"left\";for(let o in "
         "e.sensors)if(Object.hasOwnProperty.call(e.sensors,o)){let "
         "i=e.sensors[o];for(let d in "
         "i)Object.hasOwnProperty.call(i,d)&&sensorMesureInChart(o,d)&&(t.push("
         "{label:o+\"::\"+d,data:[formatSensorMesure(o,d,i[d]),],yAxisID:d,"
         "pointRadius:1,pointHoverRadius:3}),a.includes(d)||(a.push(d),r[d]={"
         "id:d,type:\"linear\",position:l},l=\"left\"===l?\"right\":\"left\"))}"
         "s.data.datasets=t,s.options={responsive:!0,scales:r};let h=new "
         "Chart(n,s);setTimeout(()=>{updateChart(h,22)},1e4)})})}function "
         "updateChart(e,t){getStationData().then(n=>{e.data.labels.push("
         "mkDateLabel()),n.json().then(n=>{let s=0;for(let a in "
         "n.sensors)if(Object.hasOwnProperty.call(n.sensors,a)){let "
         "r=n.sensors[a];for(let l in "
         "r)if(Object.hasOwnProperty.call(r,l)){let "
         "o=formatSensorMesure(a,l,r[l]);sensorMesureInChart(a,l)&&(e.data."
         "datasets[s].data.push(o),s++),document.querySelectorAll(\".sensor.\"+"
         "a+\">.snMss>.snMs.\"+l+\">.snMs-value\")[0].innerHTML=o}}e.update("
         "\"active\"),t>0&&setTimeout(()=>{updateChart(e,t)},1e3*t)})})}"
         "setTimeout(setUpdatable,250),setTimeout(()=>{initChart()},500);";
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
                             "https://cdn.jsdelivr.net/npm/purecss@3.0.0/"
                             "build/pure-min.css"
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
