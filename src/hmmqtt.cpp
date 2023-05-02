#include <Arduino.h>
#include <WifiManager.h>
#include <PubSubClient.h>


// ConnectMQTT connects to mqtt server for nr maxTries
// returns true if succeeded, false otherwise
bool ConnectMQTT(PubSubClient mqttClient, const char * mqttServer, uint8_t maxTries) {
  Serial.println("attempt connect to mqtt server");

  for (int i = 0; i < maxTries; ++i) {
    if (mqttClient.connect(mqttServer)) return true;
    delay(500);
  }
  return false; // failed after 'x' attempts
}

