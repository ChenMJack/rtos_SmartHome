//Task
#include "T_Wifi.h"

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

//net
#include "onenet.h"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"


void F_Wifi(void *argument)
{


		ConnectNet();
	    	
		osThreadExit();
		
}

void Cb_WifiCheck(void *argument)
{

}	

void ConnectNet(void)
{
	//Connect WIFI
	ESP8266_Init();

	//Connect MQTT
	printf( "Connect MQTTs Server...\r\n");
	OLED_Clear();
	OLED_PrintString(0,0,"Connect MQTTs Server...");
	
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		mdelay(500);
	printf( "Connect MQTTs Server Success\r\n");	
	OLED_PrintString(0,0,"Connect MQTTs Server Success");	
	
	//Connect OneNET
	OLED_Clear();
	OLED_PrintString(0,0,"Device login...");
	
	while(OneNet_DevLink())			
		mdelay(500);
		
	//printf("1112");
	//Subscribe Topic
	OneNET_Subscribe();	
	Display_Init();
}	

void Display_Init(void)
{

 	OLED_Clear(); 
	
	OLED_ShowChinese(0,0,1);//温
	OLED_ShowChinese(18,0,2);//度
	OLED_ShowChinese(36,0,0);//：
	OLED_ShowChinese(90,0,3);//℃
	
	OLED_ShowChinese(0,3,4);//湿
	OLED_ShowChinese(18,3,5);//度
	OLED_ShowChinese(36,3,0);//：
	OLED_PrintString(12,3,"%");//%
	
	OLED_ShowChinese(0,6,6);//台
	OLED_ShowChinese(18,6,7);//灯
	OLED_ShowChinese(36,6,0);//：
	
}
