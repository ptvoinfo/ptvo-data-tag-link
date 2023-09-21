## About

This example implements a connection with "UART Sensor" in the [PTVO firmware](https://ptvo.info/zigbee-configurable-firmware-features/uart/uart-sensors-2/) and allows you to add new sensors to a Zigbee network. 

* Endpoint #2: On/Off status.
* Endpoint #3: Analog value from any external sensor.
* Endpoint #4: Level control (0 - 255).
* Endpoint #5: Button clicks.

## How to start

1. Configure and flash a Zigbee firmware for your module. You can find the configuration prototype in "ptvo_firmware_config.ini" and load it in "File - Read configuration from a file".
2. Connect UART pins from your module to Arduino or a similar external MCU.
3. Compile and upload this sketch to your MCU.
4. Use [Zigbee2MQTT](https://www.zigbee2mqtt.io/) as a coordinator because it is friendly for testing.
5. Open the web interface of Zigbee2MQTT.
6. Join the Zigbee module to your Zigbee network.
7. Find your device Zigbee2MQTT, go to the device settings, switch to the "Exposes" tab, and control your MCU with existing standard controls.

## How to connect

### Arduino Nano + CC2530

Arduino RX ↔ CC2530 P03 (TX)

Arduino TX ↔ CC2530 P02 (RX)

### Arduino Nano + CC2652(1) or CC1352

This chip can use any pins for UART. The example above uses DIO12 for RX and DIO13 for TX.

Arduino RX ↔ CC2652 P13 (TX)

Arduino TX ↔ CC2652 P12 (RX)

## Notes

This code cannot work with the power saving firmware (PSM) in PTVO without additional modifications.