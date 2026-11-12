#include <Arduino.h>
#include <SoftwareSerial.h>

#include "config.h"
#include "sensor.h"

// RX-only software UART: sensor TX connects to NodeMCU D5.
static SoftwareSerial sensorSerial(SENSOR_UART_RX_PIN, -1);

static uint8_t frame[4];
static uint8_t frameIndex = 0;
static uint16_t distanceMM = 0;
static bool newReading = false;
static unsigned long lastTriggerMs = 0;
static unsigned long lastValidMs = 0;

static void triggerMeasurement()
{
    // DYP controlled UART mode: falling edge on sensor RX triggers one reading.
    digitalWrite(SENSOR_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(SENSOR_TRIGGER_PIN, LOW);
    delayMicroseconds(20);
    digitalWrite(SENSOR_TRIGGER_PIN, HIGH);
}

static void parseIncomingBytes()
{
    while (sensorSerial.available() > 0)
    {
        const uint8_t value = static_cast<uint8_t>(sensorSerial.read());

        if (frameIndex == 0)
        {
            if (value == 0xFF)
            {
                frame[frameIndex++] = value;
            }
            continue;
        }

        frame[frameIndex++] = value;

        if (frameIndex == 4)
        {
            const uint8_t checksum =
                static_cast<uint8_t>((frame[0] + frame[1] + frame[2]) & 0xFF);

            if (checksum == frame[3])
            {
                const uint16_t measured =
                    (static_cast<uint16_t>(frame[1]) << 8) | frame[2];

                if (measured >= SENSOR_MIN_DISTANCE_MM &&
                    measured <= SENSOR_MAX_DISTANCE_MM)
                {
                    distanceMM = measured;
                    newReading = true;
                    lastValidMs = millis();
                }
            }

            frameIndex = 0;
        }
    }
}

void sensorInit()
{
    pinMode(SENSOR_TRIGGER_PIN, OUTPUT);
    digitalWrite(SENSOR_TRIGGER_PIN, HIGH);

    sensorSerial.begin(SENSOR_BAUD_RATE);
    sensorSerial.listen();

    lastTriggerMs = millis() - SENSOR_TRIGGER_INTERVAL_MS;
    Serial.println(F("DYP-A02YYTW UART sensor initialized."));
}

void sensorUpdate()
{
    const unsigned long now = millis();

    if (now - lastTriggerMs >= SENSOR_TRIGGER_INTERVAL_MS)
    {
        lastTriggerMs = now;
        triggerMeasurement();
    }

    parseIncomingBytes();
}

bool sensorHasNewReading()
{
    if (newReading)
    {
        newReading = false;
        return true;
    }
    return false;
}

uint16_t sensorGetDistanceMM()
{
    return distanceMM;
}

unsigned long sensorLastValidReadingMs()
{
    return lastValidMs;
}
