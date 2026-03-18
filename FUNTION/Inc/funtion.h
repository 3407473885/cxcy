/**
 * @file funtion.h
 * @author 刘畅 (3407473885@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef __FUNTION_H__
#define __FUNTION_H__

#include "main.h"
#include "kalman_filter.h"

typedef enum {
    ALARM_NONE = 0,
    ALARM_LOW,      // 黄色警告
    ALARM_MEDIUM,   // 橙色警告
    ALARM_HIGH      // 红色警告（紧急）
}AlarmLevel;

float Get_vol(void);

float Get_Current(void);

void Remaining_battery_show(float cur_vol);

float Get_Temp(void);

void Beep_Handle(void);

void KalmanFilter_Handle(KalmanFilter* kf1, KalmanFilter* kf2, KalmanFilter* kf3, float vol, float current, float temp);

AlarmLevel Check_Battery_Alarm(AlarmLevel level);

void Trigger_Alarm(AlarmLevel level);

#endif
