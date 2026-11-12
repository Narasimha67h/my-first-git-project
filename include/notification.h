#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <Arduino.h>

void sendLevelNotification(uint8_t threshold, float currentPercentage);
void sendNotification(const String &message);

#endif