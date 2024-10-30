#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Arduino.h>

#define SERVICE_UUID        "7e9d09e2-6e09-4310-9eac-e51aab8c59d9"
#define CHARACTERISTIC_UUID "71981180-9104-41f3-873b-0bd2145e8d3b"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Relógio conectado!");
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Relógio desconectado.");
    }
};

void setup() {
    Serial.begin(115200);

    BLEDevice::init("ESP32_HelloWorld");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_WRITE  |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();

    pServer->getAdvertising()->start();
    Serial.println("Esperando por conexão do relógio...");
}

void loop() {
    if (deviceConnected) {
        pCharacteristic->setValue("Hello World");
        pCharacteristic->notify();
        delay(2000);
    }

    if (!deviceConnected && oldDeviceConnected) {
        delay(500);
        pServer->startAdvertising();
        Serial.println("Relógio desconectado. Aguardando nova conexão...");
        oldDeviceConnected = deviceConnected;
    }

    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}
