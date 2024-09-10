//Task
#include "T_CloudDownload.h"
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

extern osTimerId_t Ti_WifiDownHandle;
extern osEventFlagsId_t EventGroupHandle;
extern osMutexId_t M_dataHandle;

unsigned char *dataPtr = NULL;

int i = 0;

//net
#include "onenet.h"
void F_CloudDownload(void *argument)
{

	osTimerStart (Ti_WifiDownHandle, 9);  //定时器1以50个tick周期运行
	for(;;)
	{
		
		
		osEventFlagsWait(
			EventGroupHandle, 
			CLOUD_DOWNLOAD_EVENT, 
			osFlagsWaitAny, 
			osWaitForever);		
		
		osMutexWait(M_dataHandle, osWaitForever); // 等待获取互斥量

		OneNet_RevPro(dataPtr);  // 解析和处理数据
		//dataPtr = NULL; // 处理完毕后清空指针

		osMutexRelease(M_dataHandle); // 释放互斥量
		//printf("%d",++i);
		osDelay(pdMS_TO_TICKS(1));  // 增加适当的延迟，避免频繁运行
	}

}

void Cb_WifiDown(void *argument)
{
	//printf("%d",++i);
	osMutexWait(M_dataHandle, osWaitForever); // 等待获取互斥量
	dataPtr = ESP8266_GetIPD(0);

    if (dataPtr != NULL)
    {
        // 如果有下载数据，通知 CloudDownload 任务进行处理
        osEventFlagsSet(EventGroupHandle, CLOUD_DOWNLOAD_EVENT);
    }
	osMutexRelease(M_dataHandle); // 释放互斥量
    
}
