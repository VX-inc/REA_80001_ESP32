#include <ArtnetETH.h>
#include <ArtnetWiFi.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "Adafruit_HUSB238.h"
#include <Wire.h>
#include <Arduino.h>

#define EEPROM_ID_SSID 2
#define EEPROM_ID_PASSWORD 40
#define EEPROM_ID_VOLTAGE 0

uint8_t verboseLevel = 2; 
// Define states as an enumeration for clarity
typedef enum {
  DISCONNECTED,
  CONNECTED_TO_ETHERNET,
  CONNECTING_TO_WIFI,
  CONNECTED_TO_WIFI
} CONNECTION_ENUM;

enum PSUState {
  PSU_POWER_OFF = 1,
  PSU_20V = 2,
  PSU_12V = 3,
  PSU_5V = 4
};

enum PSUStatus {
  PSU_OK = 0,
  PSU_OVER_CURRENT = 1
};

enum CANDataType {
  CAN_PSU_VOLTAGE = 1,
  CAN_TEST_PATTERN = 2,
  CAN_CURRENT_REQUEST = 3,
  CAN_CURRENT_ZERO_REQUEST = 4,
  CAN_CURRENT_DATA = 5,
  CAN_PSU_STATUS = 6,
  CAN_PSU_TEST_VOLTAGE = 7,
  CAN_PING = 8
};

void setup() {
  initializeStatusLED();
  initializeLEDStrip();
  initializeSerial();
  initializeDMX();
  initCAN();
  initializeConnectionManager();
  initializeEEPROM();
}

void loop() {
  slottedLoop();
}

//Functions that run once every 100ms
void Slot_100ms() {
  psuAutoStart();
  ledHandler();
  LEDStrip100msHandler();
  connectionManagerSlowHandler();
  //checkCableOrientation();
  CANConnectionHandler();
}

//Functions that run once every 10ms
void Slot_10ms() {
  checkCANMessages();
  serialParser();
  LEDStrip10msHandler();
}

//Functions that run once every loop (the fastest possible)
void Slot_EveryLoop() {
  LEDStripHandler();
  DMXLoop();
  artnetLoop();
}
