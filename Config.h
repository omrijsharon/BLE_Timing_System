#pragma once

namespace Config {
    constexpr const char* ssid = "DroneIsraelLeague";
    constexpr const char* password = "n3v3rd1sarm";
    constexpr const char* DEVICE_NAME_PREFIX = "DIL";
}

#define MAX_LAPS 24
#define MAX_DEVICES 8
#define LAP_COOLDOWN 5000

const int ledPin = 10; // Choose a PWM-capable pin for the LED
const int pwmChannel = 0;
const int pwmResolution = 8; // 8-bit resolution (0-255)
const int pwmFrequency = 500; // frequency
const int rssiMin = -100;
const int rssiMax = -60;
const int ledIntensityMin = 10;
const int ledIntensityMax = 250;