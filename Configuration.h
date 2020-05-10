/* This file defines network information  */
#ifndef __NET_INFO_H
#define __NET_INFO_H

/* MQTT server information */
#define MQTT_SERVER "telstar.palnet.net"
#define MQTT_PORT 1883
#define MQTT_RECONNECT_TIME 5000
#define MQTT_PREFIX "raw/"

/* Include CCS811 sensor */
//#define SENSOR_CCS811

/* Include BME680 sensor */
//#define SENSOR_BME680

/* Include Plantower PMS sensors */
//#define SENSOR_PMS

/* Include BLE scanner support */
#define SENSOR_BLE

#endif
