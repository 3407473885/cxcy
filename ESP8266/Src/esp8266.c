#include "esp8266.h"

/**
 * @brief esp8266初始化
 * 
 * @return uint8_t 0：初始化失败，1：初始化成功
 */
uint8_t ESP8266_Init(void)
{
    osDelay(500);
    if( !ESP8266_SendCmd("AT", "OK") ) return 0;
    if( !ESP8266_SendCmd("ATE0", "OK") ) return 0;
    // printf("esp8266初始化成功\n");
    
    osDelay(100);  
    ESP8266_ConnectWifi("iQOO12", "34074700");
    ESP8266_ConnectMqttAliyun(MQTT_USERNAME, MQTT_PASSWORD, MQTT_CLIENTID, MQTT_HOSTURL);
    ESP8266_SubscribeTopic("/k1qq30jp47P/ESP32/user/subscribe"); 
    return 1;
}

uint8_t ESP8266_SendCmd(const char *cmd, const char *expectation)
{
    char rx_buffer[512];
    uint32_t len = 0;
    uint8_t byte;

    printf("CMD:%s\n",cmd);
    osDelay(100);   
    HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 1000);
    HAL_UART_Transmit(&huart1, (uint8_t *)"\r\n", 2, 1000);

    uint32_t start = HAL_GetTick();

    while (HAL_GetTick() - start < 5000) {
        if (HAL_UART_Receive(&huart1, &byte, 1, 100) == HAL_OK) rx_buffer[len++] = byte;
    }

    rx_buffer[len] = '\0';
    printf("%s\n",rx_buffer);
    osDelay(100);   
    if(expectation && (strstr(rx_buffer, expectation) || strstr(rx_buffer, "\r\nOK\r\n")))
    {
        printf("%s + 成功\n",cmd);
        osDelay(100);   
        return 1;
    }
    printf("%s + 失败\n",cmd);
    osDelay(100);   
    return 0;
}


/**
 * @brief 连接Wifi
 * 
 * @param ssid 网络名称
 * @param pwd 网络密码
 * @return uint8_t 0：连接失败，1：连接成功
 */
uint8_t ESP8266_ConnectWifi(const char *ssid, const char *pwd)
{
    char cmd[128];
    if( !ESP8266_SendCmd("AT+CWMODE=1", "OK") ) return 0;
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
    if( !ESP8266_SendCmd(cmd , "WIFI CONNECTED") ) return 0;
    // printf("连接网络成功\n");
    osDelay(100);   
    return 1;
}


/**
 * @brief 连接阿里云
 * 
 * @param username username
 * @param password passwd
 * @param clientId clientId
 * @param mqttHostUrl mqttHostUrl
 * @return uint8_t 0：连接失败，1：连接成功
 */
uint8_t ESP8266_ConnectMqttAliyun(const char *username, const char *password, const char *clientId, const char *mqttHostUrl)
{
    char cmd[256];
    sprintf(cmd, "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"", username, password);
    if( !ESP8266_SendCmd((const char *)cmd, "OK") ) return 0;
    sprintf(cmd, "AT+MQTTCLIENTID=0,\"%s\"", clientId);
    if( !ESP8266_SendCmd((const char *)cmd, "OK") ) return 0;
    sprintf(cmd, "AT+MQTTCONN=0,\"%s\",1883,1", mqttHostUrl);
    if( !ESP8266_SendCmd((const char *)cmd, "MQTTCONNECTED") ) return 0;

    // printf("连接阿里云成功\n");
    osDelay(100);   
    return 1;
}


/**
 * @brief 发布内容到云端
 * 
 * @param topic 订阅的主题
 * @param content 内容
 * @return uint8_t 0：发送失败，1：发送成功
 */
uint8_t ESP8266_Publish(const char* topic, const char* content)
{
    char cmd[512];
    sprintf(cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",1,0",topic, content);
    if( !ESP8266_SendCmd(cmd, "OK") ){
        // printf("发布失败");
        osDelay(100);   
        return 0;
    } 
    // printf("发布成功");
    osDelay(100);   
    return 1;
}


/**
 * @brief 订阅主题
 * 
 * @param topic 主题
 * @return uint8_t 0：订阅失败，1：订阅成功
 */
uint8_t ESP8266_SubscribeTopic(const char *topic)
{
    char cmd[512];
    sprintf(cmd, "AT+MQTTSUB=0,\"%s\",1", topic);
    if( !ESP8266_SendCmd(cmd, "ok") ) {
        // printf("订阅失败");
        osDelay(100);   
        return 0;
    }
    // printf("订阅成功\n");
    osDelay(100);   
    return 1;
}


