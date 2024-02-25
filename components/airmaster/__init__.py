import esphome.codegen as cg
import voluptuous as vol
from esphome import core
from esphome.components import sensor, uart, output
from esphome.const import (
    CONF_ID,
)

DEPENDENCIES = ['uart']
AUTO_LOAD = ['sensor', 'output']

airmaster_ns = cg.esphome_ns.namespace('airmaster')
AirMasterSensor = airmaster_ns.class_('AirMasterSensor', cg.PollingComponent, uart.UARTDevice)

LED_OUTPUT = output.output_ns.class_('Output')

LED_OUTPUT_SCHEMA = vol.Schema({
    vol.Required(CONF_ID): core.ID,
})

CONFIG_SCHEMA = sensor.sensor_schema(AirMasterSensor).extend({
    vol.Optional("led_output"): LED_OUTPUT_SCHEMA,
}).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)
    
    if "led_output" in config:
        led = yield cg.get_variable(config["led_output"])
        cg.add(var.set_led_output(led))
