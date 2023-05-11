#include <string>
#include <vector>


enum class RssiProcessingAlgorithm {
  CLOSEST,
  FIRST,
  QUADRATIC
};


class Competitor {
public:
  Competitor() : name(""), macAddress(""), lapsCount(0), lastLapTime(0), lastLapRssi(-150), bestLapTime(0), lastLapTimestamp(0), startTimestamp(0), processingAlgorithm(RssiProcessingAlgorithm::CLOSEST) {}

  std::string name;
  std::string macAddress;
  uint32_t lapsCount;
  uint32_t lastLapTime;
  int lastLapRssi;
  uint32_t bestLapTime;
  uint32_t lastLapTimestamp;
  uint32_t startTimestamp;
  std::vector<int> rssiBuffer;
  std::vector<uint32_t> timestampBuffer;
  std::vector<uint32_t> allLapTimes;
  RssiProcessingAlgorithm processingAlgorithm;

  void reset();
  void processRssiData();
  void updateLapInfo(uint32_t timestamp, int rssi);
};