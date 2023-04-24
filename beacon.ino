#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_bt.h>
#include <WiFi.h>

const char* DEVICE_NAME_PREFIX = "DIL";

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic* pCharacteristic;

void setup() {
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

  pCharacteristic->setValue("");
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Racing module is up and running!");
}

void loop() {
  delay(1000);
}