#include "ws2812handler.h"
#include "MessageFormatter.h"

uint8_t processMacAddress(const String& macAddress) {
    uint8_t  result = 0;
    for (int i = 0; i < macAddress.length(); i++) {
        if (macAddress[i] != ':') {
            uint8_t hexValue = strtoul(macAddress.substring(i, i + 2).c_str(), nullptr, 16);
            result = (result + hexValue) % 256;
            i++; // Skip the next character, as we already processed two hex digits
        }
    }
    return result;
}

#ifdef USE_WS2812_LED
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(LED_COUNT, LED_PIN);

void turnOffLed() {
  strip.SetPixelColor(0, RgbColor(0, 0, 0));
  strip.Show();
}

void setLedColor(String address, int rssi, int rssiThreshold) {
  // Convert MAC address to integer
  uint8_t macInt = processMacAddress(address);

  // Map MAC address to hue (0.0 - 1.0)
  float hue = static_cast<float>(macInt % 256) / 256.0f;

  // Map RSSI to intensity (0.0 - 1.0)
  int8_t minRssi = rssiThreshold;
  int8_t maxRssi = -30;
  float intensity = static_cast<float>(map(constrain(rssi, minRssi, maxRssi), minRssi, maxRssi, 0, 255)) / 255.0f;

  // Set LED color and intensity
  HslColor hslColor(hue, 1.0f, intensity * 0.5f);
  strip.SetPixelColor(0, hslColor);
  strip.Show();
}

void rainbowSweepLED(unsigned long hueSweepDuration) {
  unsigned long startTime = millis();
  while (millis() - startTime < hueSweepDuration) {
    float hue = (millis() - startTime) / static_cast<float>(hueSweepDuration); // Calculate hue value [0.0, 1.0)
    HslColor hslColor(hue, 1.0, 0.1); // Use lightness value of 0.4
    strip.SetPixelColor(0, hslColor);
    strip.Show();
    delay(10); // Small delay to control the speed of the hue sweep
  }
  turnOffLed();
}
#endif