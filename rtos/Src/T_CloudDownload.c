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
#include "stm32f1xx_hal.h"  // ��������HALͷ�ļ�

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

	osTimerStart (Ti_WifiDownHandle, 9);  //��ʱ��1��50��tick��������
	for(;;)
	{
		
		
		osEventFlagsWait(
			EventGroupHandle, 
			CLOUD_DOWNLOAD_EVENT, 
			osFlagsWaitAny, 
			osWaitForever);		
		
		osMutexWait(M_dataHandle, osWaitForever); // �ȴ���ȡ������

		OneNet_RevPro(dataPtr);  // �����ʹ�������
		//dataPtr = NULL; // ������Ϻ����ָ��

		osMutexRelease(M_dataHandle); // �ͷŻ�����
		//printf("%d",++i);
		osDelay(pdMS_TO_TICKS(1));  // �����ʵ����ӳ٣�����Ƶ������
	}

}

void Cb_WifiDown(void *argument)
{
	//printf("%d",++i);
	osMutexWait(M_dataHandle, osWaitForever); // �ȴ���ȡ������
	dataPtr = ESP8266_GetIPD(0);

    if (dataPtr != NULL)
    {
        // ������������ݣ�֪ͨ CloudDownload ������д���
        osEventFlagsSet(EventGroupHandle, CLOUD_DOWNLOAD_EVENT);
    }
	osMutexRelease(M_dataHandle); // �ͷŻ�����
    
}
