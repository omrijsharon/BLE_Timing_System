#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <NeoPixelBus.h>
#include <EEPROM.h>
#include <vector>
#include <HardwareSerial.h>

#include "MessageFormatter.h"
#include "WS2812handler.h"

#define BAUD_RATE 500000

#define RX_PIN 20
#define TX_PIN 21

HardwareSerial hSerial0(0);

int rssiThreshold = -99; // Minimum RSSI value to consider for LED color (default: -99)

const int rssiThresholdAddress = 0;
const int windowValueAddress = sizeof(int); // Store the window value right after the rssiThreshold value
std::vector<String> allowedDevices;

NimBLEScan* pBLEScan;

void saveValueToEEPROM(int address, int value) {
  EEPROM.put(address, value);
  EEPROM.commit();
}

int loadValueFromEEPROM(int address) {
  int value;
  EEPROM.get(address, value);
  return value;
}

bool isDeviceRegistered(const String& macAddress) {
  for (const auto& allowedDevice : allowedDevices) {
    if (macAddress == allowedDevice) {
      return true;
    }
  }
  return false;
}

void printMessage(const BLEAddress& address, int rssi, unsigned long time) {
  #ifdef USE_WS2812_LED
    setLedColor(address.toString().c_str(), rssi, rssiThreshold);
  #endif
  String message = createMessage(time, address.toString().c_str(), rssi);
  hSerial0.write(message.c_str(), 27);
}

void applySettings(const String &input) {
  char settingType = input.charAt(0);
  int value = input.substring(1).toInt();

  if (settingType == 'W' || settingType == 'w') {
    if (value > 0) {
      saveValueToEEPROM(windowValueAddress, value);
      int newInterval = round(value * 1.1);
      // Check if new interval is smaller than new window
      if (newInterval <= value) {
        newInterval = value + 1; // Make sure the new interval is larger than the new window
      }
      pBLEScan->stop();
      delay(1000);
      pBLEScan->setWindow(value);
      pBLEScan->setInterval(newInterval);
      Serial.printf("New settings: Window = %d ms, Interval = %d ms\n", value, newInterval);
      pBLEScan->start(0, nullptr, false); // Restart the scanner after applying new settings
    }
    } else if (settingType == 'T' || settingType == 't') { // Add a new setting type for threshold
        rssiThreshold = -1 * value;
        saveValueToEEPROM(rssiThresholdAddress, rssiThreshold); // Save the new threshold value to EEPROM
        Serial.printf("New RSSI threshold: %d\n", rssiThreshold);
        delay(1000);
    }
}

class MyAdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {
public:
  void onResult(NimBLEAdvertisedDevice *advertisedDevice) {
  int rssi = advertisedDevice->getRSSI();
  String macAddress = advertisedDevice->getAddress().toString().c_str();

  // Register new devices with RSSI greater than -50
  if (rssi > -50 && !isDeviceRegistered(macAddress)) {
    allowedDevices.push_back(macAddress);
    // print the device mac address and its corresponding hue color using processMacAddress
    Serial.printf("New device registered: %s, hue: %d\n", macAddress.c_str(), processMacAddress(macAddress.c_str()));
  }

  // Process and send messages for allowed devices only
  if (rssi > rssiThreshold && isDeviceRegistered(macAddress)) {
    printMessage(advertisedDevice->getAddress(), rssi, millis());

  } else {
    turnOffLed(); // Turn off the LED when the RSSI value is below the threshold or the device is not allowed
    // setPwmForRssi(-100); // Set the PWM value based on the RSSI
  }
  }
};

void setup() {
  Serial.begin(BAUD_RATE);
  hSerial0.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
  #ifdef USE_WS2812_LED
    ledcSetup(0, pwmFrequency, pwmResolution); // Configure PWM properties for channel 0
    ledcAttachPin(pwmPin, 0); // Attach the PWM pin to channel 0
    strip.Begin();
    strip.Show();
    rainbowSweepLED(1000);
  #endif

  EEPROM.begin(512); // Initialize EEPROM with 512 bytes of storage

  // Load saved values from EEPROM
  int savedRssiThreshold = loadValueFromEEPROM(rssiThresholdAddress);
  int savedWindowValue = loadValueFromEEPROM(windowValueAddress);

  NimBLEDevice::init("");
  pBLEScan = NimBLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
  pBLEScan->setActiveScan(false); // Set active scanning, this will get more data from the advertiser.
  pBLEScan->setMaxResults(0); // do not store the scan results, use callback only.
    // Check if loaded values are valid, if not, use default values
  if (savedRssiThreshold != 0 && savedRssiThreshold <= 0) {
    rssiThreshold = savedRssiThreshold;
    Serial.printf("Loaded RSSI threshold: %d\n", rssiThreshold);
    Serial.println("Use 'T' command to change the RSSI threshold.");
  }
  if (savedWindowValue != 0 && savedWindowValue > 0) {
    pBLEScan->setWindow(savedWindowValue);
    pBLEScan->setInterval(savedWindowValue + 2);
    Serial.printf("Loaded settings: Window = %d ms, Interval = %d ms\n", savedWindowValue, savedWindowValue + 2);
    Serial.println("Use 'W' command to change the scan window.");
  } else {
    pBLEScan->setInterval(8); // How often the scan occurs / switches channels; in milliseconds,
    pBLEScan->setWindow(6);  // How long to scan during the interval; in milliseconds.
  }
}

void loop() {
  if(pBLEScan->isScanning() == false) {
    // Start scan with: duration = 0 seconds(forever), no scan end callback, not a continuation of a previous scan.
    pBLEScan->start(0, nullptr, false);
  }
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove any whitespace or newline characters
    applySettings(input);
  }
}