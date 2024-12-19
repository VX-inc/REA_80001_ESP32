void slottedLoop() {
  static uint32_t slot_100ms = 0;
  static uint32_t slot_10ms = 0;

  if (millis() >= slot_100ms) {
    Slot_100ms();
    while (millis() >= slot_100ms) {
      slot_100ms += 100;
    }
  }

  if (millis() >= slot_10ms) {
    Slot_10ms();
    while (millis() >= slot_10ms) {
      slot_10ms += 10;
    }
  }

  Slot_EveryLoop();
}

#define LED_STATUS_PIN 15
#define LED_STATUS_ADDRESS 0

Adafruit_NeoPixel status_led(4, LED_STATUS_PIN, NEO_GRB + NEO_KHZ800);

void initializeStatusLED() {
  status_led.begin();
  status_led.clear();
  status_led.setPixelColor(0, status_led.Color(10, 10, 10));
  status_led.show();
}

void refreshStatusLED() {
  updateStatusLED(psuState);
}

void updateStatusLED(PSUState commandedSupplyState) {
  if (commandedSupplyState == PSU_POWER_OFF) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(20, 20, 20));
  }
  if (commandedSupplyState == PSU_20V) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(0, 0, 20));
  }
  if (commandedSupplyState == PSU_12V) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(0, 20, 20));
  }
  if (commandedSupplyState == PSU_5V) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(0, 20, 0));
  }
  status_led.show();
}

void updatePDStatusLED(HUSB238_PDSelection pdStatus) {
  if (pdStatus == PD_SRC_20V) {
    status_led.setPixelColor(LED_PD_STATUS_ADDRESS, status_led.Color(0, 20, 0));
  } else {
    status_led.setPixelColor(LED_PD_STATUS_ADDRESS, status_led.Color(20, 0, 0));
  }
  while (!status_led.canShow()) {}
  status_led.show();
}

void ledHandler(void) {
  updateStatusLED(getPSUStatus());
  updatePDStatusLED(getPDStatus());
}

void updatePowerState(PSUState commandedSupplyState) {
  Serial.print("Power Supply State: ");
  Serial.println(commandedSupplyState);
  powerStateMachineCommand(commandedSupplyState);
  psuState = commandedSupplyState;
}

void refreshStatusLED() {
  updateStatusLED(psuState);
}