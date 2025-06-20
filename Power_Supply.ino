
PolarityDetectType polarityStatusReceived = POLARITY_DETECT_NOT_RUN;
PSUState psuState = PSU_POWER_OFF;

void sendVoltageCommand(PSUState voltageState) {
  //Serial.println("Sending Supply Voltage CAN Message.");
  clearPSUStatusReceived();
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

void sendFullBridgeCommand(FullBridgeType fullBridgeState) {
  CanFrame txFrame = { 0 };
  txFrame.identifier = CAN_IDENTIFIER;
  txFrame.extd = 0;
  txFrame.data_length_code = 8;
  txFrame.data[0] = CAN_SET_FULL_BRIDGE;
  txFrame.data[1] = fullBridgeState;
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

PSUState getPSUStatus(void) {
  return psuState;
}

void psuAutoStart() {
  static uint8_t countdown = 20;
  if (countdown >= 1) {
    if (countdown == 1) {
      PSUState state = (PSUState)readUint8FromEEPROM(EEPROM_ID_VOLTAGE);
      if (state == PSU_POWER_OFF || state == PSU_12V || state == PSU_5V) {
        sendVoltageCommand(state);
        Serial.print("Startup Voltage Set: ");
        Serial.println(state);
      } else {
        Serial.println("Invalid PSU Start up State set in EEPROM");
      }
    }
    countdown--;
  }
}

void setPowerOnState(PSUState state) {
  if (state == PSU_POWER_OFF || state == PSU_12V || state == PSU_5V) {
    writeUint8ToEEPROM(EEPROM_ID_VOLTAGE, state);
    Serial.println("Startup Voltage Configuration Set Successfully!");
  } else {
    Serial.println("Invalid Power Supply State Received.");
  }
}

PolarityDetectType getPolarityDetectState () {
  return polarityStatusReceived;
}

void receivedPolarityStatus(PolarityDetectType polarityStatus) {
  polarityStatusReceived = polarityStatus;
  switch (polarityStatus) {
    case POLARITY_NO_DETECT:
      if (verboseLevel >= 2) Serial.println("No current detected.");
      break;
    case POLARITY_FORWARD:
      if (verboseLevel >= 2) Serial.println("Forward polarity detected.");
      break;
    case POLARITY_REVERSE:
      if (verboseLevel >= 2) Serial.println("Reverse polarity detected.");
      break;
    case POLARITY_SHORTED:
      if (verboseLevel >= 2) Serial.println("Output appears shorted.");
      break;
    default:
      if (verboseLevel >= 2) Serial.println("Unknown polarity state.");
      break;
  }
}

void sendPolarityCheckCommand(PSUState psu_state, PolarityDetectType polarity) {
  polarityStatusReceived = POLARITY_DETECT_NOT_RUN;
  if (verboseLevel >= 2) Serial.println("Sending Polarity Check Command");
  CanFrame txFrame = { 0 };
  txFrame.identifier = CAN_IDENTIFIER;
  txFrame.extd = 0;
  txFrame.data_length_code = 8;
  txFrame.data[0] = CAN_RUN_POLARITY_CHECK;
  txFrame.data[1] = psu_state;
  txFrame.data[2] = polarity;
  txFrame.data[3] = CAN_STUFFING_FRAME;
  txFrame.data[4] = CAN_STUFFING_FRAME;
  txFrame.data[5] = CAN_STUFFING_FRAME;
  txFrame.data[6] = CAN_STUFFING_FRAME;
  txFrame.data[7] = CAN_STUFFING_FRAME;
  ESP32Can.writeFrame(txFrame, 0);
}
