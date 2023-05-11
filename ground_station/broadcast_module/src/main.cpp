#include <Arduino.h>
#include "Config.h"
#include <HardwareSerial.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

// #include "Competitor.h"
#include "CompetitorManager.h"
#include "MessageDecoder.h"

#define BAUD_RATE 500000

#define RX_PIN 20
#define TX_PIN 21

#ifdef USE_WS2812_LED
  #include <NeoPixelBus.h>
  #include "ws2812handler.h"
  const int ledPin = LED_BUILTIN;
#endif

HardwareSerial hSerial0(0);

IPAddress Actual_IP;

IPAddress PageIP(192, 168, 1, 1);
IPAddress Gateway(192, 168, 1, 1);
IPAddress Subnet(255, 255, 255, 0);
IPAddress ip;

AsyncWebServer server(80);

MessageDecoder messageDecoder;
CompetitorManager competitorManager;

unsigned long prevTime = 0;
unsigned long lastProcessAndDisplayRssiDataTime = 0;
long timeOffset = 0;
bool isFirstMessage = true;

void handleGetResults(AsyncWebServerRequest *request) {
  StaticJsonDocument<1024> jsonDocument = competitorManager.createJsonResults();
  // Serial.print(competitorManager.summary());
  String jsonResponse;
  serializeJsonPretty(jsonDocument, jsonResponse);
  request->send(200, "application/json", jsonResponse);
}

void handleResetAllContestants(AsyncWebServerRequest *request) {
  competitorManager.resetAll();
  request->send(200, "text/plain", "All contestants have been reset.");
}

void handleGetLapTimes(AsyncWebServerRequest *request) {
  if (request->hasParam("macAddress", true)) {
    String macAddress = request->getParam("macAddress", true)->value();
    int competitorIndex = competitorManager.findCompetitorByMAC(macAddress.c_str());

    if (competitorIndex >= 0) {
      String jsonResponse = competitorManager.getCompetitorLapTimes(competitorIndex);
      request->send(200, "application/json", jsonResponse);
    } else {
      request->send(404, "text/plain", "Competitor not found.");
    }
  } else {
    request->send(400, "text/plain", "Missing 'macAddress' parameter.");
  }
}

void setup() {
  Serial.begin(BAUD_RATE);
  hSerial0.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

  Actual_IP = WiFi.softAPIP();
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
    String macAddress = request->arg("macAddress");
    String newName = request->arg("name");
    
    if (newName.length() > 0) {
        bool isUpdated = competitorManager.updateCompetitorName(macAddress, newName);
        if (isUpdated) {
            request->send(200, "text/plain", "Name updated");
        } else {
            request->send(404, "text/plain", "Competitor not found");
        }
    } else {
        request->send(400, "text/plain", "Name cannot be empty");
    }
  });
  
  server.on("/reset_contestant", HTTP_POST, [](AsyncWebServerRequest *request) {
    String macAddress = request->arg("macAddress");
    int index = competitorManager.findCompetitorByMAC(macAddress.c_str());
    // print the mac address and index of the competitor
    // Serial.printf("Resetting - MAC address: %s, index: %d \r \n", macAddress.c_str(), index);
    if (index != -1) {
      // Reset the contestant's data here
      competitorManager.competitors[index].reset(); // Assuming there is a reset() function in the Competitor class
      request->send(200, "text/plain", "Contestant reset");
    } else {
      request->send(400, "text/plain", "Contestant not found");
    }
  });

  server.on("/reset_all_contestants", HTTP_POST, handleResetAllContestants);

  server.on("/get_lap_times", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleGetLapTimes(request);
  });

  server.on("/change_algorithm", HTTP_POST, [](AsyncWebServerRequest *request) {
    String newAlgorithmString;
    if (request->hasParam("algorithm", true)) {
      newAlgorithmString = request->getParam("algorithm", true)->value();
    } else {
      request->send(400, "text/plain", "Missing 'algorithm' parameter");
      return;
    }

    RssiProcessingAlgorithm newAlgorithm;
    if (newAlgorithmString == "CLOSEST") {
      newAlgorithm = RssiProcessingAlgorithm::CLOSEST;
    } else if (newAlgorithmString == "FIRST") {
      newAlgorithm = RssiProcessingAlgorithm::FIRST;
    } else if (newAlgorithmString == "QUADRATIC") {
      // newAlgorithm = RssiProcessingAlgorithm::QUADRATIC;
      request->send(400, "text/plain", "algorithm not implemented");
      return;
    } else {
      request->send(400, "text/plain", "Invalid 'algorithm' parameter");
      return;
    }

    competitorManager.changeRssiProcessingAlgorithm(newAlgorithm);
    request->send(200, "text/plain", "Algorithm changed successfully");
  });


  #ifdef USE_WS2812_LED
    pinMode(ledPin, OUTPUT);
    strip.Begin();
    strip.Show();
    rainbowSweepLED(1000);
  #endif
  competitorManager.resetAll();
    // Create access point
  WiFi.mode(WIFI_AP);
  delay(250);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  delay(250);
  WiFi.softAPConfig(PageIP, Gateway, Subnet);
  delay(100);
  server.begin();
}

void loop() {
  unsigned long currentTime = millis();
  if (hSerial0.available()) {
    String msg = hSerial0.readStringUntil('$');
    DecodedMessage decodedMessage;

    if (messageDecoder.decodeMessage(msg, decodedMessage)) {
      /*
      // Calculate the time offset when the first message is received
      // Synchronizing the clocks of the two modules.
      if (isFirstMessage) {
        timeOffset = millis() - decodedMessage.time;
        isFirstMessage = false;
      }
      // Adjust the time of the decoded message with the calculated time offset
      decodedMessage.time += timeOffset;
      */
      decodedMessage.time = currentTime;
      competitorManager.updateCompetitorData(decodedMessage);

      /*
      Serial.print("Decoded message received ---");
      Serial.print(" Time: ");
      Serial.print(timeToString(decodedMessage.time));
      Serial.print(" MAC Address: ");
      Serial.print(decodedMessage.macAddress);
      Serial.print(" RSSI: ");
      Serial.print(decodedMessage.rssi);
      // print the difference between the decodedMessage.time and prevTime
      Serial.print(" Time difference: ");
      Serial.print(decodedMessage.time - prevTime);
      Serial.println(" ms");
      */
      prevTime = decodedMessage.time;
      #ifdef USE_WS2812_LED
        // Add code to control the WS2812 LED and PWM output here
        setLedColor(decodedMessage.macAddress, decodedMessage.rssi);
      #endif
    }
  }
  if (currentTime - lastProcessAndDisplayRssiDataTime >= 500) {
    competitorManager.processRssiData();
    lastProcessAndDisplayRssiDataTime = currentTime;
  }
}