#ifndef WATER_LEVEL_H
#define WATER_LEVEL_H

#include <Arduino.h>

struct WaterLevelData
{
    float percentage;
    uint16_t waterHeightMM;
};

WaterLevelData calculateWaterLevel(uint16_t distanceMM);
void updateLevelAlerts(float percentage);

#endif
