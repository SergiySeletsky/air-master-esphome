import esphome.codegen as cg
import voluptuous as vol
from esphome.components import sensor
from esphome.core import coroutine
from . import AirMasterSensor, airmaster_ns
from esphome.const import (
    CONF_ID,
    UNIT_MICROGRAMS_PER_CUBIC_METER,
    ICON_GAUGE,
    DEVICE_CLASS_PM25,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY, 
    UNIT_PARTS_PER_MILLION,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    ICON_THERMOMETER,
    ICON_WATER_PERCENT,
)

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

@coroutine
def to_code(config):
    var = yield cg.get_variable(config[CONF_ID])
    for key, (name, unit, icon, accuracy, device_class) in SENSOR_TYPES.items():
        if key in config:
            sensor_conf = config[key]
            sens = yield sensor.new_sensor(sensor_conf)
            cg.add(getattr(var, f'set_{key}')(sens))