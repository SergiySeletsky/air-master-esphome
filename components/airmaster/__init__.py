import esphome.codegen as cg
import voluptuous as vol
from esphome import core
from esphome.components import sensor, uart, output
from esphome.const import (
    CONF_ID,
    UNIT_PARTS_PER_MILLION,
    UNIT_MICROGRAMS_PER_CUBIC_METER,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    ICON_THERMOMETER,
    ICON_WATER_PERCENT,
    ICON_GAUGE,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_PM25,
    CONF_SENSORS,
    CONF_UNIT_OF_MEASUREMENT,
    CONF_ICON,
    CONF_ACCURACY_DECIMALS,
)

DEPENDENCIES = ['uart']
AUTO_LOAD = ['sensor', 'output']

airmaster_ns = cg.esphome_ns.namespace('airmaster')
AirMasterSensor = airmaster_ns.class_('AirMasterSensor', cg.PollingComponent, uart.UARTDevice)

# Configuration for each sensor type
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

CONFIG_SCHEMA = sensor.sensor_schema(AirMasterSensor)

# Dynamically extend the schema with sensor configurations
for sensor_name, (unit, icon, decimals, device_class) in SENSOR_TYPES.items():
    CONFIG_SCHEMA.extend({
        vol.Optional(sensor_name): sensor.sensor_schema(
            unit_of_measurement=unit,
            icon=icon,
            accuracy_decimals=decimals,
            device_class=device_class
        )
    })

LED_OUTPUT_SCHEMA = vol.Schema({
    vol.Required(CONF_ID): core.ID,
})

# Extend the schema with LED output configuration
CONFIG_SCHEMA.extend({
    vol.Optional("led_output"): LED_OUTPUT_SCHEMA,
})

# Extend the schema with UART device configuration
CONFIG_SCHEMA.extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)

    for sensor_name, (unit, icon, decimals, device_class) in SENSOR_TYPES.items():
        if sensor_name in config:
            conf = config[sensor_name]
            sens = yield sensor.new_sensor(conf)
            cg.add(getattr(var, f'set_{sensor_name}')(sens))
    
    if "led_output" in config:
        led_conf = config["led_output"]
        led = yield cg.get_variable(led_conf[CONF_ID])
        cg.add(var.set_led_output(led))