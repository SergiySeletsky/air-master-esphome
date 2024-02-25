#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace airmaster {
class AirMasterSensor : public PollingComponent, public UARTDevice {
 public:
  Sensor *pm25_sensor = new Sensor();
  Sensor *pm10_sensor = new Sensor();
  Sensor *hcho_sensor = new Sensor();
  Sensor *tvoc_sensor = new Sensor();
  Sensor *co2_sensor = new Sensor();
  Sensor *temperature_sensor = new Sensor();
  Sensor *humidity_sensor = new Sensor();

  Sensor *ppm03_sensor = new Sensor();
  Sensor *ppm05_sensor = new Sensor();
  Sensor *ppm1_sensor = new Sensor();
  Sensor *ppm2_sensor = new Sensor();
  Sensor *ppm5_sensor = new Sensor();
  Sensor *ppm10_sensor = new Sensor();

  Output *led_output;

  AirMasterSensor(UARTComponent *parent, Output *led_output) : UARTDevice(parent), led_output(led_output) {}

  void setup() override {}

  void update() override {
    const uint8_t command[] = {0x55, 0xCD, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x69, 0x0D, 0x0A};
    write_array(command, sizeof(command));
    delay(1000);  // Wait for sensor response

    uint8_t buffer[40];
    if (available() >= 40) {
      read_bytes(buffer, 40);
      
      unsigned int received_checksum = buffer[37] | buffer[36] << 8;
      unsigned int calculated_checksum = 0;
      for (int i = 0; i < 36; i++) {
        calculated_checksum += buffer[i];
      }

      if (received_checksum == calculated_checksum) {
        // Process and publish sensor data
        pm25_sensor->publish_state((buffer[2] << 8) | buffer[1]);
        pm10_sensor->publish_state((buffer[4] << 8) | buffer[3]);
        hcho_sensor->publish_state((buffer[6] << 8) | buffer[5]);
        tvoc_sensor->publish_state((buffer[8] << 8) | buffer[7]);
        co2_sensor->publish_state((buffer[10] << 8) | buffer[9]);
        temperature_sensor->publish_state(((buffer[12] << 8) | buffer[11]) / 100.0);
        humidity_sensor->publish_state(((buffer[14] << 8) | buffer[13]) / 100.0);
        ppm03_sensor->publish_state((buffer[20] << 8) | buffer[19]);
        ppm05_sensor->publish_state((buffer[22] << 8) | buffer[21]);
        ppm1_sensor->publish_state((buffer[24] << 8) | buffer[23]);
        ppm2_sensor->publish_state((buffer[26] << 8) | buffer[25]);
        ppm5_sensor->publish_state((buffer[28] << 8) | buffer[27]);
        ppm10_sensor->publish_state((buffer[30] << 8) | buffer[29]);
        
        ESP_LOGD("air_master", "Data received: PM2.5: %d, PM10: %d, HCHO: %d, TVOC: %d, CO2: %d, Temperature: %.2f, Humidity: %.2f, 0.3um: %d, 0.5um: %d, 1um: %d, 2.5um: %d, 5um: %d, 10um: %d",
                 (buffer[2] << 8) | buffer[1], (buffer[4] << 8) | buffer[3], (buffer[6] << 8) | buffer[5], (buffer[8] << 8) | buffer[7], (buffer[10] << 8) | buffer[9],
                 ((buffer[12] << 8) | buffer[11]) / 100.0, ((buffer[14] << 8) | buffer[13]) / 100.0, (buffer[20] << 8) | buffer[19], (buffer[22] << 8) | buffer[21],
                 (buffer[24] << 8) | buffer[23], (buffer[26] << 8) | buffer[25], (buffer[28] << 8) | buffer[27], (buffer[30] << 8) | buffer[29]);
                 
        // LED feedback for valid data
        led_output->turn_on();
        delay(5000);
        led_output->turn_off();
      } else {
        // Checksum validation failed
        for(int i = 0; i < 3; i++) {
          led_output->turn_on();
          delay(50);
          led_output->turn_off();
          delay(50);
        }

        ESP_LOGD("air_master", "Reading data...");
      }
    }
  }
};
}
}