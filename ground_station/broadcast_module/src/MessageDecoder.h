#ifndef MESSAGE_DECODER_H
#define MESSAGE_DECODER_H

#include <Arduino.h>


String addColonsToMacAddress(const String& data);

struct DecodedMessage {
  unsigned long time;
  String macAddress;
  int rssi;
};

class MessageDecoder {
public:
  // Process incoming character and returns true if a complete message has been received and decoded
  bool processMessage(const String& message);
  // Get the decoded message
  DecodedMessage getMessage() const;
  bool decodeMessage(const String& message, DecodedMessage& decodedMessage);

private:
  enum class State {
    WAIT_FOR_START,
    READ_DATA,
    READ_CRC
  };

  State state = State::WAIT_FOR_START;
  String buffer;
  String crcBuffer;
  DecodedMessage message;

};

String timeToString(unsigned long time);

#endif // MESSAGE_DECODER_H