# PalNode
Code for ESP32-POE based sensor nodes
Each sensor node can be configured to include any number of supported sensors, as long as there are no port conflicts. All sensors have configurable reading and reporting, and send data back using a shared MQTT connection. Supports only wired Ethernet at this time.

## Supported sensors
* Bosch BME680 environmental sensor, including Bosch library functions
* CCS811 environmental sensor
* Plantower PMS5003/PMS7003 particulate matter sensor
* Bluetooth Low Energy (BLE) scanner to capture devices around the device (Requires ESP32-POE to be connected via wired Ethernet)
