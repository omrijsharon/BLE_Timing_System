#ifndef WS2812HANDLER_H
#define WS2812HANDLER_H

#include <Arduino.h>
#include <NeoPixelBus.h>
#include "Config.h"

#define LED_COUNT 1
#define LED_PIN 7 // The WS2812 data pin on LOLIN C3 Mini is GPIO7
#ifdef USE_WS2812_LED
    extern NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip;
#endif

void turnOffLed();
void setLedColor(String address, int rssi);
void rainbowSweepLED(unsigned long hueSweepDuration);
#endif // WS2812HANDLER_H
