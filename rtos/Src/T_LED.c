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

LED_INFO led_info = {0};

void F_LED(void *argument)
{
    for(;;)
    {
       //printf("123");
        
    }
}
