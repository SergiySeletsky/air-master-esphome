#pragma once

#include "esphome.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/output/binary_output.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace airmaster {

class AirMasterSensor : public PollingComponent, public uart::UARTDevice {
 public:
  // Constructor with UART parent and LED Output
  AirMasterSensor(uart::UARTComponent *parent, output::BinaryOutput *led_output)
      : uart::UARTDevice(parent), led_output(led_output) {}

  // Sensor pointers
  sensor::Sensor *pm25_sensor{nullptr};
  sensor::Sensor *pm10_sensor{nullptr};
  sensor::Sensor *hcho_sensor{nullptr};
  sensor::Sensor *tvoc_sensor{nullptr};
  sensor::Sensor *co2_sensor{nullptr};
  sensor::Sensor *temperature_sensor{nullptr};
  sensor::Sensor *humidity_sensor{nullptr};
  sensor::Sensor *ppm03_sensor{nullptr};
  sensor::Sensor *ppm05_sensor{nullptr};
  sensor::Sensor *ppm1_sensor{nullptr};
  sensor::Sensor *ppm2_sensor{nullptr};
  sensor::Sensor *ppm5_sensor{nullptr};
  sensor::Sensor *ppm10_sensor{nullptr};

  // Output for LED indicator
  output::BinaryOutput *led_output;

  // Component overrides
  void setup() override;
  void update() override;

  // Set sensor methods for each sensor
  void set_pm25_sensor(sensor::Sensor *pm25_sensor) { this->pm25_sensor = pm25_sensor; }
  void set_pm10_sensor(sensor::Sensor *pm10_sensor) { this->pm10_sensor = pm10_sensor; }
  void set_hcho_sensor(sensor::Sensor *hcho_sensor) { this->hcho_sensor = hcho_sensor; }
  void set_tvoc_sensor(sensor::Sensor *tvoc_sensor) { this->tvoc_sensor = tvoc_sensor; }
  void set_co2_sensor(sensor::Sensor *co2_sensor) { this->co2_sensor = co2_sensor; }
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { this->temperature_sensor = temperature_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { this->humidity_sensor = humidity_sensor; }
  void set_ppm03_sensor(sensor::Sensor *ppm03_sensor) { this->ppm03_sensor = ppm03_sensor; }
  void set_ppm05_sensor(sensor::Sensor *ppm05_sensor) { this->ppm05_sensor = ppm05_sensor; }
  void set_ppm1_sensor(sensor::Sensor *ppm1_sensor) { this->ppm1_sensor = ppm1_sensor; }
  void set_ppm2_sensor(sensor::Sensor *ppm2_sensor) { this->ppm2_sensor = ppm2_sensor; }
  void set_ppm5_sensor(sensor::Sensor *ppm5_sensor) { this->ppm5_sensor = ppm5_sensor; }
  void set_ppm10_sensor(sensor::Sensor *ppm10_sensor) { this->ppm10_sensor = ppm10_sensor; }
};

}  // namespace airmaster
}  // namespace esphome