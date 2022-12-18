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
  return HtmlElt(
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
             "style",
             "body{background-color:rgb(13, 17, 23);color:rgb(201, 209, "
             "217)}.sensor-mesure-value,.sensor-name{font-weight:700}."
             "welcome{font-size:2rem;text-align:center;}.welcome>.station-"
             "name{font-weight::bold;font-style:italic}.sensor-name{font-"
             "size:1.5rem}.station-sensor{width:33%;margin:0 "
             "auto;}.sensor-mesure{margin-left:1rem}",
             ""
         );
}

String commonJs() {
  return "<script>function "
         "setUpdatable(){document.querySelectorAll(\".station-sensor\")."
         "forEach(e=>{e.firstElementChild.firstElementChild.onclick=e=>{let "
         "t=e.target.parentNode.parentNode;fetch(\"/"
         "?sensor=\"+t.getAttribute(\"sensor-index\"),{headers:{Accept:\"text/"
         "html\"},method:\"GET\"}).then(e=>{e.text().then(e=>{t.innerHTML=e,"
         "setUpdatable()})})};e.firstElementChild.lastElementChild.childNodes."
         "forEach((e,t)=>{e.onclick=e=>{fetch(\"/"
         "?sensor=\"+e.target.parentNode.parentNode.parentNode.parentNode."
         "getAttribute(\"sensor-index\")+\"&mesure=\"+t,{headers:{Accept:"
         "\"text/"
         "html\"},method:\"GET\"}).then(t=>{t.text().then(t=>{e.target."
         "parentNode.innerHTML=t,setUpdatable()})})}})})}function "
         "getStationData(){return "
         "fetch(\"/?format=application/json\",{headers:{Accept:\"application/"
         "json\"},method:\"GET\"})}function initChart(){let "
         "e=document.getElementsByTagName(\"body\")[0],t=document."
         "createElement(\"script\");t.setAttribute(\"src\",\"https://"
         "cdn.jsdelivr.net/npm/chart.js\"),e.appendChild(t);let "
         "a=document.createElement(\"canvas\");a.setAttribute(\"id\",\"station-"
         "chart\"),a=e.appendChild(a);let "
         "n={labels:[],type:\"line\",data:{}};getStationData().then(e=>{let "
         "t=new "
         "Date(Date.now());n.labels.push(t.getHours()+\":\"+t.getMinutes()+\":"
         "\"+t.getSeconds()),e.json().then(e=>{let t=[];for(let s in "
         "e.sensors)if(Object.hasOwnProperty.call(e.sensors,s)){let "
         "r=e.sensors[s];for(let l in "
         "r)if(Object.hasOwnProperty.call(r,l)){let "
         "o=r[l];t.push({label:`${s}::${l}`,data:[o],yAxisID:l})}}n.data."
         "datasets=t;let i=new "
         "Chart(a,n);setTimeout(()=>{updateChart(i,22)},1e4)}).catch(e=>{"
         "console.error(e)})}).catch(e=>{console.error(e)})}function "
         "updateChart(e,t){getStationData().then(a=>{let n=new "
         "Date(Date.now());e.data.labels.push(n.getHours()+\":\"+n.getMinutes()"
         "+\":\"+n.getSeconds()),a.json().then(a=>{let n=0;for(let s in "
         "a.sensors)if(Object.hasOwnProperty.call(a.sensors,s)){let "
         "r=a.sensors[s];for(let l in "
         "r){if(Object.hasOwnProperty.call(r,l)){let "
         "o=r[l];e.data.datasets[n].data.push(o),document.querySelectorAll(`."
         "sensor.${s}>.sensor-mesures>.sensor-mesure.${l}>.sensor-mesure-value`"
         ")[0].innerHTML=o}n++}}e.update(\"active\"),t>0&&setTimeout(()=>{"
         "updateChart(e,t)},1e3*t)}).catch(e=>{console.error(e)})}).catch(e=>{"
         "console.error(e)})}setTimeout(setUpdatable,250),setTimeout(()=>{"
         "initChart()},1500);</script>";
}

String commonBody(String body) { return "<body>" + body + "</body>"; }

String commonHtmlHeader(String title) {
  return "<!DOCTYPE html><html>" +
         HtmlElt(
             "head", HtmlElt(
                         "meta", "",
                         HtmlAttribute(
                             "content", "width=device-width, initial-scale=1"
                         ),
                         true
                     ) + HtmlElt("title", title) +
                         commonCss()
         );
}
