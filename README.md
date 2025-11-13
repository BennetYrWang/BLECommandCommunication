# BLEManager Usage Guide

This project provides a lightweight BLE manager designed to simplify BLE communication on Arduino/ESP32.
It handles BLE initialization, service/characteristic setup, connection tracking, message formatting, and sending commands with an automatic timestamp system.

You only need minimal code to get BLE running.

------------------------------------------------------------

## Features

- Easy BLE initialization using a single config structure
- Automatic service and characteristic creation
- Automatic connection state tracking
- Automatic timestamp injection when sending commands
- Unified message protocol:

    commandName:timestamp:payload

- Command write events are supported — you can attach your own handler logic inside the manager (handler not included here; add your own implementation).

------------------------------------------------------------

## 1. BLEConfig Setup

Create and fill a BLEConfig structure:

    BLEConfig cfg;
    cfg.deviceName        = "Arduino Nano ESP32 BLE";
    cfg.mtuSize           = 128;
    cfg.sendIntervalMs    = 10;
    cfg.serviceUuid       = "12345678-1234-5678-1234-56789abcdef0";
    cfg.sensorUuid        = "12345678-1234-5678-1234-56789abcdef1";
    cfg.commandUuid       = "12345678-1234-5678-1234-56789abcdef2";
    cfg.autoPingInterval  = 3000;

### Config Fields

| Field             | Description                                      |
|-------------------|--------------------------------------------------|
| deviceName        | BLE device name shown during scanning            |
| mtuSize           | Desired MTU size                                 |
| sendIntervalMs    | Minimum delay between message sends              |
| serviceUuid       | Primary BLE service UUID                         |
| sensorUuid        | Characteristic used for sending data             |
| commandUuid       | Characteristic used for receiving commands       |
| autoPingInterval  | Automatic heartbeat interval (ms)                |

------------------------------------------------------------

## 2. Initialize BLEManager

Declare the manager:

    BLEManager ble;

Initialize in setup():

    ble.initialize(cfg);

This automatically:

- Starts BLE
- Creates the service and characteristics
- Registers write callbacks
- Begins advertising
- Handles MTU negotiation

------------------------------------------------------------

## 3. Call update() in the Main Loop

update() maintains BLE state and processes events.

    void loop() {
        ble.update();
    }

------------------------------------------------------------

## 4. Sending Commands

Use sendCommand(name, payload):

    ble.sendCommand("ping", "");

Messages follow the protocol:

    commandName:timestamp:payload

Example:

    HIT:1731812281:LeftSensor

Timestamp is generated automatically.

------------------------------------------------------------

## 5. Connection Check

    if (ble.checkConnection()) {
        ble.sendCommand("ping", "");
    }

------------------------------------------------------------

## 6. Command Handling (Optional)

The manager supports command-write events.

You can add your own handler logic inside:

    onCommandWritten()

Handler code is not included here — implement your own parsing or routing.

------------------------------------------------------------

## 7. Minimal Working Example

    #include <ArduinoBLE.h>
    #include "BLEManager.h"
    #include "BLEConfig.h"

    BLEManager ble;
    BLEConfig cfg;

    void setup() {
        Serial.begin(115200);

        cfg.deviceName        = "Arduino Nano ESP32 BLE";
        cfg.mtuSize           = 128;
        cfg.sendIntervalMs    = 10;
        cfg.serviceUuid       = "12345678-1234-5678-1234-56789abcdef0";
        cfg.sensorUuid        = "12345678-1234-5678-1234-56789abcdef1";
        cfg.commandUuid       = "12345678-1234-5678-1234-56789abcdef2";
        cfg.autoPingInterval  = 3000;

        ble.initialize(cfg);
    }

    void loop() {
        ble.update();

        if (ble.checkConnection()) {
            ble.sendCommand("ping", "");
        }

        delay(1000);
    }

------------------------------------------------------------

## Summary

1. Create a BLEConfig
2. Call ble.initialize(cfg)
3. Call ble.update() in your loop
4. Send messages using sendCommand(name, payload)
5. Add optional command handlers if needed

This keeps BLE usage clean, simple, and modular.
