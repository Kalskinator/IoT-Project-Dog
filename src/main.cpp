/*
Arduino-MAX30100 oximetry / heart rate integrated sensor library
Copyright (C) 2016  OXullo Intersecans <x@brainrapers.org>


This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Arduino.h>
#include "main.h"



void setup() {

  // Initialize Serial DEBUG
  esp_log_level_set(LOG_TAG, ESP_LOG_VERBOSE);
  // WiFi connection (you need to configure SSID and Password)


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  ESP_LOGI("[WIFI]", "WiFi Connected");

  mqtt.setServer(endpoint.c_str(), 1883);
  ESP_LOGI("MQTT", "MQTT Configured");

  mqtt.connect(token.c_str(),token.c_str(),"");


  detectSensors();
}

void loop()
{
  unsigned long now = millis();
  pox.update();
  collectData();
    // Asynchronously dump heart rate and oxidation levels to the serial
/*   if (now - lastMeasurementUpload > (unsigned long) 1*1000){
    float hr = pox.getHeartRate();
    float SP02 = pox.getSpO2();
    unsigned long now = millis();
    ESP_LOGI("TEST", "pox print");
      ESP_LOGI("MAX30100 hr", "%f", hr);
      ESP_LOGI("MAX30100 sp02", "%f", SP02 );
      lastMeasurementUpload = now;
  } */

    if (now - lastMeasurementUpload > (unsigned long) measurement_time*1000){
        pox.shutdown();
        PayloadPrepare();
        resetData();
        
        if (!mqtt.connected()) {
            if (mqtt.connect(devicename.c_str(), token.c_str(), "")) {
                ESP_LOGI("MQTT", "MQTT Connected");
            } else {
                ESP_LOGW("MQTT", "Failed to connect to MQTT");
                delay(5000);  // Wait before retrying
                return;
            }
        }

        String telemetry;

        serializeJson( jsonTelemetry, telemetry );
        ESP_LOGI("TELEMETRY", "%s", telemetry.c_str());
        mqtt.publish(topic.c_str(), telemetry.c_str(), true);
        ESP_LOGI("MQTT", "Published"); 
        mqtt.disconnect();
        lastMeasurementUpload = now;
        pox.resume();
    }
}