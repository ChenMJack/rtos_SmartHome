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
#include "stdio.h"

extern osEventFlagsId_t EventGroupHandle;
extern osMessageQueueId_t Q_DataHandle;




void F_Sensor(void *argument)
{

	SensorData data;
	uint8_t timeout;
	
	osStatus_t status; // 是否成功
	
    for(;;)
    {
		timeout = 3; // 每次循环重置 timeout 计数器
		data.hum=0;
		data.temp=0;
		
		while(--timeout>0)
		{	
			//读取数据
			DHT11_Read(&data.hum,&data.temp);	
			//printf("%d,%d\n",data.hum,data.temp);
			
			osDelay(pdMS_TO_TICKS(100)); // 使用非阻塞延时代替
		}
		
		//将读取的数据发送给数据队列
		status = osMessageQueuePut(Q_DataHandle, &data, 0U, osWaitForever);	
		if(status == osOK)
		{
			// 设置事件位，通知OLED任务和云平台上传任务
			osEventFlagsSet(EventGroupHandle, TEMP_HUMIDITY_BIT);			
		}else
		{
			printf("put data error\n");
		}		
        // 延迟一段时间
        osDelay(pdMS_TO_TICKS(300));  // 每2秒采集一次		            
    }
}



