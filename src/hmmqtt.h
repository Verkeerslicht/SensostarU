#pragma once

bool ConnectMQTT(PubSubClient mqttClient, const char * mqttServer, uint8_t maxTries);
