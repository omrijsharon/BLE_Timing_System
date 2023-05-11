#include "CompetitorManager.h"
#include "Config.h"
#include "ws2812handler.h"
#include <cstring>


int CompetitorManager::findCompetitorByMAC(const std::string& macAddress) {
  for (int i = 0; i < MAX_DEVICES; i++) {
    if (competitors[i].macAddress.empty()) {
      // This is an empty slot, assign the competitor here
      competitors[i].macAddress = macAddress;
      // Initialize other fields of the Competitor class using the reset() function
      competitors[i].reset();
      // Serial.printf("New competitor detected: %s (%s) at index %d \r \n", competitors[i].name.c_str(), competitors[i].macAddress.c_str(), i);
      return i;
    } else if (competitors[i].macAddress == macAddress) {
      // Competitor already exists in the list, return its index
      return i;
    }
  }

  // Return -1 if all slots are full
  return -1;
}

StaticJsonDocument<1024> CompetitorManager::createJsonResults() {
  StaticJsonDocument<1024> jsonDocument;
  JsonArray results = jsonDocument.to<JsonArray>();

  for (int i = 0; i < MAX_DEVICES; i++) {
    JsonObject result = results.createNestedObject();
    result["name"] = competitors[i].name;
    result["macAddress"] = competitors[i].macAddress;
    result["lapsCount"] = competitors[i].lapsCount;
    result["lastLapTime"] = competitors[i].lastLapTime;
    result["bestLapTime"] = competitors[i].bestLapTime;
    result["rssi"] = competitors[i].lastLapRssi;
  }

  return jsonDocument;
}

bool CompetitorManager::updateCompetitorName(const String& macAddress, const String& newName) {
    int index = findCompetitorByMAC(macAddress.c_str());
    if (index >= 0 && index < MAX_DEVICES) {
        competitors[index].name = std::string(newName.c_str());
        return true;
    }
    return false;
}

void CompetitorManager::processRssiData() {
  bool ledOn = true;
  for (int i = 0; i < MAX_DEVICES; i++) {
    if (!competitors[i].timestampBuffer.empty() && millis() - competitors[i].timestampBuffer.back() > 100) {
      ledOn = false;
    }
  }
  if (!ledOn) {
    turnOffLed(); // Use turnOffLed() instead of ledcWrite
  }
  // Serial.println("LED turned off");
  for (int i = 0; i < MAX_DEVICES; i++) {
    if (!competitors[i].timestampBuffer.empty() && (millis() - competitors[i].timestampBuffer.front()) > LAP_COOLDOWN) {
      // Serial.print(millis() - competitors[i].timestampBuffer.front());
      // Serial.printf(" !!! Processing RSSI data for device %s", competitors[i].macAddress.c_str());
      // Serial.println(" !!! ");
      competitors[i].processRssiData();
      competitors[i].rssiBuffer.clear();
      competitors[i].timestampBuffer.clear();
      
      // Set the LED color based on the competitor's address and RSSI
      // setLedColor(competitors[i].macAddress.c_str(), competitors[i].lastLapRssi);
    }
  }
}

void CompetitorManager::updateCompetitorData(const DecodedMessage& decodedMessage) {
    int competitorIndex = findCompetitorByMAC(decodedMessage.macAddress.c_str());

    if (competitorIndex != -1) {
        competitors[competitorIndex].rssiBuffer.push_back(decodedMessage.rssi);
        competitors[competitorIndex].timestampBuffer.push_back(decodedMessage.time);
    } else {
        Serial.println("Error: All competitor slots are full. Cannot track new devices.");
    }
}

String CompetitorManager::getCompetitorLapTimes(int competitorIndex) {
  DynamicJsonDocument jsonDocument(1024);
  JsonArray lapTimesArray = jsonDocument.createNestedArray("lapTimes");

  if (competitorIndex >= 0 && competitorIndex < MAX_DEVICES) {
    Competitor &competitor = competitors[competitorIndex];
    for (uint32_t lapTime : competitor.allLapTimes) {
      lapTimesArray.add(lapTime);
    }
  }

  String jsonResponse;
  serializeJson(jsonDocument, jsonResponse);
  return jsonResponse;
}

String CompetitorManager::summary() {
  String result = "";
  for (int i = 0; i < MAX_DEVICES; i++) {
    if (!competitors[i].macAddress.empty()) {
      result += "MAC Address: " + String(competitors[i].macAddress.c_str()) + ", Name: " + String(competitors[i].name.c_str()) + ", Laps: " + String(competitors[i].lapsCount) + "\r\n";
    }
  }
  return result;
}

void CompetitorManager::resetAll() {
  for (int i = 0; i < MAX_DEVICES; i++) {
    competitors[i].reset();
  }
}

void CompetitorManager::changeRssiProcessingAlgorithm(RssiProcessingAlgorithm newAlgorithm) {
  for (int i = 0; i < MAX_DEVICES; i++) {
    competitors[i].processingAlgorithm = newAlgorithm;
  }
}