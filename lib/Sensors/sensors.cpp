#include "sensors.h"

// Define global variables here
//BMP180
Adafruit_BMP085 bmp;  // Initialize the BMP180 sensor
bmp_measurement_t bmp180Measurement = {};  // Initialize struct with default values
unsigned long bmp180LastMeasurementTime = 0;
unsigned long bmp180MeasurementPeriod = 1000;
unsigned long bmp180SampleNumber = 0;

void detect_Sensors(void) {

  if(bmp.begin()){
    ESP_LOGI("SENSORS", "BMP180 detected");
  } else {
    ESP_LOGE("SENSORS", "BMP180 wasn't detected");
  }
}



void read_BMP180 () {

    unsigned long now = millis();

    if (now - bmp180LastMeasurementTime > bmp180MeasurementPeriod) {


        bmp180Measurement.temp              = bmp.readTemperature();
        bmp180Measurement.pressure          = bmp.readPressure();
        bmp180Measurement.altitude          = bmp.readAltitude();
        bmp180Measurement.seaLevelPressure  = bmp.readSealevelPressure();
        bmp180Measurement.realAltitude      = bmp.readAltitude(SEA_LEVEL_PRESSURE);
        
        bmp180SampleNumber++; //might need later if do an avg. filter

        bmp180LastMeasurementTime = now;

        

    }

}
