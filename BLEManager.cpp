#include "BLEManager.h"
#include "config.h"

BLEManager* BLEManager::instance = nullptr;

BLEManager::BLEManager() {
    instance = this;
}

bool BLEManager::initialize(const BLEConfig& config) {
    this->cfg = config;

    // 构造 service & characteristic
    service = BLEService(cfg.serviceUuid.c_str());

    sensorDataChar = BLECharacteristic(
        cfg.sensorUuid.c_str(),
        BLERead | BLENotify,
        cfg.mtuSize
    );

    commandChar = BLECharacteristic(
        cfg.commandUuid.c_str(),
        BLEWrite | BLEWriteWithoutResponse,
        cfg.mtuSize
    );

    if (!BLE.begin()) {
        Serial.println("❌ BLE not working properly!");
        return false;
    }

    BLE.stopAdvertise();

    BLE.setLocalName(cfg.deviceName.c_str());
    BLE.setAdvertisedService(service);

    service.addCharacteristic(sensorDataChar);
    service.addCharacteristic(commandChar);

    BLE.addService(service);
    commandChar.setEventHandler(BLEWritten, onCommandWrittenStatic);

    BLE.advertise();
    Serial.println("✅ BLE started. Waiting for connection...");
    return true;
}



void BLEManager::update() {
  static bool wasConnected = false;
  bool isConnected = checkConnection();

  if (isConnected) {
    if (!wasConnected) {
      Serial.println("🟢 BLE connected");
      wasConnected = true;
    }
    BLE.poll();
  } else {
    if (wasConnected) {
      Serial.println("🔴 BLE disconnected");
      wasConnected = false;
    }
  }
}

void BLEManager::sendCommand(const String& cmdName, const String& payload) {
  if (!connected) return;

  unsigned long ts = millis();
  String msg = cmdName + ":" + String(millis()) + ":" + payload;

  if (msg.length() >= cfg.mtuSize) {
      Serial.println("⚠️ Message too long, skipped");
      return;
  }

 
  sensorDataChar.writeValue(msg.c_str());  // ✅ 实时发送
  Serial.print("📤 Sent: ");
  Serial.println(msg);
}

void BLEManager::onCommandWrittenStatic(BLEDevice central, BLECharacteristic characteristic) {
  if (instance) instance->onCommandWritten(central, characteristic);
}

bool BLEManager::checkConnection() {
  if (central && central.connected()) {
    return true;
  }

  // 尝试获取新的 central
  central = BLE.central();
  if (!central) {
    if (connected) {
      connected = false;
      Serial.println("🔌 BLE disconnected");
    }
    return false;
  }

  // 有新设备连接上了
  connected = true;
  Serial.print("📶 BLE connected: ");
  Serial.println(central.address());
  
  return true;
}
