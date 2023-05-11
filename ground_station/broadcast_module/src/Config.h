#ifndef CONFIG_H
#define CONFIG_H

#define USE_WS2812_LED // Comment this line out to use the built-in LED instead of the WS2812 LED

#define WIFI_SSID "BreakingCarbon"
#define WIFI_PASSWORD "n3v3rdisarm"

#define MAX_LAPS 24
#define MAX_DEVICES 16
#define LAP_COOLDOWN 5000

const int pwmPin = 10; // Choose a PWM-capable pin for the LED
const int pwmChannel = 0;
const int pwmResolution = 8; // 8-bit resolution (0-255)
const int pwmFrequency = 500; // frequency
const int rssiMin = -100;
const int rssiMax = -60;
const int ledIntensityMin = 10;
const int ledIntensityMax = 250;

#endif // CONFIG_H
