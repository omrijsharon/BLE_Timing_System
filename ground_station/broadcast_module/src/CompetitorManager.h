#pragma once
#include "Config.h"
#include "Competitor.h"
#include "MessageDecoder.h"
#include <string>
#include <ArduinoJson.h>

class CompetitorManager {
public:
  Competitor competitors[MAX_DEVICES];
  int findCompetitorByMAC(const std::string& macAddress);
  bool updateCompetitorName(const String& macAddress, const String& newName);
  StaticJsonDocument<1024> createJsonResults();
  void processRssiData();
  void updateCompetitorData(const DecodedMessage& decodedMessage);
  String getCompetitorLapTimes(int competitorIndex);
  String summary();
  void resetAll();
  void changeRssiProcessingAlgorithm(RssiProcessingAlgorithm newAlgorithm);
private:
};