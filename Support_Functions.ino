void initializeStatusLED() {
  status_led.begin();
  status_led.clear();
  status_led.setPixelColor(0, status_led.Color(10, 10, 10));
  status_led.show();
}

void initializeLEDStrip() {
  led_strip.begin();
  led_strip.clear();
  led_strip.show();
}

void initCAN() {
  if (ESP32Can.begin(ESP32Can.convertSpeed(125), CAN_TX, CAN_RX, 10, 10)) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }
}

void checkCANMessages() {
  if (ESP32Can.readFrame(rxFrame, 0)) {
    uint8_t ident = rxFrame.identifier;
    uint8_t CANMessageType = rxFrame.data[0];
    uint8_t parameter = rxFrame.data[1];
    // Serial.println(ident);
    // Serial.println(CANMessageType);
    // Serial.println(parameter);
    if (ident == CAN_IDENTIFIER) {
      if (CANMessageType == CAN_PSU_VOLTAGE) {
        if (parameter > 0 && parameter <= PSU_5V) {
          updatePowerState((PSUState)parameter);
        }
      }
      if (CANMessageType == CAN_TEST_PATTERN) {
        testPatternState = !testPatternState;
      }
    }
  }
}


void sendVoltageCommand(PSUState voltageState) {
  Serial.println("Sending Supply Voltage CAN Message.");
  updatePowerState(voltageState);
  CanFrame txFrame = { 0 };
  txFrame.identifier = CAN_IDENTIFIER;
  txFrame.extd = 0;
  txFrame.data_length_code = 8;
  txFrame.data[0] = CAN_PSU_VOLTAGE;
  txFrame.data[1] = voltageState;
  txFrame.data[2] = CAN_STUFFING_FRAME;
  txFrame.data[3] = CAN_STUFFING_FRAME;
  txFrame.data[4] = CAN_STUFFING_FRAME;
  txFrame.data[5] = CAN_STUFFING_FRAME;
  txFrame.data[6] = CAN_STUFFING_FRAME;
  txFrame.data[7] = CAN_STUFFING_FRAME;
  ESP32Can.writeFrame(txFrame, 0);
}

void updatePowerState(PSUState commandedSupplyState) {
  updateStatusLED(commandedSupplyState);
  psuState = commandedSupplyState;
}

const int maxLength = 50;     // Maximum length of the input string
char inputString[maxLength];  // Array to hold the incoming string
int inputIndex = 0;           // Index to keep track of the current position in the array
bool stringComplete = false;

void initializeSerial() {
  Serial.begin(115200);
  Serial.println("ESP32 Hardware Test Suite v0.1 Starting...");
  printCommands();
}

void printCommands() {
  Serial.println("The test commands are as follows:");
  Serial.println("0 : Turn off LED Power Supply");
  Serial.println("20V : Turn on 20V LED Power (currently not functional)");
  Serial.println("12V : Turn on 12V LED Power");
  Serial.println("5V : Turn on 5V LED Power");
  Serial.println("t : Run/Stop Test Pattern on LED Strip (power must be enabled first)");
}

void serialParser() {
  if (stringComplete) {
    bool validCommand = false;

    if (strcmp(inputString, "0") == 0) {
      sendVoltageCommand(PSU_POWER_OFF);
      Serial.println("Turning off LED Power ");
      validCommand = true;
    }
    if (strcmp(inputString, "20V") == 0) {
      //setVoltage(PSU_20V);
      sendVoltageCommand(PSU_POWER_OFF);
      Serial.println("20V Mode Currently Disabled");
      Serial.println("Turning off LED Power");
      validCommand = true;
    }
    if (strcmp(inputString, "12V") == 0) {
      sendVoltageCommand(PSU_12V);
      Serial.println("12V LED Power Enabling");
      validCommand = true;
    }
    if (strcmp(inputString, "5V") == 0) {
      sendVoltageCommand(PSU_5V);
      Serial.println("5V LED Power Enabling");
      validCommand = true;
    }
    if (strcmp(inputString, "t") == 0) {
      testPatternState = !testPatternState;
      Serial.println("Starting/Stopping Test Pattern");
      validCommand = true;
    }

    if (validCommand == false) {
      Serial.println("Invalid Command");
    }

    memset(inputString, 0, sizeof(inputString));
    inputIndex = 0;
    stringComplete = false;
  }

  while (Serial.available()) {
    char inChar = (char)Serial.read();  // Read the incoming byte

    if (inChar == '\n') {
      inputString[inputIndex] = '\0';  // Null-terminate the string
      stringComplete = true;
    } else {
      if (inputIndex < maxLength - 1) {
        inputString[inputIndex++] = inChar;  // Add the incoming byte to the string
      }
    }
  }
}

void updateStatusLED(PSUState commandedSupplyState) {
  if (commandedSupplyState == PSU_POWER_OFF) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(10, 10, 10));
  }
  if (commandedSupplyState == PSU_20V) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(0, 0, 10));
  }
  if (commandedSupplyState == PSU_12V) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(0, 10, 10));
  }
  if (commandedSupplyState == PSU_5V) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(0, 10, 0));
  }
  status_led.show();
}