#ifndef WS2812HANDLER_H
#define WS2812HANDLER_H

#include <Arduino.h>
#include <NeoPixelBus.h>

#define LED_COUNT 1
#define LED_PIN 7 // The WS2812 data pin on LOLIN C3 Mini is GPIO7
#define USE_WS2812_LED // Comment this line out to use the built-in LED instead of the WS2812 LED
#ifdef USE_WS2812_LED
    const int pwmPin = 10; // GPIO5 for PWM output
    const int pwmFrequency = 500; // PWM frequency in Hz
    const int pwmResolution = 8;   // PWM resolution in bits (8 bits = 0 to 255)

    extern NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip;
    const int ledPin = LED_BUILTIN;
#endif
uint8_t processMacAddress(const String& macAddress);
void turnOffLed();
void setLedColor(String address, int rssi, int rssiThreshold);
void rainbowSweepLED(unsigned long hueSweepDuration);

#endif // WS2812HANDLER_H
