#include <Arduino.h>

#include "Config.h"
#include "Competitor.h"


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

  if (maximumIndex != -1)
  {
    updateLapInfo(timestampBuffer[maximumIndex], maxRssi);
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
    Serial.printf("New lap detected: %d ms\n", currentLapTime);
    Serial.println();
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
  }
}