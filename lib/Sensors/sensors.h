#include <Adafruit_BMP085.h>
#include <Arduino.h>
#include "ArduinoJson.h"

void detect_Sensors();

//BMP180
typedef struct {
    float temp;
    float pressure;
    float altitude;
    float seaLevelPressure;
    float realAltitude;
} bmp_measurement_t;

extern  Adafruit_BMP085         bmp;
extern  bmp_measurement_t       bmp180Measurement;
extern  unsigned long           bmp180LastMeasurementTime;
extern  unsigned long           bmp180MeasurementPeriod;
extern  unsigned long           bmp180SampleNumber;
const float                     SEA_LEVEL_PRESSURE = 1015.00;  // Adjust based on your location

void read_BMP180 ();
