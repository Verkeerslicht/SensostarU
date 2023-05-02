#pragma once

#include <ArduinoJson.h>
#if defined(ESP32)
#include <WebServer.h>
#include <mDNS.h>
#elif defined(ESP8266)
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#else
#error "neither ESP8266 or ESP32"
#endif
#include <sensostar.h>
#include <ArduinoJson.h>
#include <hmapi.h>

#if defined(ESP32)
typedef ESPWebServer HMApiServer;
#elif defined(ESP8266)
typedef ESP8266WebServer HMApiServer;
#else
#error "neither ESP8266 or ESP32"
#endif

extern struct sensostar_data heatMeterResult;
extern HMApiServer apiServer;

void HMApiSetup();
