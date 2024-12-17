#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "ArduinoJson.h"

#define MAX30100_REG_SPO2_CONFIGURATION         0x4  // set sampling rate for the HR sensor (M5)
#include "MAX30100_PulseOximeter.h"

StaticJsonDocument<1024>     doc;
JsonObject              jsonTelemetry;


#include "sensors.h" 

#include "esp_log.h"
static const char* LOG_TAG = "[SYSTEM]"; 


WiFiClient wifi;
PubSubClient mqtt(wifi);

String devicename = "Collar_A001";
String endpoint = "mqtt.thingsboard.cloud";
String topic = "v1/devices/me/telemetry";
String token = "DcuXXsX2ZC2tkJvGdElV";

unsigned long           lastMeasurementUpload = 0;
unsigned int            measurement_time = 10;  //in sec 


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

    //TODO:
    values["MAX30100_HeartRate"]       = max30100Measurement.heartrate;
    values["MAX30100_SP02"]            = max30100Measurement.sp02;
    
    jsonTelemetry = values;
}

