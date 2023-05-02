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

struct sensostar_data heatMeterResult;
HMApiServer apiServer(80);

typedef struct thing {
  const char *type;
  float value;
  const char *unit;
} apiElement;


void addJsonObj(JsonDocument& doc, const char *tag, float value, const char *unit);

void getTotalHeatEnergy();
void getPower();
void getFlowTemp();
void getFlowReturnTemp();
void getFlowDeltaTemp();
void getFlowSpeed();
void getError();
void getAll();

// HMApiSetup binds functions to API calls
void HMApiSetup() {
  apiServer.on("/total_heat_energy", getTotalHeatEnergy);
  apiServer.on("/power", getPower);
  apiServer.on("/flow_temp", getFlowTemp);
  apiServer.on("/flow_return_temp", getFlowReturnTemp);
  apiServer.on("/flow_delta_temp", getFlowDeltaTemp);
  apiServer.on("/flow_speed", getFlowSpeed);
  apiServer.on("/err", getError);

  apiServer.on("/all", getAll);

  apiServer.begin();
}

void getTotalHeatEnergy() {
  StaticJsonDocument<150> json;
  String jsonString;

  json["type"] = "total energy";
  json["value"] = heatMeterResult.total_heat_energy;
  json["unit"] = "kWh";
  serializeJson(json, jsonString);

  apiServer.send(200, "application/json", jsonString);
}

void getPower() {
  Serial.println("getPower");
  StaticJsonDocument<150> json;
  String jsonString;

  json["type"] = "temperature";
  json["value"] = heatMeterResult.power;
  json["unit"] = "Watt";
  serializeJson(json, jsonString);

  apiServer.send(200, "application/json", jsonString);
}

void getFlowTemp() {
  StaticJsonDocument<150> json;
  String jsonString;

  json["type"] = "flow_temp";
  json["value"] = heatMeterResult.flow_temperature;
  json["unit"] = "°C";
  serializeJson(json, jsonString);

  apiServer.send(200, "application/json", jsonString);
}

void getFlowReturnTemp() {
  StaticJsonDocument<150> json;
  String jsonString;

  json["type"] = "flow_return_temp";
  json["value"] = heatMeterResult.return_temperature;
  json["unit"] = "°C";
  serializeJson(json, jsonString);

  apiServer.send(200, "application/json", jsonString);
}

void getFlowDeltaTemp() {
  StaticJsonDocument<150> json;
  String jsonString;

  json["type"] = "flow_delta_temp";
  json["value"] = heatMeterResult.flow_return_difference_temperature;
  json["unit"] = "°C";
  serializeJson(json, jsonString);

  apiServer.send(200, "application/json", jsonString);
}

void getFlowSpeed() {
  StaticJsonDocument<150> json;
  String jsonString;

  json["type"] = "flow_speed";
  json["value"] = heatMeterResult.flow_speed;
  json["unit"] = "L/h";
  serializeJson(json, jsonString);

  apiServer.send(200, "application/json", jsonString);
}

void getError() {
  StaticJsonDocument<150> json;
  String jsonString;

  json["type"] = "error";
  json["value"] = heatMeterResult.error;
  json["unit"] = "NA";
  serializeJson(json, jsonString);

  apiServer.send(200, "application/json", jsonString);
}

void addJsonObj(JsonDocument& doc, const char *tag, float value, const char *unit) {
  JsonObject obj = doc.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit;
}

void getAll() {
  Serial.println("getAll");

  StaticJsonDocument<1000> json;
  addJsonObj(json, "total_heat", heatMeterResult.total_heat_energy, "kWh");
  addJsonObj(json, "power", heatMeterResult.power, "Watt");
  addJsonObj(json, "flow_temp", heatMeterResult.flow_temperature, "°C");
  addJsonObj(json, "flow_return_temp", heatMeterResult.return_temperature, "°C");
  addJsonObj(json, "flow_delta_temp", heatMeterResult.flow_return_difference_temperature * 10, "°C");
  addJsonObj(json, "err", heatMeterResult.error, "NA");
  addJsonObj(json, "flow_speed", heatMeterResult.flow_speed, "L/h");

  String jsonString;
  serializeJson(json, jsonString);
  apiServer.send(200, "application/json", jsonString);
}
