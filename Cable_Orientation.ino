void printCableDFP(void) {
  Serial.print("CC1 DFP: ");
  Serial.println(averageAnalogPinA0());
  Serial.print("CC2 DFP: ");
  Serial.println(averageAnalogPinA1());
}


float averageAnalogPinA0(void) {
  uint32_t analogAverage = 0;
  for (int i = 0; i < 100; i++) {
    analogAverage += analogRead(A0);
  }
  return analogAverage / 100.0;
}

float averageAnalogPinA1(void) {
  uint32_t analogAverage = 0;
  for (int i = 0; i < 100; i++) {
    analogAverage += analogRead(A1);
  }
  return analogAverage / 100.0;
}

void checkCableOrientation() {
  static uint8_t count = 0;
  if (count > 10) {
    float CC1 = averageAnalogPinA0();
    float CC2 = averageAnalogPinA1();

    if (CC1 < 1000) {
      updateCableFlipStatusLED(1);
    } else {
      if (CC2 < 1000) {
        updateCableFlipStatusLED(2);
      } else {
        updateCableFlipStatusLED(0);
      }
    }

    count = 0;
  }
  count++;
}
