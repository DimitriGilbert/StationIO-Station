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

String commonBody(String body) {
  return "<body>" + body +
         "</body><script>window.addEventListener('load',()=>{setTimeout(()=>{let scr=document.createElement('script');scr.setAttribute('src','/assets/js/common.js');document.body.appendChild(scr)}, 75)})</script>";
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
                         HtmlAttribute("href", "/assets/css/common.css"), true
                 )
         );
}
