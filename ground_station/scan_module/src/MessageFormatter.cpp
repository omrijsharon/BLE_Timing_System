#include "MessageFormatter.h"

String formatTime(unsigned long time) {
  char buffer[11];
  snprintf(buffer, sizeof(buffer), "%010lu", time);
  return String(buffer);
}

String formatMacAddress(const String& macAddress) {
  String formattedMac = macAddress;
  formattedMac.replace(":", "");
  return formattedMac;
}

String addColonsToMacAddress(const String& data) {
  String mac = "";
  for (int i = 10; i < 22; i += 2) {
    mac += data.substring(i, i + 2);
    if (i < 20) {
      mac += ":";
    }
  }
  return mac;
}

String formatRssi(int rssi) {
  char buffer[3];
  int formattedRssi = max(-99, rssi);
  snprintf(buffer, sizeof(buffer), "%02d", -formattedRssi);
  return String(buffer);
}

uint8_t computeCRC(const String& data) {
  uint8_t crc = 0;
  for (size_t i = 0; i < data.length(); i++) {
    crc ^= data[i];
  }
  return crc;
}

String createMessage(unsigned long time, const String& macAddress, int rssi) {
  String header = "$";
  String formattedTime = formatTime(time); //time size is 10
  String formattedMac = formatMacAddress(macAddress); //mac size is 12
  String formattedRssi = formatRssi(rssi); //rssi size is 2
  String data = formattedTime + formattedMac + formattedRssi;
  uint8_t crc = computeCRC(data); // crc size is 2
  char crcBuffer[3];
  snprintf(crcBuffer, sizeof(crcBuffer), "%02X", crc);
  String message = header + data + String(crcBuffer);
  return message;
}