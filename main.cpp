#include <Arduino.h>
#include <string>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

#include "Config.h"
#include "Competitor.h"

IPAddress Actual_IP;

IPAddress PageIP(192, 168, 1, 1);
IPAddress Gateway(192, 168, 1, 1);
IPAddress Subnet(255, 255, 255, 0);
IPAddress ip;

AsyncWebServer server(80);

Competitor competitors[MAX_DEVICES];

NimBLEScan* pBLEScan;

void deleteCompetitor(int index) {
  if (index >= 0 && index < MAX_DEVICES) {
    competitors[index] = Competitor();
  }
}

int findCompetitorByMAC(const std::string& fullDeviceName) {
  std::string macAddress = fullDeviceName.substr(strlen(Config::DEVICE_NAME_PREFIX) + 1);

  for (int i = 0; i < MAX_DEVICES; i++) {
    if (competitors[i].macAddress.empty()) {
      // This is an empty slot, assign the competitor here
      competitors[i].macAddress = macAddress;
      return i;
    } else if (competitors[i].macAddress == macAddress) {
      // Competitor already exists in the list, return its index
      return i;
    }
  }

  // If all slots are full and we couldn't find a competitor with the given name, return -1
  return -1;
}

void handleGetResults(AsyncWebServerRequest *request) {
  StaticJsonDocument<1024> jsonDocument;
  JsonArray results = jsonDocument.to<JsonArray>();

  for (int i = 0; i < MAX_DEVICES; i++) {
    JsonObject result = results.createNestedObject();
    result["name"] = (competitors[i].name.empty()) ? competitors[i].macAddress : competitors[i].name;
    result["lapsCount"] = competitors[i].lapsCount;
    result["lastLapTime"] = competitors[i].lastLapTime;
    result["bestLapTime"] = competitors[i].bestLapTime;
    result["rssi"] = competitors[i].lastLapRssi;
  }

  String jsonResponse;
  serializeJsonPretty(jsonDocument, jsonResponse);
  request->send(200, "application/json", jsonResponse);
}

void processRssiDataForAllDevices() {
  for (int i = 0; i < MAX_DEVICES; i++) {
    if (!competitors[i].timestampBuffer.empty() && millis() - competitors[i].timestampBuffer.back() > LAP_COOLDOWN) {
      competitors[i].processRssiData();
      competitors[i].rssiBuffer.clear();
      competitors[i].timestampBuffer.clear();
      ledcWrite(pwmChannel, 0); // Turn off the LED
    }
  }
}

class MyAdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {
public:
  void onResult(NimBLEAdvertisedDevice *advertisedDevice)
  {
    if (strstr(advertisedDevice->getName().c_str(), Config::DEVICE_NAME_PREFIX))
    {
      int competitorIndex = findCompetitorByMAC(advertisedDevice->getName().c_str());
      if (competitorIndex != -1) {
        // If we found a competitor or assigned a new one, add the current RSSI value and timestamp to the buffer
        int rssi = advertisedDevice->getRSSI();
        int ledIntensity = map(constrain(rssi, rssiMin, rssiMax), rssiMin, rssiMax, ledIntensityMin, ledIntensityMax);
        ledcWrite(pwmChannel, ledIntensity);
        competitors[competitorIndex].rssiBuffer.push_back(rssi);
        competitors[competitorIndex].timestampBuffer.push_back(millis());
        } else {
        // If we couldn't find or assign a competitor, print an error message
        Serial.println("Error: All competitor slots are full. Cannot track new devices.");
      }
    } else {
      ledcWrite(pwmChannel, 0); // Turn off the LED
    }
  }
};

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    return;
  }

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/race_results.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/race_results.css", "text/css");
  });
  server.on("/race_results.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/race_results.js", "application/javascript");
  });
  server.on("/get_results", HTTP_GET, handleGetResults);

  server.on("/update_name", HTTP_POST, [](AsyncWebServerRequest *request) {
    int index = request->arg("index").toInt() - 1;
    String newName = request->arg("name");
    competitors[index].name = std::string(newName.c_str());
    request->send(200, "text/plain", "Name updated");
  });

  server.on("/reset_contestant", HTTP_POST, [](AsyncWebServerRequest *request) {
  int index = request->arg("index").toInt() - 1;
  // Reset the contestant's data here
  competitors[index].reset(); // Assuming there is a reset() function in the Competitor class
  request->send(200, "text/plain", "Contestant reset");
});

server.on("/delete_contestant", HTTP_POST, [](AsyncWebServerRequest *request) {
  int index = request->arg("index").toInt() - 1;
  // Delete the contestant's data here
  deleteCompetitor(index); // Assuming there is a delete() function in the Competitor class
  request->send(200, "text/plain", "Contestant deleted");
});

  // Print the SSID and password
  Serial.print("SSID: ");
  Serial.println(Config::ssid);
  Serial.print("Password: ");
  Serial.println(Config::password);

  // Create access point
  WiFi.mode(WIFI_AP);
  delay(250);
  WiFi.softAP(Config::ssid, Config::password);
  delay(250);
  WiFi.softAPConfig(PageIP, Gateway, Subnet);
  delay(100);
  Actual_IP = WiFi.softAPIP();

  // Start server
  server.begin();

  NimBLEDevice::init("");
  pBLEScan = NimBLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
  pBLEScan->setActiveScan(true); // Set active scanning, this will get more data from the advertiser.
  pBLEScan->setInterval(15); // How often the scan occurs / switches channels; in milliseconds,
  pBLEScan->setWindow(12);  // How long to scan during the interval; in milliseconds.
  pBLEScan->setMaxResults(0); // do not store the scan results, use callback only.

  // Set up the LED pin for PWM output
  pinMode(ledPin, OUTPUT);

  // Configure the PWM channel
  ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
  ledcAttachPin(ledPin, pwmChannel);
}

void loop() {
  // If an error occurs that stops the scan, it will be restarted here.
  if(pBLEScan->isScanning() == false) {
      // Start scan with: duration = 0 seconds(forever), no scan end callback, not a continuation of a previous scan.
      pBLEScan->start(0, nullptr, false);
  }
  processRssiDataForAllDevices();
  delay(100);
}