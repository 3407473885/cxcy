#include "led.h"


/**
 * @brief led控制函数
 * 
 * @param led_num 编号 1..10
 * @param led_state 灯的状态
 *          1:开灯
 *          0:关灯
 */
void led_show(uint8_t led_num, uint8_t led_state)
{
    static uint8_t led_sta = 0x00;

    if(led_state == 1)//开灯
    {
        led_sta |= (0x01 << (led_num - 1));
    }
    if(led_state == 0)//关灯
    {
        led_sta &= ~(0x01 << (led_num - 1));
    }
    
    HAL_GPIO_WritePin(GPIOD, led_sta, GPIO_PIN_RESET); 
    
}
