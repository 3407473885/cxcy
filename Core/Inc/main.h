/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "bmp280.h"
#include "led.h"
#include "lcd.h"
#include "funtion.h"
#include "esp8266.h"
#include "kalman_filter.h"

#include "ft6336.h"
#include "touch.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_conf.h"
#include "lvgl.h"
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"

#include "stdio.h"
#include "string.h"

#include "cmsis_os.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ESP8266_RX_Pin GPIO_PIN_10
#define ESP8266_RX_GPIO_Port GPIOB
#define ESP8266_TX_Pin GPIO_PIN_11
#define ESP8266_TX_GPIO_Port GPIOB
#define LED8_Pin GPIO_PIN_8
#define LED8_GPIO_Port GPIOD
#define LED9_Pin GPIO_PIN_9
#define LED9_GPIO_Port GPIOD
#define alarm_LED_Pin GPIO_PIN_10
#define alarm_LED_GPIO_Port GPIOD
#define BEEP_Pin GPIO_PIN_11
#define BEEP_GPIO_Port GPIOD
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOD
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOD
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOD
#define LED4_Pin GPIO_PIN_4
#define LED4_GPIO_Port GPIOD
#define LED5_Pin GPIO_PIN_5
#define LED5_GPIO_Port GPIOD
#define LED6_Pin GPIO_PIN_6
#define LED6_GPIO_Port GPIOD
#define LED7_Pin GPIO_PIN_7
#define LED7_GPIO_Port GPIOD
#define LCD_RST_Pin GPIO_PIN_3
#define LCD_RST_GPIO_Port GPIOB
#define LCD_RS_Pin GPIO_PIN_4
#define LCD_RS_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_5
#define LED_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_8
#define LCD_CS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define u16     uint16_t
#define u8      uint8_t


extern float filtered_voltage;
extern float filtered_current;
extern float filtered_temperature;
extern float soc_estimate;

extern KalmanFilter kf1,kf2,kf3; // 卡尔曼滤波句柄

extern float filtered_voltage; //滤波后的电压
extern float filtered_current; //滤波后的电流
extern float filtered_temperature; //滤波后的温度
extern float soc_estimate; //剩余电量

extern float vol; //测量电压
extern float current; //测量电流
extern float temp; //测量温度

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
