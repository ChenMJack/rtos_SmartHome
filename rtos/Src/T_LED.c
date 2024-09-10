//Task
#include "T_LED.h"

//c
#include <string.h>
#include "stdio.h"

//HAL
#include "stm32f1xx_hal.h"  // 包含所有HAL头文件

//rtos
#include "cmsis_os.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"   

//MCU Driver
#include "driver_uart.h"
#include "driver_led.h"

extern osEventFlagsId_t EventGroupHandle;
extern osMessageQueueId_t Q_DataHandle;

LED_INFO led_info = {0};

void F_LED(void *argument)
{


    for(;;)
    {
		// 等待外部中断设置的事件标志
		EventBits_t uxBits = osEventFlagsWait(
			EventGroupHandle, 
			LED_UPDATE_BIT, 
			osFlagsWaitAny, 
			osWaitForever);
		
		if (led_info.Led_Status == LED_ON) {
			Led_Control(LED_GREEN, 0);
			led_info.Led_Status = LED_OFF;
		} else {
			Led_Control(LED_GREEN, 1);
			led_info.Led_Status = LED_ON;
		}
        

        // 可以添加延迟以避免快速连续触发
        osDelay(pdMS_TO_TICKS(10)); // 延迟10ms        
    }
}

