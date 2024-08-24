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

    // Checksum constants
    static const unsigned int CHECKSUM_SENSOR_NOT_CONNECTED_1 = 243;
    static const unsigned int CHECKSUM_SENSOR_NOT_CONNECTED_2 = 244;
    static const unsigned int CHECKSUM_SENSOR_NOT_CONNECTED_3 = 680;

    // Index constants
    static const unsigned int INDEX_PM25 = 1;
    static const unsigned int INDEX_PM10 = 3;
    static const unsigned int INDEX_HCHO = 5;
    static const unsigned int INDEX_TVOC = 7;
    static const unsigned int INDEX_CO2 = 9;
    static const unsigned int INDEX_TEMPERATURE = 11;
    static const unsigned int INDEX_HUMIDITY = 13;
    static const unsigned int INDEX_PPM03 = 19;
    static const unsigned int INDEX_PPM05 = 21;
    static const unsigned int INDEX_PPM1 = 23;
    static const unsigned int INDEX_PPM25 = 25;
    static const unsigned int INDEX_PPM5 = 27;
    static const unsigned int INDEX_PPM10 = 29;

    template<typename T>
    void AirMasterSensor::extract_and_publish(uint8_t *response, Sensor *sensor, unsigned int index, T min_limit, T max_limit, T scale)
    {
      T value = (response[index + 1] | (response[index] << 8)) / scale;
      if (value > min_limit && value < max_limit)
        sensor->publish_state(static_cast<float>(value));
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
        // Calculate checksum manually
        unsigned int received_checksum = response[37] | (response[36] << 8);
        unsigned int calculated_checksum = 0;
        for (int i = 0; i < 36; i++)
        {
          calculated_checksum += response[i];
        }

        if (received_checksum == calculated_checksum &&
            received_checksum != CHECKSUM_SENSOR_NOT_CONNECTED_1 &&
            received_checksum != CHECKSUM_SENSOR_NOT_CONNECTED_2 &&
            received_checksum != CHECKSUM_SENSOR_NOT_CONNECTED_3) // 243, 244, and 680 are common checksums when the sensor is not connected
        {
          // Process and publish sensor data using the template function
          extract_and_publish(response, pm25_sensor, INDEX_PM25, MIN_SENSOR_LIMIT, MAX_PM25);
          extract_and_publish(response, pm10_sensor, INDEX_PM10, MIN_SENSOR_LIMIT, MAX_PM10);
          extract_and_publish(response, hcho_sensor, INDEX_HCHO, MIN_SENSOR_LIMIT, MAX_HCHO);
          extract_and_publish(response, tvoc_sensor, INDEX_TVOC, MIN_SENSOR_LIMIT, MAX_TVOC);
          extract_and_publish(response, co2_sensor, INDEX_CO2, MIN_CO2, MAX_CO2);

          extract_and_publish(response, temperature_sensor, INDEX_TEMPERATURE, MIN_TEMPERATURE, MAX_TEMPERATURE, 100.0);
          extract_and_publish(response, humidity_sensor, INDEX_HUMIDITY, MIN_HUMIDITY, MAX_HUMIDITY, 100.0);

          extract_and_publish(response, ppm03_sensor, INDEX_PPM03, MIN_SENSOR_LIMIT, MAX_PPM03);
          extract_and_publish(response, ppm05_sensor, INDEX_PPM05, MIN_SENSOR_LIMIT, MAX_PPM05);
          extract_and_publish(response, ppm1_sensor, INDEX_PPM1, MIN_SENSOR_LIMIT, MAX_PPM1);
          extract_and_publish(response, ppm25_sensor, INDEX_PPM25, MIN_SENSOR_LIMIT, MAX_PPM25);
          extract_and_publish(response, ppm5_sensor, INDEX_PPM5, MIN_SENSOR_LIMIT, MAX_PPM5);
          extract_and_publish(response, ppm10_sensor, INDEX_PPM10, MIN_SENSOR_LIMIT, MAX_PPM10);

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