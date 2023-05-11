#ifndef MESSAGE_FORMATTER_H
#define MESSAGE_FORMATTER_H

#include <Arduino.h>

String formatTime(unsigned long time);
String formatMacAddress(const String& macAddress);
String addColonsToMacAddress(const String& data);
String formatRssi(int rssi);
uint8_t computeCRC(const String& data);
String createMessage(unsigned long time, const String& macAddress, int rssi);

#endif