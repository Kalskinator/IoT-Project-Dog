#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "sensors.h" 


#include "esp_log.h"
static const char* LOG_TAG = "[SYSTEM]"; 


WiFiClient wifi;
PubSubClient mqtt(wifi);

String devicename = "Collar_A001";
String endpoint = "mqtt.thingsboard.cloud";
String topic = "v1/devices/me/telemetry";
String token = "DcuXXsX2ZC2tkJvGdElV";
String telemetry = "{\"temperature\":25}";