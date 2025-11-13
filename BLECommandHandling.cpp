#pragma once
#include "BLEManager.h"


// Declare Handlers

void pingHandler(const String& cmd, long ts, const String& payload);
void pongHandler(const String& cmd, long ts, const String& payload);


  /**
   * @brief 处理 central 写入命令特征的回调。
   *        在此解析收到的文本指令并触发相应逻辑。
   */
void BLEManager::onCommandWritten(BLEDevice central, BLECharacteristic characteristic) {
  const uint8_t* raw = characteristic.value();
  int len = characteristic.valueLength();
  String command = String((const char*)raw, len);

  Serial.print("📩 Received command: ");
  Serial.println(command);

  int sep = command.indexOf(':');
  if (sep == -1) {
    Serial.println("⚠️ Invalid command format");
    return;
  }

  String cmd = command.substring(0, sep);
  cmd.toLowerCase();
  String payload = command.substring(sep + 1);
  long ts = millis();

  if (cmd == "ping") {
    pingHandler(cmd, ts, payload);
    return;
  }
  if (cmd == "pong") {
    pongHandler(cmd, ts, payload);
    return;
  }

  Serial.print("⚠️ Unknown command: ");
  Serial.print(cmd);
  Serial.print(" -> ");
  Serial.println(payload);
}



// Implement Handlers

void pingHandler(const String& cmd, long ts, const String& payload) {
  // ts = 对方的时间戳
  // payload = 可能为空或上一轮的时间戳
  long now = millis();
  long remoteTs = ts;

  // 回复 pong:<now>:<remoteTs>
  BLEManager::getInstance()->sendCommand("PONG", String(remoteTs));

  Serial.print("📤 Reply → PONG:");
}


void pongHandler(const String& cmd, long ts, const String& payload) {
  // 收到 pong，计算 RTT
  long now = millis();
  long remoteSendTs = ts;          // 对方发送 pong 的时间
  long myPingTs = payload.toInt(); // 我最早发送 ping 的时间
  long rtt = now - myPingTs;
  Serial.print("📶 Pong received, RTT = ");
  Serial.print(rtt);
  Serial.println(" ms");
}