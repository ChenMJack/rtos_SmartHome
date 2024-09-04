//Task
#include "T_Sensor.h"

//HAL
#include "stm32f1xx_hal.h"  // ��������HALͷ�ļ�

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
	
	osStatus_t status; // �Ƿ�ɹ�
	
    for(;;)
    {
		timeout = 3; // ÿ��ѭ������ timeout ������
		data.hum=0;
		data.temp=0;
		
		while(--timeout>0)
		{	
			//��ȡ����
			DHT11_Read(&data.hum,&data.temp);	
			//printf("%d,%d\n",data.hum,data.temp);
			
			osDelay(pdMS_TO_TICKS(100)); // ʹ�÷�������ʱ����
		}
		
		//����ȡ�����ݷ��͸����ݶ���
		status = osMessageQueuePut(Q_DataHandle, &data, 0U, osWaitForever);	
		if(status == osOK)
		{
			// �����¼�λ��֪ͨOLED�������ƽ̨�ϴ�����
			osEventFlagsSet(EventGroupHandle, TEMP_HUMIDITY_BIT);			
		}else
		{
			printf("put data error\n");
		}		
        // �ӳ�һ��ʱ��
        osDelay(pdMS_TO_TICKS(300));  // ÿ2��ɼ�һ��		            
    }
}



