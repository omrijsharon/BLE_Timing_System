#include <Arduino.h>

#include "Config.h"
#include "Competitor.h"
#include "MessageDecoder.h"


void Competitor::reset() {
  lapsCount = 0;
  lastLapTime = 0;
  lastLapRssi = -150;
  bestLapTime = 0;
  lastLapTimestamp = 0;
  startTimestamp = 0;
  rssiBuffer.clear();
  timestampBuffer.clear();
  allLapTimes.clear();
}

void Competitor::processRssiData()
{
  int maximumIndex = -1;
  float maxRssi = -150;

  if (rssiBuffer.empty())
  {
    return;
  }

  for (size_t i = 0; i < rssiBuffer.size(); ++i)
  {
    if (rssiBuffer[i] > maxRssi)
    {
      maxRssi = rssiBuffer[i];
      maximumIndex = i;
    }
  }

if (maximumIndex != -1) {
    switch (processingAlgorithm) {
      case RssiProcessingAlgorithm::CLOSEST:
        Serial.println("Processing RSSI by CLOSEST");
        updateLapInfo(timestampBuffer[maximumIndex], maxRssi);
        break;
      case RssiProcessingAlgorithm::FIRST:
        Serial.println("Processing RSSI by FIRST");
        updateLapInfo(timestampBuffer[0], maxRssi);
        break;
      case RssiProcessingAlgorithm::QUADRATIC:
        Serial.println("Processing RSSI by QUADRATIC");
        // Implement the quadratic curve algorithm here
        break;
    }
  }
}

void Competitor::updateLapInfo(uint32_t timestamp, int rssi) {
  if (startTimestamp == 0)
  { // First pass, begining of the first lap
    Serial.println("First pass detected");
    startTimestamp = timestamp;
    lastLapTimestamp = timestamp;
  } else
  { // New lap
    uint32_t currentLapTime = timestamp - lastLapTimestamp;
    lastLapTime = currentLapTime;

    // if allLapTimes is full, remove the first element
    if (allLapTimes.size() == MAX_LAPS) {
        allLapTimes.erase(allLapTimes.begin());
    }
    allLapTimes.push_back(currentLapTime);
    lastLapTimestamp = timestamp;
    lastLapRssi = rssi;
    if (lapsCount == 0 || currentLapTime < bestLapTime) {
        bestLapTime = currentLapTime;
    }
    lapsCount++;
    Serial.printf("Lap #%d detected: %s ms\n", lapsCount, timeToString(currentLapTime));
    Serial.println();
  }
}