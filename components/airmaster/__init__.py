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
    # Additional sensor configuration schema extensions
}).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)
    
    if "led_output" in config:
        led = yield cg.get_variable(config["led_output"])
        cg.add(var.set_led_output(led))
    # Additional code generation logic for sensors








# CONFIG_SCHEMA = sensor.sensor_schema(AirMasterSensor)

# # Dynamically extend the schema with sensor configurations
# for sensor_name, (unit, icon, decimals, device_class) in SENSOR_TYPES.items():
#     CONFIG_SCHEMA.extend({
#         vol.Optional(sensor_name): sensor.sensor_schema(
#             unit_of_measurement=unit,
#             icon=icon,
#             accuracy_decimals=decimals,
#             device_class=device_class
#         )
#     })

# LED_OUTPUT_SCHEMA = vol.Schema({
#     vol.Required(CONF_ID): core.ID,
# })

# # Extend the schema with LED output configuration
# CONFIG_SCHEMA.extend({
#     vol.Optional("led_output"): LED_OUTPUT_SCHEMA,
# })

# # Extend the schema with UART device configuration
# CONFIG_SCHEMA.extend(uart.UART_DEVICE_SCHEMA)

# def to_code(config):
#     var = cg.new_Pvariable(config[CONF_ID])
#     yield cg.register_component(var, config)
#     yield uart.register_uart_device(var, config)

#     for sensor_name, (unit, icon, decimals, device_class) in SENSOR_TYPES.items():
#         if sensor_name in config:
#             conf = config[sensor_name]
#             sens = yield sensor.new_sensor(conf)
#             cg.add(getattr(var, f'set_{sensor_name}')(sens))
    
#     if "led_output" in config:
#         led_conf = config["led_output"]
#         led = yield cg.get_variable(led_conf[CONF_ID])
#         cg.add(var.set_led_output(led))