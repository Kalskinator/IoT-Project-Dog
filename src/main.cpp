#include "main.h"

void setup() {

  // Initialize Serial DEBUG
  esp_log_level_set(LOG_TAG, ESP_LOG_VERBOSE);
  // WiFi connection (you need to configure SSID and Password)
/*   const char* ssid = getenv(SSID);
  const char* password = getenv("PASSWORD"); */

/*   if (ssid == nullptr || password == nullptr) {
      ESP_LOGE("[WIFI]", "SSID or PASSWORD environment variable not set");
      return;
  } */

  // WiFi connection (you need to configure SSID and Password)

  WiFi.begin("iot", "");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  ESP_LOGI("[WIFI]", "WiFi Connected");

  detect_Sensors();

  // MQTT setup
  mqtt.setServer(endpoint.c_str(), 1883);
  ESP_LOGI("[MQTT]", "MQTT Configured");

  mqtt.connect(token.c_str(),token.c_str(),"");

  delay(1000);

}

void loop() {

  //TODO: Read sensor func
  read_BMP180();

  mqtt.loop();  // Maintain MQTT connection
  if (!mqtt.connected()) {
      if (mqtt.connect(devicename.c_str(), token.c_str(), "")) {
          ESP_LOGI("MQTT", "MQTT Connected");
      } else {
          ESP_LOGW("MQTT", "Failed to connect to MQTT");
          delay(5000);  // Wait before retrying
          return;
      }
  }
  
  PayloadPrepare();
  String telemetry;
  serializeJson( jsonTelemetry, telemetry );
  ESP_LOGI("TELEMETRY", "%s", telemetry.c_str());
  mqtt.publish(topic.c_str(), telemetry.c_str(), true);
  ESP_LOGI("MQTT", "Published");

  delay(1000);  // Adjust delay as necessary
}