#include <string>
#include <vector>

class Competitor {
public:
  Competitor() : name(""), macAddress(""), lapsCount(0), lastLapTime(0), lastLapRssi(-150), bestLapTime(0), lastLapTimestamp(0), startTimestamp(0) {}

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

  void reset();
  void processRssiData();
  void updateLapInfo(uint32_t timestamp, int rssi);
};