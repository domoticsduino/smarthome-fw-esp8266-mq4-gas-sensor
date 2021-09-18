# Smarthome firmware for ESP8266 - MQ-4 Gas Sensor
Firmware for my board named **ESP8266MCU11**, based on **ESP8266 microcontroller**.

This device use a **MQ-4 gas sensor** and use **MQTT protocol** to send data to **OpenHAB Smart Home Automation System**.

Built with **platformio** (https://platformio.org) and **visual studio code** (https://code.visualstudio.com)

Depends on the following *dd libraries*:

 - ddcommon
 - ddwifi
 - ddmqtt
 - ddmq4

To build your firmware bin file:
 - clone this repository with the *--recursive* flag to checkout **dd-libraries**
 - rename file *src/user-config-template.h* in *src/user-config.h*
 - set your **WIFI** and **MQTT** settings in file *src/user-config.h*
 
Firmware file will be in ***.pio/build/esp12e/firmware.bin***