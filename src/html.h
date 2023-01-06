#include <Arduino.h>

#include "./Sensor.h"
#include "./Station.h"

String HtmlElt(
    String tagName, String inner, String attributes, bool selfClosing
);
String HtmlElt(String tagName, String inner, String attributes);
String HtmlElt(String tagName, String inner);
String HtmlAttribute(String name, String value, bool noSpace = false);
String HtmlClass(String value);
String HtmlId(String value);
String HtmlDiv(String inner, String attributes);
String HtmlDiv(String inner);
String HtmlInput(String attributes);
String HtmlInput();
String commonCss();
String commonJs();
String commonBody(String body);
String commonHtmlHeader(String title);