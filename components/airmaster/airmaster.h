#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/output/output.h"

namespace esphome {
namespace airmaster {

class AirMasterSensor : public PollingComponent, public uart::UARTDevice {
 public:
  sensor::Sensor *pm25_sensor = new sensor::Sensor();
  sensor::Sensor *pm10_sensor = new sensor::Sensor();
  sensor::Sensor *hcho_sensor = new sensor::Sensor();
  sensor::Sensor *tvoc_sensor = new sensor::Sensor();
  sensor::Sensor *co2_sensor = new sensor::Sensor();
  sensor::Sensor *temperature_sensor = new sensor::Sensor();
  sensor::Sensor *humidity_sensor = new sensor::Sensor();
  sensor::Sensor *ppm03_sensor = new sensor::Sensor();
  sensor::Sensor *ppm05_sensor = new sensor::Sensor();
  sensor::Sensor *ppm1_sensor = new sensor::Sensor();
  sensor::Sensor *ppm2_sensor = new sensor::Sensor();
  sensor::Sensor *ppm5_sensor = new sensor::Sensor();
  sensor::Sensor *ppm10_sensor = new sensor::Sensor();
  output::Output *led_output;

  AirMasterSensor(uart::UARTComponent *parent, output::Output *led_output);

  void setup() override;
  void update() override;
};

}  // namespace airmaster
}  // namespace esphome