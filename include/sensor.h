#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

void sensorInit();
void sensorUpdate();
bool sensorHasNewReading();
uint16_t sensorGetDistanceMM();
unsigned long sensorLastValidReadingMs();

#endif
