#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_bt.h>
#include <WiFi.h>
#include <NeoPixelBus.h>

#define LED_COUNT 1
#define LED_PIN 7 // The WS2812 data pin on LOLIN C3 Mini is GPIO7

// #define USE_WS2812_LED // Comment this line out to use the built-in LED instead of the WS2812 LED

#ifdef USE_WS2812_LED
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(LED_COUNT, LED_PIN);
const int ledPin = LED_BUILTIN;
#endif

const char* DEVICE_NAME_PREFIX = "DIL";

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


BLECharacteristic* pCharacteristic;

void blinkLED(unsigned long hueSweepDuration = 2000) {
  #ifdef USE_WS2812_LED
    unsigned long startTime = millis();
    while (millis() - startTime < hueSweepDuration) {
      float hue = (millis() - startTime) / static_cast<float>(hueSweepDuration); // Calculate hue value [0.0, 1.0)
      HslColor hslColor(hue, 1.0, 0.1); // Use lightness value of 0.4
      strip.SetPixelColor(0, hslColor);
      strip.Show();
      delay(10); // Small delay to control the speed of the hue sweep
    }
  #endif
}

void setPowerLevel(float power) {
  if (power < -27) {
    power = -27;
  } else if (power > 18) {
    power = 18;
  }
  int8_t power_index = round((power + 27) / 3); // Convert power to index
  esp_power_level_t power_level = (esp_power_level_t)power_index;
  BLEDevice::setPower(power_level);
  Serial.print("Power level set to: ");
  Serial.print(power);
  Serial.println(" dBm");
  #ifdef USE_WS2812_LED
    float hue = 0.67 * (power + 27.0) / (18.0 + 27.0); // Map the power level to the hue value between red (0.0) and blue (0.67)
    HslColor hslColor(hue, 1.0, 0.1);
    strip.SetPixelColor(0, hslColor);
    strip.Show();
  #endif
}

// Custom callback class for the characteristic
class PowerLevelCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      char *end;
      float power = strtof(value.c_str(), &end);
      if (end != value.c_str()) {
        setPowerLevel(power);
      } else {
        Serial.println("Invalid input received over Bluetooth.");
      }
    }
  }
};

void setup() {
  #ifdef USE_WS2812_LED
    pinMode(ledPin, OUTPUT);
  #endif
  Serial.begin(115200);
  // Get the MAC address
  String macAddress = WiFi.macAddress();
  macAddress.replace(":", ""); // Remove colons from the MAC address

  // Create the device name
  String deviceName = String(DEVICE_NAME_PREFIX) + "_" + macAddress;

 // Initialize the device with the new name
  BLEDevice::init(deviceName.c_str());
  BLEDevice::setPower(ESP_PWR_LVL_P9);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  // Set the callback for the characteristic
  pCharacteristic->setCallbacks(new PowerLevelCallback());
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Racing module is up and running!");
  #ifdef USE_WS2812_LED
    strip.Begin();
    strip.Show();
    blinkLED();
  #endif

  Serial.println("Enter power level in dBm (from -27 to 18):");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    char *end;
    float power = strtof(input.c_str(), &end);
    if (end != input.c_str()) {
      setPowerLevel(power);
    } else {
      Serial.println("Invalid input. Please enter a valid number.");
    }
    Serial.println("Enter power level in dBm (from -27 to 18):");
  }
}