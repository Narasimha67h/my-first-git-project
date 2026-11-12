
#include <Arduino.h>
#include "config.h"
#include "water_level.h"
#include "notification.h"

static const uint8_t thresholds[3] = {
    25, 50, 100
};

static bool alertArmed[3] = { true, true, true };
static uint8_t aboveCount[3] = { 0, 0, 0 };

static bool emptyAlertArmed = true;
static uint8_t emptyCount = 0;

WaterLevelData calculateWaterLevel(uint16_t distanceMM)
{
    WaterLevelData result;
    float percentage = 0.0f;

    // FULL: 8 cm
    if (distanceMM <= FULL_DISTANCE_MM)
    {
        percentage = 100.0f;
    }
    // FULL to HALF: 8 cm to 15 cm
    else if (distanceMM <= HALF_DISTANCE_MM)
    {
        percentage = 100.0f -
            ((distanceMM - FULL_DISTANCE_MM) * 50.0f /
             (HALF_DISTANCE_MM - FULL_DISTANCE_MM));
    }
    // HALF to 25%: 15 cm to 21 cm
    else if (distanceMM <= QUARTER_DISTANCE_MM)
    {
        percentage = 50.0f -
            ((distanceMM - HALF_DISTANCE_MM) * 25.0f /
             (QUARTER_DISTANCE_MM - HALF_DISTANCE_MM));
    }
    // 25% to EMPTY: 21 cm to 28 cm
    else if (distanceMM <= EMPTY_DISTANCE_MM)
    {
        percentage = 25.0f -
            ((distanceMM - QUARTER_DISTANCE_MM) * 25.0f /
             (EMPTY_DISTANCE_MM - QUARTER_DISTANCE_MM));
    }
    else
    {
        percentage = 0.0f;
    }

    if (percentage < 0.0f) percentage = 0.0f;
    if (percentage > 100.0f) percentage = 100.0f;

    result.percentage = percentage;

    result.waterHeightMM =
        static_cast<uint16_t>(
            (percentage * BUCKET_HEIGHT_MM) / 100.0f
        );

    return result;
}

void updateLevelAlerts(float percentage)
{
    // 25%, 50%, 100% alerts while filling
    for (uint8_t i = 0; i < 3; i++)
    {
        const uint8_t threshold = thresholds[i];

        if (percentage <=
            (threshold - LEVEL_RESET_HYSTERESIS_PERCENT))
        {
            alertArmed[i] = true;
            aboveCount[i] = 0;
        }

        if (alertArmed[i] && percentage >= threshold)
        {
            if (aboveCount[i] < 3)
                aboveCount[i]++;

            if (aboveCount[i] >= 3)
            {
                alertArmed[i] = false;
                sendLevelNotification(threshold, percentage);
            }
        }
        else if (percentage < threshold)
        {
            aboveCount[i] = 0;
        }
    }

    // LOW / EMPTY alert at 28 cm or more
    if (percentage <= 0.5f)
    {
        if (emptyAlertArmed)
        {
            if (emptyCount < 3)
                emptyCount++;

            if (emptyCount >= 3)
            {
                emptyAlertArmed = false;
                sendNotification("WARNING: Water level LOW / EMPTY!");
            }
        }
    }
    else
    {
        emptyCount = 0;

        if (percentage >= 5.0f)
            emptyAlertArmed = true;
    }
}
