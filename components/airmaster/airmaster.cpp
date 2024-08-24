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

    // Define the structure for AirMaster data
    struct AirMasterData {
        uint16_t pm25;             // Particulate matter 2.5
        uint16_t pm10;             // Particulate matter 10
        uint16_t hcho;             // Formaldehyde concentration
        uint16_t tvoc;             // Total Volatile Organic Compounds
        uint16_t co2;              // Carbon Dioxide concentration
        uint16_t temperature_raw;  // Raw temperature data (multiplied by 100)
        uint16_t humidity_raw;     // Raw humidity data (multiplied by 100)
        uint16_t ppm03;            // PPM 0.3
        uint16_t ppm05;            // PPM 0.5
        uint16_t ppm1;             // PPM 1.0
        uint16_t ppm25;            // PPM 2.5
        uint16_t ppm5;             // PPM 5.0
        uint16_t ppm10;            // PPM 10.0
        uint8_t reserved[4];       // Reserved bytes (could be padding or unused)
        uint16_t checksum;         // Checksum for data validation
    };

    void publish_if_within_limits(Sensor *sensor, unsigned int value, unsigned int min_limit, unsigned int max_limit)
    {
        if (value >= min_limit && value < max_limit) sensor->publish_state(value);
    }

    void publish_if_within_limits(Sensor *sensor, double value, double min_limit, double max_limit)
    {
        if (value > min_limit && value < max_limit) sensor->publish_state(value);
    }

    void AirMasterSensor::update()
    {
      uint8_t response[AIRMASTER_RESPONSE_LENGTH];

      while (this->available() > 0 && this->peek_byte(&response[0]) && response[0] != 170)
        this->read();

      bool read_success = read_array(response, AIRMASTER_RESPONSE_LENGTH);

      if (!read_success)
      {
        ESP_LOGW(TAG, "Reading data from AirMaster failed!");
        return; // Exit the function if read failed
      }

      // Cast the response to the AirMasterData struct
      const AirMasterData* data = reinterpret_cast<const AirMasterData*>(response);

      unsigned int received_checksum = data->checksum;
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
      publish_if_within_limits(pm25_sensor, data->pm25, MIN_SENSOR_LIMIT, MAX_PM25);
      publish_if_within_limits(pm10_sensor, data->pm10, MIN_SENSOR_LIMIT, MAX_PM10);
      publish_if_within_limits(hcho_sensor, data->hcho, MIN_SENSOR_LIMIT, MAX_HCHO);
      publish_if_within_limits(tvoc_sensor, data->tvoc, MIN_SENSOR_LIMIT, MAX_TVOC);
      publish_if_within_limits(co2_sensor, data->co2, MIN_CO2, MAX_CO2);

      double temperature = data->temperature_raw / 100.0;
      publish_if_within_limits(temperature_sensor, temperature, MIN_TEMPERATURE, MAX_TEMPERATURE);

      double humidity = data->humidity_raw / 100.0;
      publish_if_within_limits(humidity_sensor, humidity, MIN_HUMIDITY, MAX_HUMIDITY);

      publish_if_within_limits(ppm03_sensor, data->ppm03, MIN_SENSOR_LIMIT, MAX_PPM03);
      publish_if_within_limits(ppm05_sensor, data->ppm05, MIN_SENSOR_LIMIT, MAX_PPM05);
      publish_if_within_limits(ppm1_sensor, data->ppm1, MIN_SENSOR_LIMIT, MAX_PPM1);
      publish_if_within_limits(ppm25_sensor, data->ppm25, MIN_SENSOR_LIMIT, MAX_PPM25);
      publish_if_within_limits(ppm5_sensor, data->ppm5, MIN_SENSOR_LIMIT, MAX_PPM5);
      publish_if_within_limits(ppm10_sensor, data->ppm10, MIN_SENSOR_LIMIT, MAX_PPM10);

      // write request command
      const uint8_t command[] = {0x55, 0xCD, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x69, 0x0D, 0x0A};
      this->write_array(command, 13);
    }

  } // namespace airmaster
} // namespace esphome
