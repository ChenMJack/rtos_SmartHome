#ifndef T_Sensor_H
#define T_Sensor_H

void F_Sensor(void *argument);

typedef struct {
    int hum;
    int temp;
} SensorData;

#define TEMP_HUMIDITY_BIT   	(1U << 1) 	  // 温湿度更新事件位
#define SENSOR_UPDATE_EVENT      (1U << 3)    //  回调事件

void Cb_Sensor(void *argument);

#

#endif
