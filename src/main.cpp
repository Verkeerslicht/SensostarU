#include <Arduino.h>
#include <sensostar.h>
#include <WifiManager.h>
#include <PubSubClient.h>
#include <hmapi.h>
#include <hmmqtt.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

uint32_t lastMsg = 0;
const char* mqttServer = "test.mosquitto.org"; // TODO use personal mqtt broker here


static size_t hmWrite(uint8_t c) {
  return Serial.write(c);
}
Sensostar heatMeter(hmWrite, millis);

void setup() {
  Serial.begin(2400); // low baudrate for mbus

  Serial.println("create wifi access point");
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  // wm.resetSettings(); // comment when releasing

  // connect to WiFi
  bool wifiConnected = wm.autoConnect("HeatMeter"); // access ap without pass
  if (!wifiConnected) {
    Serial.println("failed connecting to WiFi-network");
    delay(500);
    ESP.reset();
  }
  Serial.println("success connecting to WiFi-network");

  // connect to mqtt broker
  mqttClient.setServer(mqttServer, 1883);
  bool mqttConnected = ConnectMQTT(mqttClient, mqttServer, 5);
  if (!mqttConnected) {
    Serial.println("failed connecting to mqtt broker");
    Serial.println("wifi conecton reset");
    wm.resetSettings(); // comment when releasing
    ESP.reset();
  }

  // setup heat meter API
  Serial.println("setup heat meter API");
  HMApiSetup();

  Serial.println("connect to heat energy meter: 2400,8,E,1");
  delay(1000);

  Serial.begin(2400, SERIAL_8E1);
}


void loop() {

  apiServer.handleClient();

  // check whether received data is pending
  while(Serial.available()) {
    char c = Serial.read();
    struct sensostar_data result;

    uint8_t res = heatMeter.process(c, &result);
    if (res > 0) {
      heatMeterResult = result;

      mqttClient.publish("heatmeter/TotalHeatEnergy", String(heatMeterResult.total_heat_energy).c_str());
      mqttClient.publish("heatmeter/Power", String(heatMeterResult.power).c_str());
      mqttClient.publish("heatmeter/FlowTemp", String(heatMeterResult.flow_temperature).c_str());
      mqttClient.publish("heatmeter/FlowReturnTemp", String(heatMeterResult.return_temperature).c_str());
      mqttClient.publish("heatmeter/FlowDeltaTemp", String(heatMeterResult.flow_return_difference_temperature).c_str());
      mqttClient.publish("heatmeter/FlowSpeed", String(heatMeterResult.flow_speed).c_str());
      mqttClient.publish("heatmeter/Error", String(heatMeterResult.error).c_str());
    }
  }

  // request heat meter data each 120 seconds
  static uint32_t hmTime = 0;
  if (millis() > hmTime + 120000) {
    heatMeter.request();
    hmTime = millis();
  }

  // force reconnect if WiFi connection drops
  if (WiFi.status() != WL_CONNECTED) {
    ESP.reset();
  }
}