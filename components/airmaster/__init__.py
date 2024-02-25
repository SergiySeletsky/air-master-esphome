import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import core
import voluptuous as vol
from esphome.components import sensor, uart, output
from esphome.const import CONF_ID, CONF_SENSORS, CONF_UPDATE_INTERVAL
from .sensor import SENSOR_TYPES, create_sensor_schema

DEPENDENCIES = ['uart']
AUTO_LOAD = ['sensor', 'output']

airmaster_ns = cg.esphome_ns.namespace('airmaster')
AirMasterSensor = airmaster_ns.class_('AirMasterSensor', cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = sensor.sensor_schema(AirMasterSensor).extend({
    vol.Optional(CONF_UPDATE_INTERVAL): cv.update_interval,
    vol.Optional("led_output"): cv.use_id(output.OutputComponent),
}).extend(create_sensor_schema(SENSOR_TYPES)).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)
    
    if "led_output" in config:
        led = yield cg.get_variable(config["led_output"])
        cg.add(var.set_led_output(led))