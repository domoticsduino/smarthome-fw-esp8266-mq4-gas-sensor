/* 1.0.0 VERSION */

#ifndef config_h
#define config_h

// USER SETTINGS
#include "user-config.h"

// GENERAL SETTINGS
#define LEDSTATUSPIN 2 // digital pin for status led
#define SERIAL_BAUDRATE 9600
#define READ_INTERVAL 5000; // ms
#define NUM_SAMPLES 12; // num data samples before send to MQTT
#define WARMUP_TIME 60; // seconds - MQ-4 sensor needs a warmup time

// MQ-4 SETTINGS
#define MQ4PIN A0 // analog pin for MQ-4 sensor

// WIFI SETTINGS (see user-config.h)
const char *ssid = USER_SETTINGS_WIFI_SSID;
const char *password = USER_SETTINGS_WIFI_PASSWORD;
const char *wifihostname = USER_SETTINGS_WIFI_HOSTNAME;

// MQTT SETTINGS (see user-config.h)
const char MQTT_HOST[] = USER_SETTINGS_MQTT_HOST;
int MQTT_PORT = USER_SETTINGS_MQTT_PORT;
const char TOPIC_P[] = USER_SETTINGS_MQTT_TOPIC_P;
const char TOPIC_S[] = USER_SETTINGS_MQTT_TOPIC_S;
const char DEVICE[] = USER_SETTINGS_MQTT_DEVICE;
const char MQTT_USER[] = USER_SETTINGS_MQTT_USER;
const char MQTT_PWD[] = USER_SETTINGS_MQTT_PWD;
int MQTT_QOS = USER_SETTINGS_MQTT_QOS;

#endif