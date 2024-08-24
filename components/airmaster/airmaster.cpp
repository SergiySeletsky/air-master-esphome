#include "airmaster.h"
#include "esphome/components/sensor/sensor.h" // Include the correct header for the Sensor class

namespace esphome
{
  namespace airmaster
  {
    using esphome::sensor::Sensor; // Ensure the Sensor class is properly referenced

    static const char *const TAG = "air_master";
    static const uint8_t AIRMASTER_RESPONSE_LENGTH = 40;

    // Sensor limit constants
    static const unsigned int MAX_PM25 = 999;
    static const unsigned int MAX_PM10 = 999;
    static const unsigned int MAX_HCHO = 3000;
    static const unsigned int MAX_TVOC = 2000;
    static const unsigned int MAX_CO2 = 5000;
    static const unsigned int MIN_CO2 = 350;
    static const double MAX_TEMPERATURE = 85.0;
    static const double MIN_TEMPERATURE = -25.0;
    static const double MAX_HUMIDITY = 99.0;
    static const double MIN_HUMIDITY = 0.0;
    static const unsigned int MAX_PPM03 = 5000;
    static const unsigned int MAX_PPM05 = 2000;
    static const unsigned int MAX_PPM1 = 1000;
    static const unsigned int MAX_PPM25 = 100;
    static const unsigned int MAX_PPM5 = 50;
    static const unsigned int MAX_PPM10 = 25;
    static const unsigned int MIN_SENSOR_LIMIT = 0;

    // Checksum constants 243, 244, and 680 are common checksums when the sensor is not connected
    static const unsigned int CHECKSUM_SENSOR_NOT_CONNECTED_1 = 243;
    static const unsigned int CHECKSUM_SENSOR_NOT_CONNECTED_2 = 244;
    static const unsigned int CHECKSUM_SENSOR_NOT_CONNECTED_3 = 680;

    template <typename T>
    void AirMasterSensor::extract_and_publish(uint8_t *response, Sensor *sensor, unsigned int &index, T min_limit, T max_limit, T scale)
    {
      T value = (response[index + 1] | (response[index] << 8)) / scale;
      if (value > min_limit && value < max_limit)
        sensor->publish_state(static_cast<float>(value));
      index += sizeof(uint16_t);
    }

    unsigned int AirMasterSensor::calculate_checksum(const uint8_t *response, unsigned int length)
    {
      unsigned int checksum = 0;
      for (unsigned int i = 0; i < length; i++)
      {
        checksum += response[i];
      }
      return checksum;
    }

    void AirMasterSensor::update()
    {
      uint8_t response[AIRMASTER_RESPONSE_LENGTH];
      uint8_t peeked;

      while (this->available() > 0 && this->peek_byte(&peeked) && peeked != 170)
        this->read();

      bool read_success = read_array(response, AIRMASTER_RESPONSE_LENGTH);

      if (read_success)
      {
        // Calculate checksum
        unsigned int received_checksum = response[37] | (response[36] << 8);
        unsigned int calculated_checksum = calculate_checksum(response, 36);

        if (received_checksum == calculated_checksum &&
            received_checksum != CHECKSUM_SENSOR_NOT_CONNECTED_1 &&
            received_checksum != CHECKSUM_SENSOR_NOT_CONNECTED_2 &&
            received_checksum != CHECKSUM_SENSOR_NOT_CONNECTED_3)
        {
          unsigned int index = 1;

          // Process and publish sensor data using the template function with post-increment
          extract_and_publish(response, pm25_sensor, index, MIN_SENSOR_LIMIT, MAX_PM25);
          extract_and_publish(response, pm10_sensor, index, MIN_SENSOR_LIMIT, MAX_PM10);
          extract_and_publish(response, hcho_sensor, index, MIN_SENSOR_LIMIT, MAX_HCHO);
          extract_and_publish(response, tvoc_sensor, index, MIN_SENSOR_LIMIT, MAX_TVOC);
          extract_and_publish(response, co2_sensor, index, MIN_CO2, MAX_CO2);
          extract_and_publish(response, temperature_sensor, index, MIN_TEMPERATURE, MAX_TEMPERATURE, 100.0);
          extract_and_publish(response, humidity_sensor, index, MIN_HUMIDITY, MAX_HUMIDITY, 100.0);

          // Skip reserved bytes by incrementing the index
          index += 4; // Assuming 4 reserved bytes

          extract_and_publish(response, ppm03_sensor, index, MIN_SENSOR_LIMIT, MAX_PPM03);
          extract_and_publish(response, ppm05_sensor, index, MIN_SENSOR_LIMIT, MAX_PPM05);
          extract_and_publish(response, ppm1_sensor, index, MIN_SENSOR_LIMIT, MAX_PPM1);
          extract_and_publish(response, ppm25_sensor, index, MIN_SENSOR_LIMIT, MAX_PPM25);
          extract_and_publish(response, ppm5_sensor, index, MIN_SENSOR_LIMIT, MAX_PPM5);
          extract_and_publish(response, ppm10_sensor, index, MIN_SENSOR_LIMIT, MAX_PPM10);
        }
        else
        {
          ESP_LOGW(TAG, "Checksum mismatch: received %d, calculated %d", received_checksum, calculated_checksum);
        }
      }
      else
      {
        ESP_LOGW(TAG, "Reading data from AirMaster failed!");
      }

      // write request command
      const uint8_t command[] = {0x55, 0xCD, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x69, 0x0D, 0x0A};
      this->write_array(command, 13);
    }

  } // namespace airmaster
} // namespace esphome