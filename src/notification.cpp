
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#include "config.h"
#include "notification.h"

static String urlEncode(const String &input)
{
    String encoded;
    char hex[4];

    for (size_t i = 0; i < input.length(); i++)
    {
        const uint8_t c = static_cast<uint8_t>(input[i]);

        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~')
        {
            encoded += static_cast<char>(c);
        }
        else if (c == ' ')
        {
            encoded += "%20";
        }
        else
        {
            snprintf(hex, sizeof(hex), "%%%02X", c);
            encoded += hex;
        }
    }

    return encoded;
}


static void sendTelegramMessage(const String &message)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("Telegram alert skipped: Wi-Fi disconnected."));
        return;
    }

    const String token = TELEGRAM_BOT_TOKEN;

    const String chatIDs[] = {
        TELEGRAM_CHAT_ID1,
        TELEGRAM_CHAT_ID2
    };

    if (token.startsWith("YOUR_"))
    {
        Serial.println(F("Set Telegram bot token in config.h"));
        return;
    }

    for (int i = 0; i < 2; i++)
    {
        if (chatIDs[i].startsWith("YOUR_") || chatIDs[i].length() == 0)
        {
            Serial.print(F("Invalid Telegram Chat ID: Phone "));
            Serial.println(i + 1);
            continue;
        }

        WiFiClientSecure client;
        client.setInsecure();

        HTTPClient https;

        const String url =
            "https://api.telegram.org/bot" + token +
            "/sendMessage?chat_id=" + urlEncode(chatIDs[i]) +
            "&text=" + urlEncode(message);

        if (https.begin(client, url))
        {
            https.setTimeout(7000);

            const int httpCode = https.GET();

            Serial.print(F("Telegram Phone "));
            Serial.print(i + 1);
            Serial.print(F(" HTTP response: "));
            Serial.println(httpCode);

            if (httpCode > 0)
                Serial.println(https.getString());
            else
                Serial.println(https.errorToString(httpCode));

            https.end();
        }
        else
        {
            Serial.print(F("Telegram connection failed for phone "));
            Serial.println(i + 1);
        }
    }
}


void sendLevelNotification(uint8_t threshold, float currentPercentage)
{
    String message;

    if (threshold == 25)
    {
        message = "Water level reached 25%.";
    }
    else if (threshold == 50)
    {
        message = "Water level reached 50% (HALF).";
    }
    else if (threshold == 100)
    {
        message = "WARNING: BUCKET FULL! Water level reached 100%.";
    }
    else
    {
        message = "Water level alert: " + String(threshold) + "%.";
    }

    message += " Current reading: ";
    message += String(currentPercentage, 1);
    message += "%.";

    sendTelegramMessage(message);
}

// Used for LOW / EMPTY warning
void sendNotification(const String &message)
{
    sendTelegramMessage(message);
}
