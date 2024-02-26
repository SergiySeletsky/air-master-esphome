import esphome.codegen as cg
import voluptuous as vol
from esphome import core
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import (
    CONF_ID, CONF_SENSORS, CONF_UPDATE_INTERVAL,
    UNIT_MICROGRAMS_PER_CUBIC_METER, ICON_GAUGE, DEVICE_CLASS_PM25,
    DEVICE_CLASS_TEMPERATURE, DEVICE_CLASS_HUMIDITY, UNIT_PARTS_PER_MILLION,
    UNIT_CELSIUS, UNIT_PERCENT, ICON_THERMOMETER, ICON_WATER_PERCENT,
    DEVICE_CLASS_PM10, DEVICE_CLASS_VOLATILE_ORGANIC_COMPOUNDS,
    DEVICE_CLASS_CARBON_DIOXIDE, DEVICE_CLASS_PM1,
    ICON_GRAIN, ICON_FLASK, ICON_CHEMICAL_WEAPON, ICON_MOLECULE_CO2,
    STATE_CLASS_MEASUREMENT,
)

DEPENDENCIES = ['uart']

airmaster_ns = cg.esphome_ns.namespace('airmaster')
AirMasterSensor = airmaster_ns.class_('AirMasterSensor', cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(AirMasterSensor),
            cv.Optional("pm25_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_MICROGRAMS_PER_CUBIC_METER,
                icon=ICON_GRAIN,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_PM25,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("pm10_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_MICROGRAMS_PER_CUBIC_METER,
                icon=ICON_GRAIN,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_PM10,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("hcho_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_MICROGRAMS_PER_CUBIC_METER,
                icon=ICON_FLASK,
                accuracy_decimals=2,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("tvoc_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_MICROGRAMS_PER_CUBIC_METER,
                icon=ICON_CHEMICAL_WEAPON,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_VOLATILE_ORGANIC_COMPOUNDS,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("co2_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                icon=ICON_MOLECULE_CO2,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_CARBON_DIOXIDE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("temperature_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                icon=ICON_THERMOMETER,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("humidity_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                icon=ICON_WATER_PERCENT,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            # ppm sensors
            cv.Optional("ppm03_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                icon=ICON_GRAIN,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("ppm05_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                icon=ICON_GRAIN,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("ppm1_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                icon=ICON_GRAIN,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_PM1,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("ppm25_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                icon=ICON_GRAIN,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_PM25,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("ppm5_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                icon=ICON_GRAIN,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("ppm10_sensor"): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                icon=ICON_GRAIN,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_PM10,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if "pm25_sensor" in config:
        sens = await sensor.new_sensor(config["pm25_sensor"])
        cg.add(var.set_pm25_sensor(sens))
    if "pm10_sensor" in config:
        sens = await sensor.new_sensor(config["pm10_sensor"])
        cg.add(var.set_pm10_sensor(sens))
    if "hcho_sensor" in config:
        sens = await sensor.new_sensor(config["hcho_sensor"])
        cg.add(var.set_hcho_sensor(sens))
    if "tvoc_sensor" in config:
        sens = await sensor.new_sensor(config["tvoc_sensor"])
        cg.add(var.set_tvoc_sensor(sens))
    if "co2_sensor" in config:
        sens = await sensor.new_sensor(config["co2_sensor"])
        cg.add(var.set_co2_sensor(sens))
    if "temperature_sensor" in config:
        sens = await sensor.new_sensor(config["temperature_sensor"])
        cg.add(var.set_temperature_sensor(sens))
    if "humidity_sensor" in config:
        sens = await sensor.new_sensor(config["humidity_sensor"])
        cg.add(var.set_humidity_sensor(sens))
    if "ppm03_sensor" in config:
        sens = await sensor.new_sensor(config["ppm03_sensor"])
        cg.add(var.set_ppm03_sensor(sens))
    if "ppm05_sensor" in config:
        sens = await sensor.new_sensor(config["ppm05_sensor"])
        cg.add(var.set_ppm05_sensor(sens))
    if "ppm1_sensor" in config:
        sens = await sensor.new_sensor(config["ppm1_sensor"])
        cg.add(var.set_ppm1_sensor(sens))
    if "ppm25_sensor" in config:
        sens = await sensor.new_sensor(config["ppm25_sensor"])
        cg.add(var.set_ppm25_sensor(sens))
    if "ppm5_sensor" in config:
        sens = await sensor.new_sensor(config["ppm5_sensor"])
        cg.add(var.set_ppm5_sensor(sens))
    if "ppm10_sensor" in config:
        sens = await sensor.new_sensor(config["ppm10_sensor"])
        cg.add(var.set_ppm10_sensor(sens))