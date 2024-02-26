#include "airmaster.h"

namespace esphome {
namespace airmaster {

static const char *const TAG = "air_master";
static const uint8_t AIRMASTER_RESPONSE_LENGTH = 40;

void AirMasterSensor::update() {
  uint8_t response[AIRMASTER_RESPONSE_LENGTH];
  uint8_t peeked;

  while (this->available() > 0 && this->peek_byte(&peeked))
    this->read();

  bool read_success = read_array(response, AIRMASTER_RESPONSE_LENGTH);

  if (!read_success) {
    ESP_LOGW(TAG, "Reading data from SM300D2 failed!");
    return;
  }

  if (read_success) {

    unsigned int received_checksum = response[37] | response[36] << 8;
    unsigned int calculated_checksum = 0;
    for (int i = 0; i < 36; i++) {
      calculated_checksum += response[i];
    }
    ESP_LOGD(TAG, "received_checksum= %d", received_checksum);
    ESP_LOGD(TAG, "calculated_checksum= %d", calculated_checksum);
    ESP_LOGD(TAG, "temp= %d", ((response[12] << 8) | response[11]) / 100.0);
    if (received_checksum == calculated_checksum) {
        // Process and publish sensor data
        pm25_sensor->publish_state((response[2] << 8) | response[1]);
        pm10_sensor->publish_state((response[4] << 8) | response[3]);
        hcho_sensor->publish_state((response[6] << 8) | response[5]);
        tvoc_sensor->publish_state((response[8] << 8) | response[7]);
        co2_sensor->publish_state((response[10] << 8) | response[9]);
        temperature_sensor->publish_state(((response[12] << 8) | response[11]) / 100.0);
        humidity_sensor->publish_state(((response[14] << 8) | response[13]) / 100.0);
        ppm03_sensor->publish_state((response[20] << 8) | response[19]);
        ppm05_sensor->publish_state((response[22] << 8) | response[21]);
        ppm1_sensor->publish_state((response[24] << 8) | response[23]);
        ppm25_sensor->publish_state((response[26] << 8) | response[25]);
        ppm5_sensor->publish_state((response[28] << 8) | response[27]);
        ppm10_sensor->publish_state((response[30] << 8) | response[29]);
        
        ESP_LOGD(TAG, "Data received: PM2.5: %d, PM10: %d, HCHO: %d, TVOC: %d, CO2: %d, Temperature: %.2f, Humidity: %.2f, 0.3um: %d, 0.5um: %d, 1um: %d, 2.5um: %d, 5um: %d, 10um: %d",
                 (response[2] << 8) | response[1], (response[4] << 8) | response[3], (response[6] << 8) | response[5], (response[8] << 8) | response[7], (response[10] << 8) | response[9],
                 ((response[12] << 8) | response[11]) / 100.0, ((response[14] << 8) | response[13]) / 100.0, (response[20] << 8) | response[19], (response[22] << 8) | response[21],
                 (response[24] << 8) | response[23], (response[26] << 8) | response[25], (response[28] << 8) | response[27], (response[30] << 8) | response[29]);
                 
      } else {
        ESP_LOGD(TAG, "Reading data...");
      }

      // write request command
      const uint8_t command[] = {0x55, 0xCD, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x69, 0x0D, 0x0A};
      this->write_array(command, 13);

  }
}

}  // namespace airmaster
}  // namespace esphome