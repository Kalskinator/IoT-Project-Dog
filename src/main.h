#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "ArduinoJson.h"
#include "secrets.h"
#include "sensors.h" 
#include "esp_log.h"

StaticJsonDocument<1024>     doc;
JsonObject              jsonTelemetry;

static const char* LOG_TAG = "[SYSTEM]"; 

WiFiClient wifi;
PubSubClient mqtt(wifi);

String devicename = "Collar_A001";
String endpoint = "mqtt.thingsboard.cloud";
String topic = "v1/devices/me/telemetry";
String token = "DcuXXsX2ZC2tkJvGdElV";

void PayloadPrepare (void) {

    static uint continuousMessages = 0;

    // Create the values object inside the document
    jsonTelemetry.clear();
    JsonObject values;
    values = doc.to<JsonObject>();

    //TODO: IF BMP180 detected. 
    values["BMP180_TEMP"]              = bmp180Measurement.temp;
    values["BMP180_PRESSURE"]          = bmp180Measurement.pressure;
    values["BMP180_ALTITUDE"]          = bmp180Measurement.altitude;
    values["BMP180_SEALEVELPRES"]      = bmp180Measurement.seaLevelPressure;
    values["BMP180_REALALTITUDE"]      = bmp180Measurement.realAltitude;
    
    jsonTelemetry = values;

}