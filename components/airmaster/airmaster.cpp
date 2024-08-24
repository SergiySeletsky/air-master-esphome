#include "airmaster.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome
{
  namespace airmaster
  {
    static const char *const TAG = "air_master";
    static const uint8_t AIRMASTER_RESPONSE_LENGTH = 40;

    // Sensor limit constants
    static const unsigned int MAX_PM25 = 999;       // 999 max range of pm25 meter
    static const unsigned int MAX_PM10 = 999;       // 999 max range of pm10 meter
    static const unsigned int MAX_HCHO = 3000;      // 3000 max range of hcho meter (3.0 mg/m3)
    static const unsigned int MAX_TVOC = 2000;      // 2000 max range of tvoc meter (2.0 mg/m3)
    static const unsigned int MAX_CO2 = 5000;       // to 5000 max range of co2 meter
    static const unsigned int MIN_CO2 = 350;        // from 350 range of co2 meter
    static const double MAX_TEMPERATURE = 85.0;     // 85 max degrees Celsius
    static const double MIN_TEMPERATURE = -25.0;    // -25 min degrees Celsius
    static const double MAX_HUMIDITY = 99.0;        // max 99% relative humidity
    static const double MIN_HUMIDITY = 0.0;         // min 0% relative humidity
    static const unsigned int MAX_PPM03 = 5000;     // 5000 max range of ppm03 meter
    static const unsigned int MAX_PPM05 = 2000;     // 2000 max range of ppm05 meter
    static const unsigned int MAX_PPM1 = 1000;      // 1000 max range of ppm1 meter
    static const unsigned int MAX_PPM25 = 100;      // 100 max range of ppm25 meter
    static const unsigned int MAX_PPM5 = 50;        // 50 max range of ppm5 meter
    static const unsigned int MAX_PPM10 = 25;       // 25 max range of ppm10 meter
    static const unsigned int MIN_SENSOR_LIMIT = 0; // 0 min range of sensor

    // Checksum constants
    static const unsigned int CHECKSUM_SENSOR_NOT_CONNECTED_1 = 243;
    static const unsigned int CHECKSUM_SENSOR_NOT_CONNECTED_2 = 244;
    static const unsigned int CHECKSUM_SENSOR_NOT_CONNECTED_3 = 680;

    void publish_if_within_limits(Sensor *sensor, unsigned int value, unsigned int min_limit, unsigned int max_limit)
    {
        if (value < max_limit) sensor->publish_state(value);
    }

    void publish_if_within_limits(Sensor *sensor, double value, double min_limit, double max_limit)
    {
        if (value > min_limit && value < max_limit) sensor->publish_state(value);
    }

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
        return; // Exit the function if read failed
      }

      unsigned int received_checksum = response[37] | response[36] << 8;
      unsigned int calculated_checksum = 0;
      for (int i = 0; i < 36; i++)
      {
        calculated_checksum += response[i];
      }

      if (received_checksum != calculated_checksum
      || received_checksum == CHECKSUM_SENSOR_NOT_CONNECTED_1
      || received_checksum == CHECKSUM_SENSOR_NOT_CONNECTED_2
      || received_checksum == CHECKSUM_SENSOR_NOT_CONNECTED_3)
      { // 243, 244 and 680 are common checksums when the sensor is not connected
        ESP_LOGW(TAG, "Checksum mismatch: received %d, calculated %d", received_checksum, calculated_checksum);
        return; // Exit the function if checksum mismatch
      }

      // Process and publish sensor data
      unsigned int pm25 = response[2] | response[1] << 8;
      publish_if_within_limits(pm25_sensor, pm25, MIN_SENSOR_LIMIT, MAX_PM25);

      unsigned int pm10 = response[4] | response[3] << 8;
      publish_if_within_limits(pm10_sensor, pm10, MIN_SENSOR_LIMIT, MAX_PM10);

      unsigned int hcho = response[6] | response[5] << 8;
      publish_if_within_limits(hcho_sensor, hcho, MIN_SENSOR_LIMIT, MAX_HCHO);

      unsigned int tvoc = response[8] | response[7] << 8;
      publish_if_within_limits(tvoc_sensor, tvoc, MIN_SENSOR_LIMIT, MAX_TVOC);

      unsigned int co2 = response[10] | response[9] << 8;
      publish_if_within_limits(co2_sensor, co2, MIN_CO2, MAX_CO2);

      double temperature = (response[12] | response[11] << 8) / 100.0;
      publish_if_within_limits(temperature_sensor, temperature, MIN_TEMPERATURE, MAX_TEMPERATURE);

      double humidity = (response[14] | response[13] << 8) / 100.0;
      publish_if_within_limits(humidity_sensor, humidity, MIN_HUMIDITY, MAX_HUMIDITY);

      // PPM counters
      unsigned int ppm03 = response[20] | response[19] << 8;
      publish_if_within_limits(ppm03_sensor, ppm03, MIN_SENSOR_LIMIT, MAX_PPM03);

      unsigned int ppm05 = response[22] | response[21] << 8;
      publish_if_within_limits(ppm05_sensor, ppm05, MIN_SENSOR_LIMIT, MAX_PPM05);

      unsigned int ppm1 = response[24] | response[23] << 8;
      publish_if_within_limits(ppm1_sensor, ppm1, MIN_SENSOR_LIMIT, MAX_PPM1);

      unsigned int ppm25 = response[26] | response[25] << 8;
      publish_if_within_limits(ppm25_sensor, ppm25, MIN_SENSOR_LIMIT, MAX_PPM25);

      unsigned int ppm5 = response[28] | response[27] << 8;
      publish_if_within_limits(ppm5_sensor, ppm5, MIN_SENSOR_LIMIT, MAX_PPM5);

      unsigned int ppm10 = response[30] | response[29] << 8;
      publish_if_within_limits(ppm10_sensor, ppm10, MIN_SENSOR_LIMIT, MAX_PPM10);

      // write request command
      const uint8_t command[] = {0x55, 0xCD, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x69, 0x0D, 0x0A};
      this->write_array(command, 13);
    }

  } // namespace airmaster
} // namespace esphome