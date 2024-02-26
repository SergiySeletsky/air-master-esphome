#include "airmaster.h"

namespace esphome
{
  namespace airmaster
  {

    static const char *const TAG = "air_master";
    static const uint8_t AIRMASTER_RESPONSE_LENGTH = 40;

    void AirMasterSensor::update()
    {
      uint8_t response[AIRMASTER_RESPONSE_LENGTH];
      uint8_t peeked;

      while (this->available() > 0 && this->peek_byte(&peeked) && peeked != 170)
        this->read();

      bool read_success = read_array(response, AIRMASTER_RESPONSE_LENGTH);

      if (!read_success)
      {
        ESP_LOGW(TAG, "Reading data from AirMaster failed!");
      }

      if (read_success)
      {
        unsigned int received_checksum = response[37] | response[36] << 8;
        unsigned int calculated_checksum = 0;
        for (int i = 0; i < 36; i++)
        {
          calculated_checksum += response[i];
        }
        ESP_LOGD(TAG, "zero-byte= %d", response[0]);
        ESP_LOGD(TAG, "received_checksum= %d", received_checksum);
        ESP_LOGD(TAG, "calculated_checksum= %d", calculated_checksum);
        ESP_LOGD(TAG, "temp= %d", ((response[12] << 8) | response[11]) / 100.0);
        if (received_checksum == calculated_checksum)
        {
          // Process and publish sensor data
          pm25_sensor->publish_state(buffer[2] | buffer[1] << 8);
          pm10_sensor->publish_state(buffer[4] | buffer[3] << 8);
          hcho_sensor->publish_state(buffer[6] | buffer[5] << 8);
          tvoc_sensor->publish_state(buffer[8] | buffer[7] << 8);
          co2_sensor->publish_state(buffer[10] | buffer[9] << 8);
          temperature_sensor->publish_state((buffer[12] | buffer[11] << 8) / 100.0);
          humidity_sensor->publish_state((buffer[14] | buffer[13] << 8) / 100.0);

          ppm03_sensor->publish_state(buffer[20] | buffer[19] << 8);
          ppm05_sensor->publish_state(buffer[22] | buffer[21] << 8);
          ppm1_sensor->publish_state(buffer[24] | buffer[23] << 8);
          ppm25_sensor->publish_state(buffer[26] | buffer[25] << 8);
          ppm5_sensor->publish_state(buffer[28] | buffer[27] << 8);
          ppm10_sensor->publish_state(buffer[30] | buffer[29] << 8);
        }
        else
        {
          ESP_LOGD(TAG, "Reading data...");
        }
      }

      // write request command
      const uint8_t command[] = {0x55, 0xCD, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x69, 0x0D, 0x0A};
      this->write_array(command, 13);
    }

  } // namespace airmaster
} // namespace esphome