/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "gui_guider.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
const char *topic = "/sys/k1qq30jp47P/ESP32/thing/event/property/post";
lv_ui guider_ui;

AlarmLevel level = ALARM_NONE;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for sensor_Task */
osThreadId_t sensor_TaskHandle;
const osThreadAttr_t sensor_Task_attributes = {
  .name = "sensor_Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for upload_Task */
osThreadId_t upload_TaskHandle;
const osThreadAttr_t upload_Task_attributes = {
  .name = "upload_Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for alarm_Task */
osThreadId_t alarm_TaskHandle;
const osThreadAttr_t alarm_Task_attributes = {
  .name = "alarm_Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Sensor_Task(void *argument);
void Upload_Task(void *argument);
void Alarm_Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of sensor_Task */
  sensor_TaskHandle = osThreadNew(Sensor_Task, NULL, &sensor_Task_attributes);

  /* creation of upload_Task */
  upload_TaskHandle = osThreadNew(Upload_Task, NULL, &upload_Task_attributes);

  /* creation of alarm_Task */
  alarm_TaskHandle = osThreadNew(Alarm_Task, NULL, &alarm_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();

	setup_ui(&guider_ui);
  events_init(&guider_ui);
  custom_init(&guider_ui);
  /* Infinite loop */
  for(;;)
  {
		//display_time(&guider_ui);
		lv_timer_handler();
		
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Sensor_Task */
/**
* @brief Function implementing the sensor_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Sensor_Task */
void Sensor_Task(void *argument)
{
  /* USER CODE BEGIN Sensor_Task */
  char text[10];
  /* Infinite loop */
  for(;;)
  {
		//获取电压、电流、温度
    vol = Get_vol();
    current = Get_Current();
    temp = Get_Temp();

    //卡尔曼滤波
    KalmanFilter_Handle(&kf1, &kf2, &kf3, vol, current, temp);
    //显示电量
    Remaining_battery_show(filtered_voltage);

    sprintf(text, "%.2f%s", filtered_current, "A");
    lv_label_set_text(guider_ui.screen_curr_real, text);
    sprintf(text, "%.2f%s", filtered_voltage, "v");
    lv_label_set_text(guider_ui.screen_volt_real, text);
    sprintf(text, "%.2f%s", filtered_temperature, "°C");
    lv_label_set_text(guider_ui.screen_temp_real, text); 
    osDelay(1);
  }
  /* USER CODE END Sensor_Task */
}

/* USER CODE BEGIN Header_Upload_Task */
/**
* @brief Function implementing the upload_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Upload_Task */
void Upload_Task(void *argument)
{
  /* USER CODE BEGIN Upload_Task */
  uint8_t wifi_sign = 0;
  ESP8266_Init();
  
	char text[512];
  /* Infinite loop */
  for(;;)
  {
    if( 0 == wifi_sign )
    {
      lv_label_set_text(guider_ui.screen_wifi_real, "network disconnected" LV_SYMBOL_WIFI " ");
      wifi_sign = ESP8266_ConnectWifi("iQOO12", "34074700") & ESP8266_ConnectMqttAliyun(MQTT_USERNAME, MQTT_PASSWORD, MQTT_CLIENTID, MQTT_HOSTURL) & ESP8266_SubscribeTopic("/k1qq30jp47P/ESP32/user/subscribe");
    }
    else
    {
      lv_label_set_text(guider_ui.screen_wifi_real, "network connected" LV_SYMBOL_WIFI " ");
		  sprintf(text, "{\\\"params\\\":{\\\"V\\\":%.2f,\\\"I\\\":%.2f,\\\"T\\\":%.2f,\\\"SOC\\\":%.0f%%,\\\"alarm\\\":%d}}",(double)filtered_voltage, (double)filtered_current, (double)filtered_temperature, soc_estimate,level);
      ESP8266_Publish(topic, text);
    }

    osDelay(1);
  }
  /* USER CODE END Upload_Task */
}

/* USER CODE BEGIN Header_Alarm_Task */
/**
* @brief Function implementing the alarm_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Alarm_Task */
void Alarm_Task(void *argument)
{
  /* USER CODE BEGIN Alarm_Task */
  /* Infinite loop */
  for(;;)
  {
		 //报警检查
    level = Check_Battery_Alarm(level);

    if(level == ALARM_NONE){
      // 正常：白色
      lv_obj_set_style_bg_color(guider_ui.screen_cont_15, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
      lv_label_set_text(guider_ui.screen_status_real, "正常");
    }
    else if(level == ALARM_LOW){
      // 低报警：黄色
      lv_obj_set_style_bg_color(guider_ui.screen_cont_15, lv_color_hex(0xFFFF00), LV_PART_MAIN|LV_STATE_DEFAULT);
      lv_label_set_text(guider_ui.screen_status_real, "黄色报警");
    }
    else if(level == ALARM_MEDIUM){
      // 中报警：橙色
      lv_obj_set_style_bg_color(guider_ui.screen_cont_15, lv_color_hex(0xFFA500), LV_PART_MAIN|LV_STATE_DEFAULT);
      lv_label_set_text(guider_ui.screen_status_real, "橙色报警");
    }
    else if(level == ALARM_HIGH){
      // 高报警：红色
      lv_obj_set_style_bg_color(guider_ui.screen_cont_15, lv_color_hex(0xFF0000), LV_PART_MAIN|LV_STATE_DEFAULT);
      lv_label_set_text(guider_ui.screen_status_real, "红色报警");
    }
    Trigger_Alarm(level);
    osDelay(1);
  }
  /* USER CODE END Alarm_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

