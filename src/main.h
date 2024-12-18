#pragma once
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "ArduinoJson.h"

#include <Adafruit_BMP085.h>

#include "secrets.h"
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

unsigned long           lastMeasurementUpload = 0;
unsigned int            measurement_time = 20;  //in sec 


//BMP180
typedef struct {
    float temp;
    float pressure;
    float altitude;
    float seaLevelPressure;
    float realAltitude;
} bmp_measurement_t;

Adafruit_BMP085                 bmp;  // Initialize the BMP180 sensor
bmp_measurement_t               bmp180Measurement = {};  // Initialize struct with default values
unsigned long                   bmp180LastMeasurementTime = 0;
unsigned long                   bmp180MeasurementPeriod = 100;
unsigned long                   bmp180SampleNumber = 0;
const float                     SEA_LEVEL_PRESSURE = 1015.00;  // Adjust based on your location

void read_BMP180 ();
void Bmp180AddToAverage (bmp_measurement_t& m);
void bmp180Reset (void);

//MAX30010 - M5
typedef struct{
    float heartrate;
    float sp02;
} max30100_measurement_t;

PulseOximeter                   pox;
max30100_measurement_t          max30100Measurement = {};
unsigned long                   max30100LastMeasurementTime = 0;
unsigned long                   max30100MeasurementPeriod = 50;
unsigned long                   max30100SampleNumber = 0;


void read_MAX30100 ();
void max30100AddToAverage (max30100_measurement_t& m);
void max30100Reset (void);

//main functions
void detectSensors(void) {

  if(bmp.begin()){
    ESP_LOGI("SENSORS", "BMP180 detected");
  } else {
    ESP_LOGE("SENSORS", "BMP180 wasn't detected");
  }

  if (pox.begin()) {
    ESP_LOGI("SENSORS", "MAX30100 detected");
    pox.setIRLedCurrent(MAX30100_LED_CURR_24MA);
  } else {
    ESP_LOGE("SENSORS", "MAX30100 wasn't detected");
  }
}


void collectData (void) {
  //TODO: if bmp180 decteced.
  read_BMP180();

  //TODO: if MAX30100 detected
  read_MAX30100 ();

}

void resetData(){

  bmp180Reset();

  max30100Reset();


}



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



// BMP180
void read_BMP180 () {

    unsigned long now = millis();

    if (now - bmp180LastMeasurementTime > bmp180MeasurementPeriod) {
        bmp_measurement_t current_measurement;

        current_measurement.temp              = bmp.readTemperature();
        current_measurement.pressure          = bmp.readPressure();
        current_measurement.altitude          = bmp.readAltitude();
        current_measurement.seaLevelPressure  = bmp.readSealevelPressure();
        current_measurement.realAltitude      = bmp.readAltitude(SEA_LEVEL_PRESSURE);
        
        bmp180SampleNumber++; //might need later if do an avg. filter

        bmp180LastMeasurementTime = now;

        Bmp180AddToAverage(current_measurement);

    }

}

void Bmp180AddToAverage (bmp_measurement_t& m) {
    // New average = old average * (n-1)/n + new value /n
    bmp180Measurement.temp = bmp180Measurement.temp * (bmp180SampleNumber-1) / bmp180SampleNumber
                            + m.temp / bmp180SampleNumber;

    bmp180Measurement.pressure = (bmp180Measurement.pressure * (bmp180SampleNumber-1) / bmp180SampleNumber
                            + m.pressure / bmp180SampleNumber);                                         

    bmp180Measurement.altitude = bmp180Measurement.altitude * (bmp180SampleNumber-1) / bmp180SampleNumber
                            + m.altitude / bmp180SampleNumber;

    bmp180Measurement.seaLevelPressure = bmp180Measurement.seaLevelPressure * (bmp180SampleNumber-1) / bmp180SampleNumber
                            + m.seaLevelPressure / bmp180SampleNumber;           

    bmp180Measurement.realAltitude = bmp180Measurement.realAltitude * (bmp180SampleNumber-1) / bmp180SampleNumber
                            + m.realAltitude / bmp180SampleNumber;                               
}

void bmp180Reset (void) {
    memset(&bmp180Measurement, 0, sizeof(bmp180Measurement));
    bmp180SampleNumber = 0;
}



void read_MAX30100 () { 
    unsigned long now = millis();

    if (now - max30100LastMeasurementTime > max30100MeasurementPeriod) {
      max30100_measurement_t current_measurement;
      current_measurement.heartrate = pox.getHeartRate();
      current_measurement.sp02      = pox.getSpO2();

      max30100SampleNumber++;
      max30100LastMeasurementTime = now;

      max30100AddToAverage(current_measurement);

    }
}

void max30100AddToAverage (max30100_measurement_t& m) {
    // New average = old average * (n-1)/n + new value /n
    max30100Measurement.heartrate = max30100Measurement.heartrate * (max30100SampleNumber-1) / max30100SampleNumber
                            + m.heartrate / max30100SampleNumber;

    max30100Measurement.sp02 = (max30100Measurement.sp02 * (max30100SampleNumber-1) / max30100SampleNumber
                            + m.sp02 / max30100SampleNumber);                                                                       
}

void max30100Reset (void) {
    memset(&max30100Measurement, 0, sizeof(max30100Measurement));
    max30100SampleNumber = 0;
}

