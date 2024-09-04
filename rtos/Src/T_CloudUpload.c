//Task
#include "T_CloudUpload.h"
#include "T_Sensor.h"
#include "T_LED.h"


//c
#include <string.h>
#include "stdio.h"

//rtos
#include "cmsis_os.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

//HAL
#include "stm32f1xx_hal.h"  // 包含所有HAL头文件

//device
#include "esp8266.h"

//Driver
#include "driver_uart.h"
#include "driver_oled.h"
#include "driver_timer.h"
#include "driver_led.h"

//net
#include "onenet.h"

extern osEventFlagsId_t EventGroupHandle;
extern osMessageQueueId_t Q_DataHandle;

int temp,hum;
void F_CloudUpload(void *argument)
{
	
    for(;;)
    {
        // 等待LED或温湿度更新事件
		EventBits_t uxBits = osEventFlagsWait(
			EventGroupHandle, 
			LED_UPDATE_BIT | TEMP_HUMIDITY_BIT, 
			osFlagsWaitAny, 
			osWaitForever);		
        
		//提取数据
        if (uxBits & TEMP_HUMIDITY_BIT) {
            SensorData data;
            osMessageQueueGet(Q_DataHandle, &data, NULL, osWaitForever);
			temp = data.temp;
			hum = data.hum;
			
        }	
		
		//发送数据到云平台
		OneNet_SendData();	
		ESP8266_Clear();

		
    }
}
