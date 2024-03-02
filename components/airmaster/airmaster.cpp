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
          unsigned int pm25 = response[2] | response[1] << 8;
          if (pm25 < 999) // 999 max range of pm25 meter
          {
            pm25_sensor->publish_state(pm25);
          }
          
          unsigned int pm10 = response[4] | response[3] << 8;
          if (pm10 < 999) // 999 max range of pm25 meter
          {
            pm10_sensor->publish_state(pm10);
          }

          unsigned int hcho = response[6] | response[5] << 8;
          if(hcho < 3000) // 3000 max range of hcho meter (3.0 mg/m3)
          {
            hcho_sensor->publish_state(hcho);
          }
          
          unsigned int tvoc = response[8] | response[7] << 8;
          if(tvoc < 2000) // 2000 max range of tvoc meter (2.0 mg/m3)
          {
            tvoc_sensor->publish_state(response[8] | response[7] << 8);
          }
          
          unsigned int co2 = response[10] | response[9] << 8;
          if (co2 < 5000) // 5000 max range of co2 meter
          {
            co2_sensor->publish_state(co2);
          }
          
          double temperature = (response[12] | response[11] << 8) / 100.0;
          if(temperature > -25 && temperature < 85) // -25 to 85 degrees Celsius
          {
            temperature_sensor->publish_state(temperature);
          }

          double humidity = (response[14] | response[13] << 8) / 100.0;
          if(humidity > 0 && humidity < 99) // 0 to 99% relative humidity
          {
            humidity_sensor->publish_state(humidity);
          }

          // PPM counters
          unsigned int ppm03 = response[20] | response[19] << 8;
          if (ppm03 < 20000) // 20000 max range of ppm03 meter
          {
            ppm03_sensor->publish_state(ppm03);
          }

          unsigned int ppm05 = response[22] | response[21] << 8;
          if (ppm05 < 10000) // 10000 max range of ppm05 meter
          {
            ppm05_sensor->publish_state(ppm05);
          }

          unsigned int ppm1 = response[24] | response[23] << 8;
          if (ppm1 < 5000) // 5000 max range of ppm1 meter
          {
            ppm1_sensor->publish_state(ppm1);
          }

          unsigned int ppm25 = response[26] | response[25] << 8;
          if (ppm25 < 2000) // 2000 max range of ppm25 meter
          {
            ppm25_sensor->publish_state(ppm25);
          }

          unsigned int ppm5 = response[28] | response[27] << 8;
          if (ppm5 < 1000) // 1000 max range of ppm5 meter
          {
            ppm5_sensor->publish_state(ppm5);
          }

          unsigned int ppm10 = response[30] | response[29] << 8;
          if (ppm10 < 500) // 500 max range of ppm10 meter
          {
            ppm10_sensor->publish_state(ppm10);
          }
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