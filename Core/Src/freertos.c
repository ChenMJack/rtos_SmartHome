/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

//rtos
#include "queue.h"
#include "timers.h"
#include "event_groups.h"

//task_file
#include "T_Wifi.h"
#include "T_Sensor.h"
#include "T_LED.h"
#include "T_OLED.h"
#include "T_CloudUpload.h"


//MCU Driver
#include "driver_oled.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticTimer_t osStaticTimerDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
typedef StaticEventGroup_t osStaticEventGroupDef_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_QUEUE_SIZE 5
#define SENSOR_DATA_SIZE sizeof(SensorData)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */


//	Wifi
osThreadId_t T_WifiHandle;
uint32_t T_WifiBuffer[ 512 ];
osStaticThreadDef_t T_WifiControlBlock;
const osThreadAttr_t T_Wifi_attributes = {
  .name = "T_Wifi",
  .cb_mem = &T_WifiControlBlock,
  .cb_size = sizeof(T_WifiControlBlock),
  .stack_mem = &T_WifiBuffer[0],
  .stack_size = sizeof(T_WifiBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal, // > normal
};

//	CloudUpload
osThreadId_t T_CloudUploadHandle;
uint32_t T_CloudUploadBuffer[ 512 ];
osStaticThreadDef_t T_CloudUploadControlBlock;
const osThreadAttr_t T_CloudUpload_attributes = {
  .name = "T_CloudUpload",
  .cb_mem = &T_CloudUploadControlBlock,
  .cb_size = sizeof(T_CloudUploadControlBlock),
  .stack_mem = &T_CloudUploadBuffer[0],
  .stack_size = sizeof(T_CloudUploadBuffer),
  .priority = (osPriority_t) osPriorityNormal1, // > no
};


//	Sensor
osThreadId_t T_SensorHandle;
uint32_t T_SensorBuffer[ 256 ];
osStaticThreadDef_t T_SensorControlBlock;
const osThreadAttr_t T_Sensor_attributes = {
  .name = "T_Sensor",
  .cb_mem = &T_SensorControlBlock,
  .cb_size = sizeof(T_SensorControlBlock),
  .stack_mem = &T_SensorBuffer[0],
  .stack_size = sizeof(T_SensorBuffer),
  .priority = (osPriority_t) osPriorityNormal1,  // > no
};

//	OLED
osThreadId_t T_OLEDHandle;
uint32_t T_OLEDBuffer[ 256 ];
osStaticThreadDef_t T_OLEDControlBlock;
const osThreadAttr_t T_OLED_attributes = {
  .name = "T_OLED",
  .cb_mem = &T_OLEDControlBlock,
  .cb_size = sizeof(T_OLEDControlBlock),
  .stack_mem = &T_OLEDBuffer[0],
  .stack_size = sizeof(T_OLEDBuffer),
  .priority = (osPriority_t) osPriorityNormal1,  // > no
};


// LED
osThreadId_t T_LEDHandle;
uint32_t T_LEDBuffer[ 128 ];
osStaticThreadDef_t T_LEDControlBlock;
const osThreadAttr_t T_LED_attributes = {
  .name = "T_LED",
  .cb_mem = &T_LEDControlBlock,
  .cb_size = sizeof(T_LEDControlBlock),
  .stack_mem = &T_LEDBuffer[0],
  .stack_size = sizeof(T_LEDBuffer),
  .priority = (osPriority_t) osPriorityNormal2,    // > no1
};

// Q_Data
osMessageQueueId_t Q_DataHandle;
uint8_t Q_DataBuffer[ MAX_QUEUE_SIZE * SENSOR_DATA_SIZE ];
osStaticMessageQDef_t Q_DataBlock;
const osMessageQueueAttr_t Q_Data_attributes = {
  .name = "Q_Data",
  .cb_mem = &Q_DataBlock,
  .cb_size = sizeof(Q_DataBlock),
  .mq_mem = &Q_DataBuffer,
  .mq_size = sizeof(Q_DataBuffer)
};

// Q_Cloud
//osMessageQueueId_t Q_CloudHandle;
//uint8_t Q_CloudBuffer[ 5 * sizeof( uint16_t ) ];
//osStaticMessageQDef_t Q_CloudBlock;
//const osMessageQueueAttr_t Q_Cloud_attributes = {
//  .name = "Q_Cloud",
//  .cb_mem = &Q_CloudBlock,
//  .cb_size = sizeof(Q_CloudBlock),
//  .mq_mem = &Q_CloudBuffer,
//  .mq_size = sizeof(Q_CloudBuffer)
//};


// M_WIFI
//osMutexId_t M_WifiHandle;
//osStaticMutexDef_t M_WifiControlBlock;
//const osMutexAttr_t M_Wifi_attributes = {
//  .name = "M_Wifi",
//  .cb_mem = &M_WifiControlBlock,
//  .cb_size = sizeof(M_WifiControlBlock),
//};


// Ti_WifiCheck
osTimerId_t Ti_WifiCheckHandle;
osStaticTimerDef_t Ti_WifiCheckBlock;
const osTimerAttr_t Ti_WifiCheck_attributes = {
  .name = "Ti_WifiCheck",
  .cb_mem = &Ti_WifiCheckBlock,
  .cb_size = sizeof(Ti_WifiCheckBlock),
};

// Binary Source
//osSemaphoreId_t BS_ResourceHandle;
//osStaticSemaphoreDef_t BS_ResourceControlBlock;
//const osSemaphoreAttr_t BS_Resource_attributes = {
//  .name = "BS_Resource",
//  .cb_mem = &BS_ResourceControlBlock,
//  .cb_size = sizeof(BS_ResourceControlBlock),
//};

// Event Group

osEventFlagsId_t EventGroupHandle;
osStaticEventGroupDef_t EventGroupControlBlock;
const osEventFlagsAttr_t EventGroup_attributes = {
  .name = "myEvent01",
  .cb_mem = &EventGroupControlBlock,
  .cb_size = sizeof(EventGroupControlBlock),
};



/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  
  // Create Task
  
	T_WifiHandle = osThreadNew(F_Wifi, NULL, &T_Wifi_attributes);
	T_SensorHandle = osThreadNew(F_Sensor, NULL, &T_Sensor_attributes);
	T_OLEDHandle = osThreadNew(F_OLED, NULL, &T_OLED_attributes);
	//T_LEDHandle = osThreadNew(F_LED, NULL, &T_LED_attributes);
	T_CloudUploadHandle = osThreadNew(F_CloudUpload, NULL, &T_CloudUpload_attributes);

  // Create Queue
	Q_DataHandle = osMessageQueueNew (MAX_QUEUE_SIZE, SENSOR_DATA_SIZE, &Q_Data_attributes);
	//Q_CloudHandle = osMessageQueueNew (5, sizeof(uint16_t), &Q_Cloud_attributes);
  
  // Create Mutex
	//M_WifiHandle = osMutexNew(&M_Wifi_attributes);
	
  // Create Timer
	//Ti_WifiCheckHandle = osTimerNew(Cb_WifiCheck, osTimerPeriodic, NULL, &Ti_WifiCheck_attributes);

   // Create EventGroup
    EventGroupHandle = osEventFlagsNew(&EventGroup_attributes);
  
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	
  /* Infinite loop */
  for(;;)
  {
    osDelay(pdMS_TO_TICKS(1));
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

