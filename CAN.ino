#include <ESP32-TWAI-CAN.hpp>

#define CAN_TX 5
#define CAN_RX 4

#define CAN_STUFFING_FRAME 0xAA
#define CAN_IDENTIFIER 0x0A

CanFrame rxFrame;

static uint8_t canTimeout = 0;


void initCAN() {
  if (ESP32Can.begin(ESP32Can.convertSpeed(125), CAN_TX, CAN_RX, 10, 10)) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }
}

static bool printCAN = false;

void toggleCANPrinting() {
  printCAN = !printCAN;
  if (printCAN) {
    Serial.println("Printing received CAN messages: Enabled");
  } else {
    Serial.println("Printing received CAN messages: Disabled");
  }
}

void printCANMessage(uint8_t ident, uint8_t CANMessageType, uint8_t parameter) {
  if (printCAN) {
    Serial.println(ident);
    Serial.println(CANMessageType);
    Serial.println(parameter);
  }
}

void checkCANMessages() {
  if (ESP32Can.readFrame(rxFrame, 0)) {
    uint8_t ident = rxFrame.identifier;
    uint8_t CANMessageType = rxFrame.data[0];
    uint8_t parameter = rxFrame.data[1];
    printCANMessage(ident, CANMessageType, parameter);
    if (ident == CAN_IDENTIFIER) {
      if (CANMessageType == CAN_PSU_VOLTAGE) {
        if (parameter > 0 && parameter <= PSU_5V) {
          updatePowerState((PSUState)parameter);
        }
      }
      if (CANMessageType == CAN_TEST_PATTERN) {
        toggleTestPattern();
      }
      if (CANMessageType == CAN_CURRENT_DATA) {
        receivedCurrentValue(rxFrame.data[1], rxFrame.data[2]);
      }
      if (CANMessageType == CAN_PSU_STATUS) {
        receivedPSUStatus((PSUState)rxFrame.data[1], (PSUStatus)rxFrame.data[2]);
      }
      if (CANMessageType == CAN_POLARITY_CHECK_DATA) {
        receivedPolarityStatus((PolarityDetectType)rxFrame.data[1]);
      }
      if (CANMessageType == CAN_PING) {
        canTimeout = 10;
      }
    }
  }
}

void CANConnectionHandler() {
  if (canTimeout != 0) {
    canTimeout--;
  }
}

bool CANDeviceConnected() {
  if (canTimeout != 0) {
    return true;
  } else {
    return false;
  }
}

void receivedCurrentValue(uint8_t highByte, uint8_t lowByte) {
  uint16_t reassembledValue = (highByte << 8) | lowByte;
  float currentValue = ((float)reassembledValue) / 1000.0;
  if (verboseLevel >= 2) Serial.print("Current Received: ");
  if (verboseLevel >= 2) Serial.println(currentValue, 3);
  receivedCurrentMeasurement(currentValue);
}