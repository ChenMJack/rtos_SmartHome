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
	
	//osStatus_t status; // 是否成功	
	
	
    for(;;)
    {
		
        // 等待LED或温湿度更新事件
		EventBits_t uxBits = osEventFlagsWait(
			EventGroupHandle, 
			 TEMP_HUMIDITY_BIT | LED_UPDATE_BIT, 
			osFlagsWaitAny, 
			osWaitForever);		
		
		SensorData data;

		temp = data.temp;
		hum = data.hum;				
		
		if (uxBits & TEMP_HUMIDITY_BIT)
		{	
			osMessageQueueGet(Q_DataHandle, &data, NULL, osWaitForever);
			//printf("%d,%d",data.temp, data.hum);
		}		
		
		//printf("%d,%d",data.temp, data.hum);
		
		OneNet_SendData();	
		ESP8266_Clear();
		osDelay(pdMS_TO_TICKS(1500));	// 正常情况下的上传间隔	
	
    }
}
