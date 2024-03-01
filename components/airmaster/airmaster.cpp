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

        if (received_checksum == calculated_checksum && received_checksum != 243 && received_checksum != 680) // 243 and 680 are common checksums when the sensor is not connected
        {
          // Process and publish sensor data
          pm25_sensor->publish_state(response[2] | response[1] << 8);
          pm10_sensor->publish_state(response[4] | response[3] << 8);
          hcho_sensor->publish_state(response[6] | response[5] << 8);
          tvoc_sensor->publish_state(response[8] | response[7] << 8);
          co2_sensor->publish_state(response[10] | response[9] << 8);
          temperature_sensor->publish_state((response[12] | response[11] << 8) / 100.0);
          humidity_sensor->publish_state((response[14] | response[13] << 8) / 100.0);

          ppm03_sensor->publish_state(response[20] | response[19] << 8);
          ppm05_sensor->publish_state(response[22] | response[21] << 8);
          ppm1_sensor->publish_state(response[24] | response[23] << 8);
          ppm25_sensor->publish_state(response[26] | response[25] << 8);
          ppm5_sensor->publish_state(response[28] | response[27] << 8);
          ppm10_sensor->publish_state(response[30] | response[29] << 8);
        }
        else
        {
          ESP_LOGI(TAG, "Reading data...");
        }
      }

      // write request command
      const uint8_t command[] = {0x55, 0xCD, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x69, 0x0D, 0x0A};
      this->write_array(command, 13);
    }

  } // namespace airmaster
} // namespace esphome