/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	esp8266.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		ESP8266�ļ�����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//MCU
#include "stm32f1xx_hal.h"  // ��������HALͷ�ļ�


//�����豸����
#include "esp8266.h"

//Ӳ������
#include "driver_timer.h"
#include "driver_uart.h"
#include "driver_oled.h"


//C��
#include <string.h>
#include <stdio.h>



#define ESP8266_WIFI_INFO		"AT+CWJAP=\"mj\",\"123456789\"\r\n"


uint8_t esp8266_buf[512];
uint8_t received_char;
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;

extern UART_HandleTypeDef huart2;


//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{


	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

//	Usart_SendString(&huart2, (unsigned char *)cmd, strlen((const char *)cmd));
	Usart_SendString(&huart2, (unsigned char *)cmd, strlen(cmd));
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				
				ESP8266_Clear();									//��ջ���
				
				return 0;
			}
		}
		//printf("Waiting for response...\n");
		mdelay(10);
	}
	//printf("Command timeout.\n");
	return 1;

}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		Usart_SendString(&huart2, data, len);		//�����豸������������
	}

}

//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//printf( "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		mdelay(5);													//��ʱ�ȴ�
	} while(timeOut--);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Init(void)
{
	OLED_Clear();
	ESP8266_Clear();
	

	printf("1. AT\r\n");
	OLED_PrintString(0, 0, "1.AT...");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		 mdelay(500);  // ǿ���ó�ʱ��Ƭ

	printf("2. CWMODE\r\n");
	OLED_PrintString(0, 2, "2.CWMODE...");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		 mdelay(500);  // ǿ���ó�ʱ��Ƭ

	printf("3. AT+CWDHCP\r\n");
	OLED_PrintString(0, 4, "3.AT+CWDHCP...");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		 mdelay(500);  // ǿ���ó�ʱ��Ƭ

	printf("4. CWJAP\r\n");
	OLED_PrintString(0, 6, "4.CWJAP..."); // ����λ��
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		 mdelay(500);  // ǿ���ó�ʱ��Ƭ

	printf("5. ESP8266 Init OK\r\n");
	OLED_Clear();
	OLED_PrintString(0, 0, "ESP8266 Init OK");
	 mdelay(500);  // ǿ���ó�ʱ��Ƭ
	
//	OLED_Clear();
//	ESP8266_Clear();
//	
//    printf("1. AT\r\n");
//    OLED_PrintString(0,0,"1.AT...");  //λ�� ��Ϣ ����
//    if (ESP8266_SendCmd("AT\r\n", "OK") == 0) {
//        OLED_PrintString(0,8,"AT OK");
//    } else {
//        OLED_PrintString(0,8,"AT FAIL");
//    }
//    mdelay(300);

//    
//    printf("2. CWMODE\r\n");
//    OLED_PrintString(0,2,"2.CWMODE...");  
//    if (ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK") == 0) {
//		
//        OLED_PrintString(0,10,"CWMODE OK");
//    } else {
//        OLED_PrintString(0,10,"CWMODE FAIL");
//    }

//    mdelay(300);
//    
//    printf("3. AT+CWDHCP\r\n");
//    OLED_PrintString(0,4,"3.AT+CWDHCP...");  
//    if (ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK") == 0) {
//        OLED_PrintString(0,12,"CWDHCP OK");
//    } else {
//        OLED_PrintString(0,12,"CWDHCP FAIL");
//    }

//    mdelay(300);

//    printf("4. CWJAP\r\n");
//    OLED_PrintString(0,6,"4.CWJAP...");  
//    if (ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP") == 0) {
//        OLED_PrintString(0,14,"CWJAP OK");
//    } else {
//        OLED_PrintString(0,14,"CWJAP FAIL");
//    }

//    mdelay(300);

//    printf("5. ESP8266 Init OK\r\n");
//    OLED_Clear();
//    OLED_PrintString(0,0,"ESP8266 Init OK");  
//    mdelay(300);
//	

}

//==========================================================
//	�������ƣ�	USART2_IRQHandler
//
//	�������ܣ�	����2�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        // �����յ�������ֱ�Ӵ洢����������
        if (esp8266_cnt >= sizeof(esp8266_buf))
            esp8266_cnt = 0; // ��ֹ���������

        esp8266_buf[esp8266_cnt++] = received_char;  // ʹ�����½��յ����ַ�

        // ����������һ���ַ��Ľ���
        HAL_UART_Receive_IT(&huart2, &received_char, 1);
    }
}
