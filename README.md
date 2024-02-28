# ESPHome AirMaster Sensor Component

![image](https://github.com/SergiySeletsky/air-master-am7-esphome/assets/1207782/6cc0a264-de88-4611-bc83-6bea4b61ea04)

## Description
This custom component for ESPHome enables integration with AirMaster sensors, providing detailed air quality measurements including PM2.5, PM10, HCHO, TVOC, CO2, temperature, humidity, and particle counts.

## Configuration
To use this component, add the following configuration to your ESPHome YAML file: esp8266-example.yaml

![Untitled](https://github.com/SergiySeletsky/air-master-am7-esphome/assets/1207782/091c8580-1095-4036-999b-27a351b6e21c)

Here is image where main UART located for communication with esp8266
![image](https://github.com/SergiySeletsky/air-master-am7-esphome/assets/1207782/f610847c-1fbe-4455-98dc-433c14e88787)

## Hardware
https://www.elecbee.com/en-24919-AM7-Plus-Indoor-Air-Quality-Detector-Meter-PM2-5-PM10-HCHO-TVOC-CO2-Temp-RH-Monitor-Laser-Gas-Sensor-Tester 

## Known issues
First power up esp8266 than connect UART and keep it powered from AirMaster, otherwise AirMaster interrupt boot sequence.
