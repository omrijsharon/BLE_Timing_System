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
  if (rssiBuffer.size() < 3)
  {
    // Handle the cases with only 1 or 2 measurement points
    // Use the highest measurement point as the "closest" point
    if (rssiBuffer.size() == 2)
    {
      maximumIndex = (rssiBuffer[0] > rssiBuffer[1]) ? 0 : 1;
    }
    updateLapInfo(timestampBuffer[maximumIndex], rssiBuffer[maximumIndex]);
    return;
  }

  for (size_t i = 1; i < rssiBuffer.size() - 1; ++i)
  {
    float prevDerivative = rssiBuffer[i] - rssiBuffer[i - 1];
    float nextDerivative = rssiBuffer[i + 1] - rssiBuffer[i];

    if (prevDerivative > 0 && nextDerivative < 0 && rssiBuffer[i] > maxRssi)
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
  }
}