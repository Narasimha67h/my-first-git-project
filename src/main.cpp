#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "config.h"
#include "sensor.h"
#include "water_level.h"
#include "ota.h"

static unsigned long lastStatusPrintMs = 0;
static unsigned long lastWifiAttemptMs = 0;
static bool otaReady = false;

static void connectWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print(F("Connecting to Wi-Fi"));

    const unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - start < 20000UL)
    {
        delay(250);
        Serial.print('.');
        yield();
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print(F("Wi-Fi connected. IP: "));
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println(F("Wi-Fi not connected. Will retry in loop."));
    }
}

void setup()
{
    Serial.begin(115200);
    delay(300);

    Serial.println();
    Serial.println(F("================================"));
    Serial.println(F(" SMART WATER LEVEL MONITOR"));
    Serial.println(F(" ESP8266 + DYP-A02YYTW-V2.0"));
    Serial.println(F("================================"));

    sensorInit();
    connectWiFi();

    if (WiFi.status() == WL_CONNECTED)
    {
        otaInit();
        otaReady = true;
    }
}

void loop()
{
    sensorUpdate();

    if (WiFi.status() == WL_CONNECTED)
    {
        if (!otaReady)
        {
            otaInit();
            otaReady = true;
        }
        otaHandle();
    }
    else
    {
        otaReady = false;

        if (millis() - lastWifiAttemptMs >= 15000UL)
        {
            lastWifiAttemptMs = millis();

            Serial.println(F("Retrying Wi-Fi..."));

            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        }
    }

    if (sensorHasNewReading())
    {
        const uint16_t distance = sensorGetDistanceMM();
        const WaterLevelData level = calculateWaterLevel(distance);

        Serial.print(F("Distance: "));
        Serial.print(distance);
        Serial.print(F(" mm ("));
        Serial.print(distance / 10.0f, 1);
        Serial.print(F(" cm) | Water: "));
        Serial.print(level.waterHeightMM / 10.0f, 1);
        Serial.print(F(" cm | Level: "));
        Serial.print(level.percentage, 1);
        Serial.println(F("%"));

        updateLevelAlerts(level.percentage);
    }

    if (millis() - lastStatusPrintMs >= 5000UL)
    {
        lastStatusPrintMs = millis();

        if (sensorLastValidReadingMs() == 0 ||
            millis() - sensorLastValidReadingMs() > 10000UL)
        {
            Serial.println(F("WARNING: No valid sensor frame received recently."));
        }
    }

    yield();
}
