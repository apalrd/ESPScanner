/* This file defines network information  */
#ifndef __NET_INFO_H
#define __NET_INFO_H

/* MQTT server information */
#define MQTT_SERVER "telstar.palnet.net"
#define MQTT_PORT 1883
#define MQTT_RECONNECT_TIME 5000
//#define MQTT_PREFIX "raw/"
//#define MQTT_USE_AUTH
//#define MQTT_AUTH_USER
//#define MQTT_AUTH_PASSWD

/* Include CCS811 sensor
 * Configure polling rate
 */
#define SENSOR_CCS811
#define SENSOR_CCS811_POLL_RATE 20

/* Include BME680 sensor 
 * Configure polling rate
 */
#define SENSOR_BME680
#define SENSOR_BME680_POLL_RATE 20

/* Include Plantower PMS sensors
 * Configure RX and TX pins, and polling rate (sec)
 */
#define SENSOR_PMS
#define SENSOR_PMS_RX 36
#define SENSOR_PMS_TX 34
#define SENSOR_PMS_POLL_RATE (60*2)

/* Include BLE scanner support
 * Note that enabling this requires building in
 * 'Large APP' mode, since the BLE libraries are huge
 * Configure polling rate, polling duration, and active mode
 */
//#define SENSOR_BLE
//#define SENSOR_BLE_POLL_RATE 20
//#define SENSOR_BLE_SCAN_LENGTH 5
//#define SENSOR_BLE_SCAN_ACTIVE false

/* Include Counter support
 * Configure scan rate for counter (to sample discretes)
 * Configure settling time in samples (consecutive samples to change state - 1 to 31)
 * Configure number of channels and pins for each channel 
 */
//#define SENSOR_COUNTER
//#define SENSOR_COUNTER_POLL_RATE 0.01
//#define SENSOR_COUNTER_VOTE_SAMPLES 4
//#define SENSOR_COUNTER_CHANNELS {4, 5}

#endif
