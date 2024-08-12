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
  Serial.println("d : Write test DMX Packet");
  Serial.println("r : Toggle DMX Packet Reading");
  Serial.println("c : Send current measurement request");
  Serial.println("z : Send Zero Current Request");
  Serial.println("ad : Auto Detect LED Strip Voltage");
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
    if (strcmp(inputString, "d") == 0) {
      Serial.println("Writing DMX Test Pattern");
      writeDMX();
      validCommand = true;
    }
    if (strcmp(inputString, "r") == 0) {
      toggleDMXRead();
      validCommand = true;
    }
    if (strcmp(inputString, "c") == 0) {
      sendCurrentMeasureCommand();
      validCommand = true;
    }
    if (strcmp(inputString, "z") == 0) {
      sendZeroCurrentCommand();
      validCommand = true;
    }
    if (strcmp(inputString, "ad") == 0) {
      startAutoDetect();
      validCommand = true;
    }


    if (validCommand == false) {
      Serial.println("Invalid Command");
      printCommands();
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