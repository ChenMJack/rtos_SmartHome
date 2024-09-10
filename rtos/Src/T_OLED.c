//Task
#include "T_OLED.h"
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


void F_OLED(void *argument)
{
	//osStatus_t status;
	SensorData data;
	
    for(;;)
    {
		 
		//等待事件
        EventBits_t uxBits = osEventFlagsWait(
			EventGroupHandle,
			TEMP_HUMIDITY_BIT | LED_UPDATE_BIT, 
			osFlagsWaitAny, 
			osWaitForever);
		
		if (uxBits & TEMP_HUMIDITY_BIT)
		{	
			osMessageQueueGet(Q_DataHandle, &data, NULL, osWaitForever);
			//printf("%d,%d",data.temp, data.hum);
		}

         Refresh_Data(data.temp, data.hum); // 刷新 OLED 数据
        
		
		osDelay(pdMS_TO_TICKS(10));  // 正常更新，每2000ms 更新一次			
		    
    }
}

void Refresh_Data(int temp,int hum)
{
	//缓冲区
	char buf[4];
	
	sprintf(buf, "%2d", temp);
	OLED_PrintString(7,0,buf); //温度值
	
	sprintf(buf, "%2d", hum);
	OLED_PrintString(7,3,buf); //湿度值
	
	if(led_info.Led_Status) OLED_ShowChinese(54,6,8);//亮
	else OLED_ShowChinese(54,6,9);//灭
	
}
