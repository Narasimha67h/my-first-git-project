#include <Arduino.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "ota.h"

void otaInit()
{
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([]()
    {
        Serial.println(F("OTA update started."));
    });

    ArduinoOTA.onEnd([]()
    {
        Serial.println(F("\nOTA update finished. Restarting..."));
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
    {
        const unsigned int percent = (progress * 100U) / total;
        Serial.printf("OTA progress: %u%%\r", percent);
    });

    ArduinoOTA.onError([](ota_error_t error)
    {
        Serial.printf("OTA error [%u]: ", error);

        if (error == OTA_AUTH_ERROR) Serial.println(F("Authentication failed"));
        else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin failed"));
        else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect failed"));
        else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive failed"));
        else if (error == OTA_END_ERROR) Serial.println(F("End failed"));
        else Serial.println(F("Unknown error"));
    });

    ArduinoOTA.begin();
    Serial.println(F("OTA ready."));
    Serial.print(F("Hostname: "));
    Serial.println(OTA_HOSTNAME);
}

void otaHandle()
{
    ArduinoOTA.handle();
}
