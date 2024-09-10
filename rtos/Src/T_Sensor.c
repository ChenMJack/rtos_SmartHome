//Task
#include "T_Sensor.h"

//HAL
#include "stm32f1xx_hal.h"  // 包含所有HAL头文件

//rtos
#include "cmsis_os.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

//MCU Driver

#include "driver_uart.h"
#include "driver_dht11.h"
#include "driver_timer.h"

//net
#include "onenet.h"

//device
#include "esp8266.h"

//c
#include <string.h>
#include <stdlib.h>
#include "stdio.h"

extern osEventFlagsId_t EventGroupHandle;
extern osMessageQueueId_t Q_DataHandle;
extern osTimerId_t Ti_SensorHandle;

void F_Sensor(void *argument)
{
	
	osTimerStart (Ti_SensorHandle, 2000);  //定时器1以50个tick周期运行
	
    for(;;)
    {
	
		osEventFlagsWait(
			EventGroupHandle, 
			SENSOR_UPDATE_EVENT, 
			osFlagsWaitAny, 
			osWaitForever);
		
		SensorData Data;
		Data.hum=0;
		Data.temp=0;
		
		//printf("123");
		int times = 3;
		for(;times>0;times--)
		{	
			//读取数据
			DHT11_Read(&Data.hum,&Data.temp);	

					
		}
		osMessageQueuePut(Q_DataHandle, &Data, 0U, osWaitForever);
		osMessageQueuePut(Q_DataHandle, &Data, 0U, osWaitForever);

		// 设置普通温湿度更新标志
		osEventFlagsSet(EventGroupHandle, TEMP_HUMIDITY_BIT);				          
    }
}


void Cb_Sensor(void *argument)
{
	osEventFlagsSet(EventGroupHandle, SENSOR_UPDATE_EVENT);

}


