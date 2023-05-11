#include <string.h>
#include "MessageDecoder.h"

uint8_t computeCRC(const String& data) {
  uint8_t crc = 0;
  for (size_t i = 0; i < data.length(); i++) {
    crc ^= data[i];
  }
  return crc;
}

unsigned long parseTime(const String& data) {
  return data.substring(0, 10).toInt();
}

String parseMacAddress(const String& data) {
  String mac = "";
  for (int i = 10; i < 22; i += 2) {
    mac += data.substring(i, i + 2);
    if (i < 20) {
      mac += ":";
    }
  }
  return mac;
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

int parseRssi(const String& data) {
  return -data.substring(22, 24).toInt();
}

uint8_t parseCRC(const String& data) {
  return (uint8_t)strtoul(data.substring(24, 26).c_str(), nullptr, 16);
}

bool checkCRC(const String& data, uint8_t crc) {
  uint8_t computedCRC = computeCRC(data);
  return computedCRC == crc;
}

bool MessageDecoder::decodeMessage(const String& message, DecodedMessage& decodedMessage) {
  if (message.length() != 26) {
    return false;
  }
  String data = message.substring(0, 24);
  uint8_t crc = parseCRC(message);
  if (!checkCRC(data, crc)) {
    return false;
  }
  decodedMessage.time = parseTime(data);
  decodedMessage.macAddress = parseMacAddress(data);
  decodedMessage.rssi = parseRssi(data);
  return true;
}

DecodedMessage MessageDecoder::getMessage() const {
  return message;
}

// a function that transforms time in milliseconds to a string in the format "mm:ss.ms"
String timeToString(unsigned long time) {
  unsigned long ms = time % 1000;
  time /= 1000;
  unsigned long s = time % 60;
  time /= 60;
  unsigned long m = time % 60;
  String timeString = "";
  if (m < 10) {
    timeString += "0";
  }
  timeString += m;
  timeString += ":";
  if (s < 10) {
    timeString += "0";
  }
  timeString += s;
  timeString += ".";
  if (ms < 10) {
    timeString += "00";
  } else if (ms < 100) {
    timeString += "0";
  }
  timeString += ms;
  return timeString;
}