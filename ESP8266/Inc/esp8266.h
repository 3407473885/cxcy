/**
 * @file esp8266.h
 * @author 刘畅 (3407473885@qq.com)
 * @brief esp8266接口
 * @version 0.1
 * @date 2025-07-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __ESP8266_H
#define __ESP8266_H

#include "main.h"
#include "string.h"
#include "usart.h"
#include <stdio.h>

#define ESP8266_TX_PORT        GPIOB
#define ESP8266_TX_PIN         GPIO_PIN_11

#define ESP8266_RX_PORT        GPIOB
#define ESP8266_RX_PIN         GPIO_PIN_10

#define WIFI_SSID       "iQOO12"
#define WIFI_PWD        "34074700"

#define MQTT_USERNAME   "ESP32&k1qq30jp47P"
#define MQTT_PASSWORD   "a9782e53df0bb63399c2e9d6474f7066b5c614dc84905190face21a68863d28c"
#define MQTT_CLIENTID   "k1qq30jp47P.ESP32|securemode=2\\,signmethod=hmacsha256\\,timestamp=1753105472671|"
#define MQTT_HOSTURL    "iot-06z00agyb6a4b71.mqtt.iothub.aliyuncs.com"


uint8_t ESP8266_Init(void);

uint8_t ESP8266_SendCmd(const char *cmd, const char *expectation);

uint8_t ESP8266_ConnectWifi(const char *ssid, const char *pwd);

uint8_t ESP8266_ConnectMqttAliyun(const char *username, const char *password, const char *clientId, const char *mqttHostUrl);

uint8_t ESP8266_Publish(const char* topic, const char* content);

uint8_t ESP8266_SubscribeTopic(const char *topic);
#endif
