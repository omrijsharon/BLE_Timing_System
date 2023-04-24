#pragma once

namespace Config {
    constexpr const char* ssid = "DroneIsraelLeague";
    constexpr const char* password = "n3v3rd1sarm";
    constexpr const char* DEVICE_NAME_PREFIX = "DIL";
}

#define MAX_LAPS 24
#define MAX_DEVICES 8
#define LAP_COOLDOWN 5000

const int ledPin = 5; // Choose a PWM-capable pin for the LED
const int pwmChannel = 0;
const int pwmResolution = 8; // 8-bit resolution (0-255)
const int pwmFrequency = 5000; // 5kHz frequency
int rssiMin = -100;
int rssiMax = -60;
int ledIntensityMin = 0;
int ledIntensityMax = 255;