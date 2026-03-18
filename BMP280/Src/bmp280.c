#include "bmp280.h"

bmp280Calib bmp280Cal;

static uint8_t bmp280_ReadCalibrationData(I2C_HandleTypeDef *i2c)
{
    uint8_t data[24];

    if (HAL_I2C_Mem_Read(i2c, BMP280_ADDR, 0x88, 1, data, 24, 100) != HAL_OK) {
        return 0;
    }

    // 解析校准数据--小端格式
    bmp280Cal.dig_T1 = (data[1] << 8) | data[0];
    bmp280Cal.dig_T2 = (data[3] << 8) | data[2];
    bmp280Cal.dig_T3 = (data[5] << 8) | data[4];
    bmp280Cal.dig_P1 = (data[7] << 8) | data[6];
    bmp280Cal.dig_P2 = (data[9] << 8) | data[8];
    bmp280Cal.dig_P3 = (data[11] << 8) | data[10];
    bmp280Cal.dig_P4 = (data[13] << 8) | data[12];
    bmp280Cal.dig_P5 = (data[15] << 8) | data[14];
    bmp280Cal.dig_P6 = (data[17] << 8) | data[16];
    bmp280Cal.dig_P7 = (data[19] << 8) | data[18];
    bmp280Cal.dig_P8 = (data[21] << 8) | data[20];
    bmp280Cal.dig_P9 = (data[23] << 8) | data[22];

    return 1;
}


/**
 * @brief bmp280初始化
 * 
 * @param i2c I2C句柄
 * @return uint8_t 0代表初始化失败，1代表初始化成功
 */
uint8_t bmp280_Init(I2C_HandleTypeDef *i2c)
{
     uint8_t id;

    // 读取芯片ID（确认通信正常）
    if (HAL_I2C_Mem_Read(i2c, BMP280_ADDR, BMP280_REG_ID, 1, &id, 1, 100) != HAL_OK) {
        return 0;
    }
    if (id != BMP280_CHIP_ID) {
        return 0; // 不是 BMP280
    }

    // 软复位
    uint8_t reset_cmd = BMP280_RESET_CMD;
    if (HAL_I2C_Mem_Write(i2c, BMP280_ADDR, BMP280_REG_RESET, 1, &reset_cmd, 1, 100) != HAL_OK) {
        return 0;
    }
    HAL_Delay(5); // 等待复位完成

    // 设置测量模式（温度×2, 压力×16, Normal）
    uint8_t ctrl_meas = BMP280_CTRL_MEAS;
    if (HAL_I2C_Mem_Write(i2c, BMP280_ADDR, BMP280_REG_CTRL, 1, &ctrl_meas, 1, 100) != HAL_OK) {
        return 0;
    }

    //设置滤波和待机时间
    uint8_t config = BMP280_CONFIG;
    if (HAL_I2C_Mem_Write(i2c, BMP280_ADDR, BMP280_REG_CONFIG, 1, &config, 1, 100) != HAL_OK) {
        return 0;
    }

    if(bmp280_ReadCalibrationData(i2c) == 0) return 0;

    return 1; // 初始化成功
}

static int32_t BMP280CompensateT(int32_t adc_temp)
{
    int32_t var1, var2, T;

    var1 = ((((adc_temp >> 3) - ((int32_t)bmp280Cal.dig_T1 << 1))) * ((int32_t)bmp280Cal.dig_T2)) >> 11;
    var2  = (((((adc_temp >> 4) - ((int32_t)bmp280Cal.dig_T1)) * ((adc_temp >> 4) - ((int32_t)bmp280Cal.dig_T1))) >> 12) * ((int32_t)bmp280Cal.dig_T3)) >> 14;
    bmp280Cal.t_fine = var1 + var2;

    T = (bmp280Cal.t_fine * 5 + 128) >> 8;

    return (int32_t)T;
}


static int32_t BMP280CompensateP(int32_t adc_press)
{
    int64_t var1, var2, p;
    var1 = ((int64_t)bmp280Cal.t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)bmp280Cal.dig_P6;
    var2 = var2 + ((var1*(int64_t)bmp280Cal.dig_P5) << 17);
    var2 = var2 + (((int64_t)bmp280Cal.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)bmp280Cal.dig_P3) >> 8) + ((var1 * (int64_t)bmp280Cal.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)bmp280Cal.dig_P1) >> 33;
    if (var1 == 0)
        return 0;
    p = 1048576 - adc_press;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)bmp280Cal.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)bmp280Cal.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)bmp280Cal.dig_P7) << 4);
    return (int32_t)p;
}


/**
 * @brief 读取数据
 * 
 * @param i2c I2句柄
 * @param bmp280_data 数据集
 * @return BMP280_DataTypedef 返回数据
 */
BMP280_DataTypedef bmp280_Read(I2C_HandleTypeDef *i2c, BMP280_DataTypedef bmp280_data)
{
    uint8_t data[6];
    int32_t adc_temp, adc_press;
    if (HAL_I2C_Master_Transmit(i2c, BMP280_ADDR, (uint8_t[]){0xF7}, 1, 100) != HAL_OK) {
        bmp280_data.state = 0;
    }

    if (HAL_I2C_Master_Receive(i2c, BMP280_ADDR | 0x01, data, 6, 100) != HAL_OK) {
        bmp280_data.state = 0;
    }
    adc_press = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    adc_temp = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);

    bmp280_data.pressure = BMP280CompensateP(adc_press) / 25600.0;
    bmp280_data.temperature = BMP280CompensateT(adc_temp) / 100.0;
    bmp280_data.state = 1;

    return bmp280_data;
}
