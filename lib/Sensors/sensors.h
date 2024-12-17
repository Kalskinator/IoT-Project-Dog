#include <Adafruit_BMP085.h>
#include <Arduino.h>
#include "ArduinoJson.h"

#define MAX30100_REG_SPO2_CONFIGURATION         0x4  // set sampling rate for the HR sensor (M5)
#include "MAX30100_PulseOximeter.h"


void detectSensors();
void collectData ();
void resetData();

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
void Bmp180AddToAverage (bmp_measurement_t& m);
void bmp180Reset (void);


//MAX30010 - M5
typedef struct{
    float heartrate;
    float sp02;
} max30100_measurement_t;

extern max30100_measurement_t   max30100Measurement;
extern PulseOximeter            pox;
extern  unsigned long           max30100LastMeasurementTime;
extern  unsigned long           max30100MeasurementPeriod;
extern  unsigned long           max30100SampleNumber;

void read_MAX30100 ();
void max30100AddToAverage (max30100_measurement_t& m);
void max30100Reset (void);
