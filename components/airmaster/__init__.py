import esphome.codegen as cg
from esphome import core
from esphome.components import sensor, uart, output
from esphome.const import CONF_ID

namespace = cg.esphome_ns.namespace('airmaster')
AirMasterSensor = namespace.class_('AirMasterSensor', cg.PollingComponent, uart.UARTDevice)

# Assuming you want to optionally configure an LED output for feedback
CONF_LED_OUTPUT = 'led_output'

CONFIG_SCHEMA = sensor.sensor_schema(AirMasterSensor).extend({
    # Extend with UART device schema for UART related configurations
    uart.UART_DEVICE_SCHEMA.extend({
        # Optionally configure an output component for LED feedback
        cg.Optional(CONF_LED_OUTPUT): output.output_schema(id_type=output.Output)
    }).extend(core.COMPONENT_SCHEMA)
})

@coroutine
def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)

    # Handle the optional LED output
    if CONF_LED_OUTPUT in config:
        led = yield cg.get_variable(config[CONF_LED_OUTPUT])
        cg.add(var.set_led_output(led))