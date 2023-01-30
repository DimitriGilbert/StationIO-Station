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
  return "body{background-color:#0d1117;color:#c9d1d9}.snMs-value,.snName{font-weight:700}.welcome{font-size:2rem;text-align:center}.welcome>.st-n{font-weight:700;font-style:italic}.st{margin-bottom:1.5rem}.sensor{background-color:var(--bs-gray-dark);border-radius:.5rem;padding-bottom:.5rem}.snName{font-size:1.5rem}.stSn{width:33%;margin:0 auto}.snMs{margin-bottom:.5rem}#station-chart{margin-bottom:1rem}";
  // commoncss-end
}

String commonJs() {
  // commonjs-start
  return "var updateTimer=22;const chartLabels=[],chartDatas=[];var chtDFrom=0,chtDTo=0,dqsa=t=>document.querySelectorAll(t),dcrel=t=>document.createElement(t),dgeli=t=>document.getElementById(t),ohop=(t,e)=>Object.prototype.hasOwnProperty.call(t,e);function setUpdatable(){dqsa(\".stSn\").forEach((t=>{t.firstElementChild.firstElementChild.onclick=t=>{let e=t.target.parentNode.parentNode;fetch(\"/?sensor=\"+e.getAttribute(\"sn-i\"),{headers:{Accept:\"text/html\"},method:\"GET\"}).then((t=>{t.text().then((t=>{e.innerHTML=t,setUpdatable()}))}))},t.firstElementChild.lastElementChild.childNodes.forEach(((t,e)=>{t.onclick=t=>{let a=t.target.parentNode.parentNode.parentNode.parentNode;fetch(\"/?sensor=\"+a.getAttribute(\"sn-i\")+\"&mesure=\"+e,{headers:{Accept:\"text/html\"},method:\"GET\"}).then((e=>{e.text().then((e=>{t.target.parentNode.innerHTML=e,setUpdatable()}))}))}}))}))}function mkDLabel(){let t=new Date(Date.now());return(t.getHours()<10?\"0\":\"\")+t.getHours()+\":\"+(t.getMinutes()<10?\"0\":\"\")+t.getMinutes()+\":\"+(t.getSeconds()<10?\"0\":\"\")+t.getSeconds()}function getStationData(){return fetch(\"/?format=application/json\",{headers:{Accept:\"application/json\"},method:\"GET\"})}function exstSenUtils(snNm){let u=!1;return eval(\"if(typeof \"+snNm+'_utils !== \"undefined\"){u='+snNm+\"_utils}\"),u}function frmtSnMes(t,e,a){let n=exstSenUtils(t);return n&&ohop(n,\"format_\"+e)?n[\"format_\"+e](a):a}function mesInCht(t,e){let a=exstSenUtils(t);return!(a&&ohop(a,\"inChart\")&&!a.inChart(e))}function initChart(){let t=document.getElementsByTagName(\"body\")[0],e=dcrel(\"script\");e.setAttribute(\"src\",\"https://cdn.jsdelivr.net/npm/chart.js\"),t.appendChild(e);let a=dcrel(\"canvas\");a.setAttribute(\"id\",\"station-chart\"),a=t.appendChild(a);let n=dcrel(\"form\");n.setAttribute(\"id\",\"chart-control\"),n.innerHTML='<div class=\"row\"><div class=\"col-6\"><div class=\"row\"><div class=\"form-group col-6\"><label for=\"chtDfrom\">From</label><input type=\"range\" class=\"form-range\" name=\"chtDfrom\"  id=\"chtDfrom\" value=\"0\" min=\"0\"></div><div class=\"form-group col-6\"><label for=\"chtDto\">To</label><input type=\"range\" class=\"form-range\" id=\"chtDto\" name=\"chtDto\" value=\"0\" min=\"0\"></div></div></div><div class=\"col-6\"><div class=\"form-group\"><label for=\"updTimer\">Update Every</label><input type=\"number\" class=\"form-control\" id=\"updTimer\" value=\"'+updateTimer+'\" min=\"5\" max=\"600\"></div></div><div class=\"col-12\"><button type=\"button\" class=\"btn btn-primary\" id=\"upd-cht-btn\">Update</button></div></div>',n=t.appendChild(dcrel(\"div\")).appendChild(n),n.parentElement.classList.add(\"container\"),dgeli(\"upd-cht-btn\").addEventListener(\"click\",(t=>{updateTimer=parseInt(dgeli(\"updTimer\").value),chtDFrom=dgeli(\"chtDfrom\").valueAsNumber,chtDTo=dgeli(\"chtDto\").valueAsNumber,buildChart(Chart.getChart(\"station-chart\"))}));const s={labels:[],type:\"line\",data:{}};getStationData().then((t=>{t.json().then((t=>{let e=[],n=[],r={},o=\"left\";for(const a in t.sensors)if(ohop(t.sensors,a)){const s=t.sensors[a];for(const t in s)if(ohop(s,t)&&mesInCht(a,t)){let i=frmtSnMes(a,t,s[t]);e.push({label:a+\"::\"+t,data:[i],yAxisID:t,pointRadius:1,pointHoverRadius:3}),chartDatas.push([i]),n.includes(t)||(n.push(t),r[t]={id:t,type:\"linear\",position:o},o=\"left\"===o?\"right\":\"left\")}}s.data.datasets=e,s.options={responsive:!0,scales:r};const i=new Chart(a,s);i.data.labels.push(mkDLabel()),chartLabels.push(mkDLabel()),i.update(),setTimeout((()=>{updateDatas(i,updateTimer)}),1e4)}))}))}function updateDatas(t){getStationData().then((e=>{e.json().then((e=>{let a=0;for(const t in e.sensors){const n=e.sensors[t];for(const e in n)if(ohop(n,e)){const s=frmtSnMes(t,e,n[e]);mesInCht(t,e)&&(chartDatas[a].push(s),a++);let r=dqsa(\".sensor.\"+t+\">.snMss>.snMs.\"+e+\">.snMs-value\")[0];r.innerHTML=s;let o=exstSenUtils(t);if(o&&ohop(o,\"format_unit_\"+e)){let a=dqsa(\".sensor.\"+t+\">.snMss>.snMs.\"+e+\">.snMs-unit\")[0];a.innerHTML=o[\"format_unit_\"+e](a.innerHTML)}r.dispatchEvent(new Event(\"updated\"))}}chartLabels.push(mkDLabel()),dgeli(\"chtDfrom\").setAttribute(\"max\",chartLabels.length);let n=dgeli(\"chtDto\");n.value==n.getAttribute(\"max\")&&(n.value=chartLabels.length),n.setAttribute(\"max\",chartLabels.length),0===chtDTo&&(t=buildChart(t)),updateTimer>0&&setTimeout((()=>{updateDatas(t,updateTimer)}),1e3*updateTimer)}))}))}function buildChart(t){let e=chtDTo<chtDFrom?chtDTo:chartLabels.length;t.data.labels=chartLabels.slice(chtDFrom,e);for(let a=0;a<t.data.datasets.length;a++)t.data.datasets[a].data=chartDatas[a].slice(chtDFrom,e);return t.update(\"active\"),t}setTimeout(setUpdatable,250),setTimeout((()=>{initChart()}),500);";
  // commonjs-end
}

String commonBody(String body) {
  return "<body>" + body +
         "</body><script>window.addEventListener('load',()=>{setTimeout(()=>{let scr=document.createElement('script');scr.setAttribute('src','/js');document.body.appendChild(scr)}, 150)})</script>";
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
                         HtmlAttribute("crossorigin", "anonymous"), true
                 ) +
                 HtmlElt(
                     "link", "",
                     HtmlAttribute("rel", "stylesheet") +
                         HtmlAttribute("href", "/css"), true
                 )
         );
}
