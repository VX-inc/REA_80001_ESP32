

PSUState psuState = PSU_POWER_OFF;

void sendVoltageCommand(PSUState voltageState) {
  //Serial.println("Sending Supply Voltage CAN Message.");
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

