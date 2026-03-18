/**
 * @file bmp280.h
 * @author 刘畅 (3407473885@qq.com)
 * @brief 温度传感器
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef __BMP280_H__
#define __BMP280_H__

#include "main.h"

typedef struct BMP280_Data
{
    uint8_t state;
    float pressure;     //气压
    float temperature;  //环境温度
    float altitude;     //海拔  
}BMP280_DataTypedef;


#define BMP280_ADDR     (0x77 << 1)
// 芯片ID寄存器地址
#define BMP280_REG_ID   0xD0
#define BMP280_CHIP_ID  0x58

// 配置寄存器地址
#define BMP280_REG_RESET 0xE0
#define BMP280_REG_CTRL  0xF4
#define BMP280_REG_CONFIG 0xF5

#define BMP280_RESET_CMD    0xB6                    // 复位命令
#define BMP280_CTRL_MEAS    0x27  // 0b00100111     // 温度×2, 压力×16, Normal mode
#define BMP280_CONFIG       0x90  // 0b10010000     // 滤波×16, standby=0.5ms

typedef struct
{
    uint16_t dig_T1;/* calibration T1 data */
    int16_t dig_T2; /* calibration T2 data */
    int16_t dig_T3; /* calibration T3 data */
    uint16_t dig_P1;/* calibration P1 data */
    int16_t dig_P2; /* calibration P2 data */
    int16_t dig_P3; /* calibration P3 data */
    int16_t dig_P4; /* calibration P4 data */
    int16_t dig_P5; /* calibration P5 data */
    int16_t dig_P6; /* calibration P6 data */
    int16_t dig_P7; /* calibration P7 data */
    int16_t dig_P8; /* calibration P8 data */
    int16_t dig_P9; /* calibration P9 data */
    int32_t t_fine; /* calibration t_fine data */
} bmp280Calib;


uint8_t bmp280_Init(I2C_HandleTypeDef *i2c);
BMP280_DataTypedef bmp280_Read(I2C_HandleTypeDef *i2c, BMP280_DataTypedef bmp280_data);

#endif
