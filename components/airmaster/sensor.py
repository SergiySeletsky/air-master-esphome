import esphome.codegen as cg
import voluptuous as vol
from esphome import core
import esphome.config_validation as cv
from esphome.components import sensor, uart, output
from esphome.const import (
    CONF_ID, CONF_SENSORS, CONF_UPDATE_INTERVAL,
    UNIT_MICROGRAMS_PER_CUBIC_METER, ICON_GAUGE, DEVICE_CLASS_PM25,
    DEVICE_CLASS_TEMPERATURE, DEVICE_CLASS_HUMIDITY, UNIT_PARTS_PER_MILLION,
    UNIT_CELSIUS, UNIT_PERCENT, ICON_THERMOMETER, ICON_WATER_PERCENT,
)

DEPENDENCIES = ['uart']
AUTO_LOAD = ['sensor', 'output']

airmaster_ns = cg.esphome_ns.namespace('airmaster')
AirMasterSensor = airmaster_ns.class_('AirMasterSensor', cg.PollingComponent, uart.UARTDevice)

LED_OUTPUT_SCHEMA = vol.Schema({
    vol.Required(CONF_ID): core.ID,
})

SENSOR_TYPES = {
    'pm25_sensor': [UNIT_MICROGRAMS_PER_CUBIC_METER, ICON_GAUGE, 2, DEVICE_CLASS_PM25],
    'pm10_sensor': [UNIT_MICROGRAMS_PER_CUBIC_METER, ICON_GAUGE, 2, None],
    'hcho_sensor': [UNIT_MICROGRAMS_PER_CUBIC_METER, ICON_GAUGE, 2, None],
    'tvoc_sensor': [UNIT_MICROGRAMS_PER_CUBIC_METER, ICON_GAUGE, 2, None],
    'co2_sensor': [UNIT_PARTS_PER_MILLION, ICON_GAUGE, 2, None],
    'temperature_sensor': [UNIT_CELSIUS, ICON_THERMOMETER, 2, DEVICE_CLASS_TEMPERATURE],
    'humidity_sensor': [UNIT_PERCENT, ICON_WATER_PERCENT, 2, DEVICE_CLASS_HUMIDITY],
    'ppm03_sensor': [UNIT_PARTS_PER_MILLION, ICON_GAUGE, 0, None],
    'ppm05_sensor': [UNIT_PARTS_PER_MILLION, ICON_GAUGE, 0, None],
    'ppm1_sensor': [UNIT_PARTS_PER_MILLION, ICON_GAUGE, 0, None],
    'ppm2_sensor': [UNIT_PARTS_PER_MILLION, ICON_GAUGE, 0, None],
    'ppm5_sensor': [UNIT_PARTS_PER_MILLION, ICON_GAUGE, 0, None],
    'ppm10_sensor': [UNIT_PARTS_PER_MILLION, ICON_GAUGE, 0, None],
}

# Generate sensor configurations
def get_sensor_configs():
    sensor_configs = vol.Schema({
        vol.Optional(sensor_name): sensor.SENSOR_SCHEMA.extend({
            vol.Optional('unit_of_measurement'): cv.string,
            vol.Optional('icon'): cv.icon,
            vol.Optional('accuracy_decimals'): cv.int,
            vol.Optional('device_class'): cv.string,
        }) for sensor_name, (unit, icon, decimals, device_class) in SENSOR_TYPES.items()
    })
    return sensor_configs

sensor_configs = get_sensor_configs()

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(AirMasterSensor),
}).extend({
    vol.Optional(CONF_UPDATE_INTERVAL): cv.update_interval,
    vol.Optional("led_output"): LED_OUTPUT_SCHEMA,
}).extend(sensor_configs).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)
    
    if "led_output" in config:
        led = yield cg.get_variable(config["led_output"])
        cg.add(var.set_led_output(led))