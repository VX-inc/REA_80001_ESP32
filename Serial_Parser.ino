const int maxLength = 50;     // Maximum length of the input string
char inputString[maxLength];  // Array to hold the incoming string
int inputIndex = 0;           // Index to keep track of the current position in the array
bool stringComplete = false;
bool wifiConfigEnabled = false;

void initializeSerial() {
  Serial.begin(115200);
  Serial.println("ESP32 Hardware Test Suite v0.1 Starting...");
  printCommands();
}

void printCommands() {
  Serial.println("---------------------------------------------------------------------------");
  Serial.print("REA_80001_ESP32 Hardware Tester, Version: ");
  Serial.print(FW_VERSION);
  Serial.print(" , Build Date: ");
  Serial.println(__DATE__);
  Serial.println("The test commands are as follows:");
  Serial.println("ip : Print Network Status");
  Serial.println("----- Power Supply -----");
  Serial.println("0 : Power Supply Off");
  Serial.println("p5 : +5V Output");
  Serial.println("n5 : -5V Output");
  Serial.println("p12 : +12V Output");
  Serial.println("n12 : -12V Output");
  Serial.println("p20 : +20V Output");
  Serial.println("n20 : -20V Output");
  Serial.println("----- Utility -----");

  //Serial.println("0 : Turn off LED Power Supply");
  //Serial.println("20V : Turn on 20V LED Power (currently not functional)");
  //Serial.println("12V : Turn on 12V LED Power");
  //Serial.println("5V : Turn on 5V LED Power");
  //Serial.println("5VPO : Turn on 5V LED Power on power up (persists through power cycle)");
  //Serial.println("12VPO : Turn on 12V LED Power on power up (persists through power cycle)");
  //Serial.println("0VPO : Disables turning on LED Power on start (persists through power cycle)");
  Serial.println("t : Run/Stop Test Pattern on LED Strip (power must be enabled first)");
  Serial.println("scan : Run I2C Scanner");
  Serial.println("d : Write test DMX Packet");
  Serial.println("r : Toggle DMX Packet Reading");
  Serial.println("c : Send current measurement request");
  Serial.println("z : Send Zero Current Request");
  Serial.println("ad : Auto Detect LED Strip Voltage");
  Serial.println("p : Print Artnet Data Received.");
  Serial.println("dfp: Print DFP Voltages");
  Serial.println("wifi: Set WiFi Credentials (persists through power cycle)");
  Serial.println("conn: Force WiFi Connection Attempt");
  Serial.println("can : toggle printing received CAN messages");
  Serial.println("verb : increase to the max print verbosity");
  Serial.println("---------------------------------------------------------------------------");
}


void runWifiConfig() {
  static uint8_t state = 0;
  if (state == 0) {
    Serial.print("SSID Received: ");
    Serial.println(inputString);
    writeStringToEEPROM(EEPROM_ID_SSID, inputString);
    Serial.println("Enter Wifi Password.");
  }
  if (state == 1) {
    Serial.print("Password Received: ");
    Serial.println(inputString);
    writeStringToEEPROM(EEPROM_ID_PASSWORD, inputString);
    Serial.println("Config Complete.");
  }

  state++;
  if (state == 2) {
    state = 0;
    wifiConfigEnabled = false;
  }
}

void serialParser() {
  if (stringComplete) {
    Serial.print("Command Received: ");
    Serial.println(inputString);
    bool validCommand = false;

    if (wifiConfigEnabled) {
      runWifiConfig();
    } else {

      if (strcmp(inputString, "ip") == 0) {
        printConnectionStatus();
        validCommand = true;
      }

      if (strcmp(inputString, "0") == 0) {
        sendCombinedCommand(PSU_POWER_OFF, FULL_BRIDGE_OFF);
        Serial.println("Turning off LED Power ");
        validCommand = true;
      }
      if (strcmp(inputString, "p20") == 0) {
        sendCombinedCommand(PSU_20V, FULL_BRIDGE_POSITIVE);
        Serial.println("20V Positive LED Power Enabling");
        validCommand = true;
      }
      if (strcmp(inputString, "n20") == 0) {
        sendCombinedCommand(PSU_20V, FULL_BRIDGE_NEGATIVE);
        Serial.println("20V Negative LED Power Enabling");
        validCommand = true;
      }
      if (strcmp(inputString, "p12") == 0) {
        sendCombinedCommand(PSU_12V, FULL_BRIDGE_POSITIVE);
        Serial.println("12V Positive LED Power Enabling");
        validCommand = true;
      }
      if (strcmp(inputString, "n12") == 0) {
        sendCombinedCommand(PSU_12V, FULL_BRIDGE_NEGATIVE);
        Serial.println("12V Negative LED Power Enabling");
        validCommand = true;
      }
      if (strcmp(inputString, "p5") == 0) {
        sendCombinedCommand(PSU_5V, FULL_BRIDGE_POSITIVE);
        Serial.println("5V Positive LED Power Enabling");
        validCommand = true;
      }
      if (strcmp(inputString, "n5") == 0) {
        sendCombinedCommand(PSU_5V, FULL_BRIDGE_NEGATIVE);
        Serial.println("5V Negative LED Power Enabling");
        validCommand = true;
      }

      if (strcmp(inputString, "0V") == 0) {
        sendVoltageCommand(PSU_POWER_OFF);
        Serial.println("Turning off LED Power ");
        validCommand = true;
      }
      if (strcmp(inputString, "20V") == 0) {
        sendVoltageCommand(PSU_20V);
        //sendVoltageCommand(PSU_POWER_OFF);
        //Serial.println("20V Mode Currently Disabled");
        //Serial.println("Turning off LED Power");
        Serial.println("Turning on 20V LED Power");
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
      if (strcmp(inputString, "12VPO") == 0) {
        setPowerOnState(PSU_12V);
        Serial.println("12V LED Power On Start Enabling");
        validCommand = true;
      }
      if (strcmp(inputString, "5VPO") == 0) {
        setPowerOnState(PSU_5V);
        Serial.println("5V LED Power On Start Enabling");
        validCommand = true;
      }
      if (strcmp(inputString, "0VPO") == 0) {
        setPowerOnState(PSU_POWER_OFF);
        Serial.println("Disabling LED Power On Start");
        validCommand = true;
      }
      if (strcmp(inputString, "po") == 0) {
        sendFullBridgeCommand(FULL_BRIDGE_POSITIVE);
        Serial.println("Sending Full Bridge Positive Command.");
        validCommand = true;
      }
      if (strcmp(inputString, "rv") == 0) {
        sendFullBridgeCommand(FULL_BRIDGE_NEGATIVE);
        Serial.println("Sending Full Bridge Negative Command.");
        validCommand = true;
      }
      if (strcmp(inputString, "off") == 0) {
        sendFullBridgeCommand(FULL_BRIDGE_OFF);
        Serial.println("Sending Full Bridge Off Command.");
        validCommand = true;
      }
      if (strcmp(inputString, "t") == 0) {
        testPatternState = !testPatternState;
        if (testPatternState) {
          Serial.println("Starting Test Pattern");
        } else {
          Serial.println("Stopping Test Pattern");
        }
        validCommand = true;
      }
      if (strcmp(inputString, "d") == 0) {
        Serial.println("Writing DMX Test Pattern");
        testWriteDMX();
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
      if (strcmp(inputString, "p") == 0) {
        enableArtnetPrint();
        validCommand = true;
      }
      if (strcmp(inputString, "dfp") == 0) {
        printCableDFP();
        validCommand = true;
      }
      if (strcmp(inputString, "wifi") == 0) {
        Serial.println("Enter Wifi SSID.");
        wifiConfigEnabled = true;
        validCommand = true;
      }
      if (strcmp(inputString, "conn") == 0) {
        connectWifi();
        validCommand = true;
      }
      if (strcmp(inputString, "can") == 0) {
        toggleCANPrinting();
        validCommand = true;
      }
      if (strcmp(inputString, "verb") == 0) {
        verboseLevel = 2;
        validCommand = true;
      }


      if (validCommand == false) {
        Serial.println("Invalid Command");
        disableArtnetPrint();
        printCommands();
      }
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