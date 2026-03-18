#include "funtion.h"


/**
 * @brief 获取电压
 * 
 * @return float 
 */
float Get_vol(void)
{
    if(HAL_ADC_Start(&hadc1) == HAL_OK && HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
    {
        return (HAL_ADC_GetValue(&hadc1) / 4096.0) * 5;
    }
    return 0;
}

/**
 * @brief 获取电流
 * 
 * @return float 
 */
float Get_Current(void)
{
    if(HAL_ADC_Start(&hadc2) == HAL_OK && HAL_ADC_PollForConversion(&hadc2, 10) == HAL_OK)
    {
        return (HAL_ADC_GetValue(&hadc1) / 4096.0) * 5 * 1.25;
    }
    return 0;
}


/**
 * @brief 剩余电量显示
 * 
 */
void Remaining_battery_show(float cur_vol)
{
    uint8_t led_num = cur_vol / 5;
    for (uint8_t i = 1; i <= 10; i++)
    {
        led_show(i, 0);
    }
    for (uint8_t i = 1; i <= led_num; i++)
    {
        led_show(i, 1);
    }
    soc_estimate = cur_vol / 5 * 100;
}


/**
 * @brief 获取温度
 * 
 * @return float 
 */
float Get_Temp(void)
{
   static BMP280_DataTypedef bmp280 = {0};

   bmp280 = bmp280_Read(&hi2c1, bmp280);

   return bmp280.temperature;
}


/**
 * @brief 蜂鸣器处理函数
 * 
 */
void Beep_Handle(void)
{
    HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
}
/**
 * @brief 卡尔曼滤波处理函数
 * 
 */
void KalmanFilter_Handle(KalmanFilter* kf1, KalmanFilter* kf2, KalmanFilter* kf3, float vol, float current, float temp)
{
    filtered_voltage = KalmanFilter_Update(kf1, vol);
    filtered_current = KalmanFilter_Update(kf2, current);
    filtered_temperature = KalmanFilter_Update(kf3, temp);
}





/**
 * @brief 检查电池状态并返回报警级别
 */
AlarmLevel Check_Battery_Alarm(AlarmLevel level)
{

    // 红色报警：严重危险
    if (filtered_voltage > 4.2f || filtered_voltage < 2.8f) {
        return ALARM_HIGH;
    }

    if (filtered_temperature > 70.0f) {
        TIM1->CCR1 = 100;
        return ALARM_HIGH;
    }

    // 橙色报警：异常行为
    if (filtered_current > 5.0f) {  // 过流
        level = ALARM_MEDIUM;
    }
    if (filtered_temperature > 55.0f) {
        TIM1->CCR1 = (70 - filtered_temperature) * 10;
        level = ALARM_MEDIUM;
    }

    // 黄色报警：需关注
    if (soc_estimate < 20.0f) {
        level = ALARM_LOW;
    }

    return level;
}

/**
 * @brief 执行报警响应动作
 */
void Trigger_Alarm(AlarmLevel level)
{
    HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
    switch (level) {
        case ALARM_LOW:
            HAL_GPIO_WritePin(alarm_LED_GPIO_Port, alarm_LED_Pin, GPIO_PIN_SET);     // 黄灯亮
            break;
        case ALARM_MEDIUM:
            HAL_GPIO_TogglePin(alarm_LED_GPIO_Port, alarm_LED_Pin);                  // 快闪
            break;
        case ALARM_HIGH:
            Beep_Handle();
            break;
        default:
            HAL_GPIO_WritePin(alarm_LED_GPIO_Port, alarm_LED_Pin, GPIO_PIN_RESET);  // 熄灭
            TIM1->CCR1 = 0;
            break;
    }
}
