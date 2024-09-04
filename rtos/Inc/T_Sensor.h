#ifndef T_Sensor_H
#define T_Sensor_H

void F_Sensor(void *argument);

typedef struct {
    int hum;
    int temp;
} SensorData;

#define TEMP_HUMIDITY_BIT   (1U << 1)

#endif
