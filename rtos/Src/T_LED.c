//Task
#include "T_LED.h"

//c
#include <string.h>
#include "stdio.h"

//HAL
#include "stm32f1xx_hal.h"  // ��������HALͷ�ļ�

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
		// �ȴ��ⲿ�ж����õ��¼���־
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
        

        // ��������ӳ��Ա��������������
        osDelay(pdMS_TO_TICKS(10)); // �ӳ�10ms        
    }
}

