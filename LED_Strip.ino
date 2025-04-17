#include <Adafruit_NeoPixel.h>

#define DELAY_BETWEEN_PATTERNS 5000

#define PIN_LED_DO 21
#define PIN_LED_CLK 22

#define LED_STRIP_LED_COUNT 90

Adafruit_NeoPixel led_strip(LED_STRIP_LED_COUNT, PIN_LED_DO, NEO_RGB + NEO_KHZ800);

unsigned long patternInterval = 20;               // time between steps in the pattern
unsigned long lastUpdate = 0;                     // for millis() when last update occoured
unsigned long intervals[] = { 20, 20, 50, 100 };  // speed for each pattern
unsigned long patternTimer = 0;

bool autoDetectStart = false;
enum AutoDetectStateType {
  AD_IDLE = 0,
  AD_START = 1,
  AD_ENABLE_POWER = 2,
  AD_TESTSHORT = 3,
  AD_ZERO_CURRENT = 4,
  AD_COMMAND_STRIP = 5,
  AD_MEASURE_CURRENT = 6,
  AD_COMPLETE = 7,
  AD_FAIL = 8
};
AutoDetectStateType autoDetectState = AD_IDLE;

bool testPatternState = false;
bool clearLEDStripActive = false;
bool testLEDStripActive = false;


void toggleTestPattern() {
  testPatternState = !testPatternState;
}

void initializeLEDStrip() {
  led_strip.begin();
  led_strip.clear();
  led_strip.show();
}

void LEDStrip100msHandler() {
  AutoVoltageDetect();
}

void LEDStrip10msHandler() {
  if (clearLEDStripActive) {
    clearLEDStrip();
  }
  if (testLEDStripActive) {
    testLEDStripCurrent();
  }
}

void LEDStripHandler() {
  if (testPatternState) {
    runTestPattern();
  }
}

void startAutoDetect() {
  autoDetectStart = true;
}

bool PSUStatusReceived = false;
PSUState psuReceivedState = PSU_POWER_OFF;
PSUStatus psuReceivedStatus = PSU_OK;

void receivedPSUStatus(PSUState state, PSUStatus status) {
  if (verboseLevel >= 1) {
    Serial.print("Received PSU Status: ");
    Serial.print("State: ");
    Serial.print(state);
    Serial.print(" Status: ");
    Serial.println(status);
  }
  psuReceivedState = state;
  psuReceivedStatus = status;
  PSUStatusReceived = true;
}

float currentValue = 0;
bool receivedCurrentUpdate = false;
void receivedCurrentMeasurement(float current) {
  currentValue = current;
  receivedCurrentUpdate = true;
}

bool currentRequestReturned() {
  if (receivedCurrentUpdate) {
    receivedCurrentUpdate = false;
    return true;
  } else {
    return false;
  }
}

float getCurrentValue() {
  return currentValue;
}

//Disable Supply, check for response
//Enable 5V
//Command off strip
//Zero Current measurement
//Wait 100ms
//Command on strip
//Measure current
//Check if a 5V strip
//If not enable 12V
//Check if above 500mA, error if true
//Command off strip
//Zero current measurement
//Wait 100ms
//Command on strip
//Measure current
//Check if a 12V strip
#define AD_TIMEOUT 40
void AutoVoltageDetect() {
  static uint8_t loopDelay = 0;
  static uint8_t timeout = AD_TIMEOUT;
  static PSUState voltageAttempt = PSU_5V;
  if (autoDetectState != AD_IDLE && autoDetectState != AD_FAIL) {
    if (timeout == 0) {
      autoDetectState = AD_FAIL;
      Serial.println("LED Strip Voltage Auto Detect Timed Out");
    } else {
      timeout--;
    }
  }

  if (loopDelay == 0) {
    switch (autoDetectState) {
      case AD_IDLE:
        if (autoDetectStart) {
          autoDetectState = AD_START;
          sendVoltageCommand(PSU_POWER_OFF);
          timeout = AD_TIMEOUT;
          autoDetectStart = false;
          voltageAttempt = PSU_5V;
          loopDelay = 1;
          if (verboseLevel >= 1) Serial.println("State: AD_START");
        }
        break;

      case AD_START:
        if (PSUStatusReceived) {
          autoDetectState = AD_ENABLE_POWER;
          sendVoltageCommand(voltageAttempt);
          clearLEDStripActive = true;
          if (verboseLevel >= 2) Serial.println("State: AD_ENABLE_POWER");
          if (voltageAttempt == PSU_5V) {
            if (verboseLevel >= 2) Serial.println("Turing on 5V supply");
          }
          if (voltageAttempt == PSU_12V) {
            if (verboseLevel >= 2) Serial.println("Turing on 12V supply");
          }
        }
        break;

      case AD_ENABLE_POWER:
        if (PSUStatusReceived) {
          autoDetectState = AD_TESTSHORT;
          if (verboseLevel >= 2) Serial.println("State: AD_TESTSHORT");
          loopDelay = 5;
        }
        break;

      case AD_TESTSHORT:
        clearLEDStripActive = false;
        sendZeroCurrentCommand();
        autoDetectState = AD_ZERO_CURRENT;
        if (verboseLevel >= 2) Serial.println("State: AD_ZERO_CURRENT");
        if (verboseLevel >= 2) Serial.println("Zeroing current measurement");
        break;

      case AD_ZERO_CURRENT:
        if (currentRequestReturned()) {
          autoDetectState = AD_COMMAND_STRIP;
          if (verboseLevel >= 2) Serial.println("State: AD_COMMAND_STRIP");
          loopDelay = 5;
          testLEDStripActive = true;
          if (verboseLevel >= 2) Serial.println("Measuring current");
        }
        break;

      case AD_COMMAND_STRIP:
        sendCurrentMeasureCommand();
        autoDetectState = AD_MEASURE_CURRENT;
        if (verboseLevel >= 2) Serial.println("State: AD_MEASURE_CURRENT");
        break;

      case AD_MEASURE_CURRENT:
        if (currentRequestReturned()) {
          if (getCurrentValue() > 0.1) {
            testLEDStripActive = false;
            clearLEDStripActive = true;
            autoDetectState = AD_COMPLETE;
            Serial.println("State: AD_COMPLETE");
          } else {
            if (voltageAttempt == PSU_5V) {
              if (verboseLevel >= 1) Serial.println("Strip not detected at 5V");
              //voltageAttempt = PSU_12V;
              testLEDStripActive = false;
              autoDetectState = AD_START;
              sendVoltageCommand(PSU_POWER_OFF);
            } else {
              autoDetectState = AD_FAIL;
              testLEDStripActive = false;
              Serial.println("No LED Strip Detected");
            }
          }
        }
        break;

      case AD_COMPLETE:
        if (voltageAttempt == PSU_5V) {
          Serial.println("5V Strip Detected");
        }
        if (voltageAttempt == PSU_12V) {
          Serial.println("12V Strip Detected");
        }
        clearLEDStripActive = false;
        autoDetectState = AD_IDLE;
        break;

      case AD_FAIL:
        if (verboseLevel >= 2) Serial.println("State: AD_FAIL");
        autoDetectState = AD_IDLE;
        sendVoltageCommand(PSU_POWER_OFF);
        testLEDStripActive = false;
        clearLEDStripActive = false;
        break;

      default:
        if (verboseLevel >= 2) Serial.println("Unknown State");
        break;
    }
  } else {
    loopDelay--;
  }
}

void sendCurrentMeasureCommand() {
  if (verboseLevel >= 2) Serial.println("Sending Current Measure Command");
  CanFrame txFrame = { 0 };
  txFrame.identifier = CAN_IDENTIFIER;
  txFrame.extd = 0;
  txFrame.data_length_code = 8;
  txFrame.data[0] = CAN_CURRENT_REQUEST;
  txFrame.data[1] = CAN_STUFFING_FRAME;
  txFrame.data[2] = CAN_STUFFING_FRAME;
  txFrame.data[3] = CAN_STUFFING_FRAME;
  txFrame.data[4] = CAN_STUFFING_FRAME;
  txFrame.data[5] = CAN_STUFFING_FRAME;
  txFrame.data[6] = CAN_STUFFING_FRAME;
  txFrame.data[7] = CAN_STUFFING_FRAME;
  ESP32Can.writeFrame(txFrame, 0);
}

void sendZeroCurrentCommand() {
  if (verboseLevel >= 2) Serial.println("Sending Zero Current Command");
  CanFrame txFrame = { 0 };
  txFrame.identifier = CAN_IDENTIFIER;
  txFrame.extd = 0;
  txFrame.data_length_code = 8;
  txFrame.data[0] = CAN_CURRENT_ZERO_REQUEST;
  txFrame.data[1] = CAN_STUFFING_FRAME;
  txFrame.data[2] = CAN_STUFFING_FRAME;
  txFrame.data[3] = CAN_STUFFING_FRAME;
  txFrame.data[4] = CAN_STUFFING_FRAME;
  txFrame.data[5] = CAN_STUFFING_FRAME;
  txFrame.data[6] = CAN_STUFFING_FRAME;
  txFrame.data[7] = CAN_STUFFING_FRAME;
  ESP32Can.writeFrame(txFrame, 0);
}


void clearLEDStrip() {
  led_strip.clear();
  led_strip.show();
}

void outputLEDArtnetData(const uint8_t *data, uint16_t size) {
  for (int i = 0; i < size / 3; i++) {
    int led = i;
    if (led < 170) {
      led_strip.setPixelColor(led, led_strip.Color(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]));
    }
  }
  led_strip.show();
}

void testLEDStripCurrent() {
  for (uint8_t i = 0; i <= 10; i++) {
    led_strip.setPixelColor(i, led_strip.Color(200, 200, 200));
  }
  led_strip.show();
}

void runTestPattern() {
  static int pattern = 0, lastReading;

  if (millis() > patternTimer) {
    pattern++;                             // change pattern number
    if (pattern > 3) pattern = 0;          // wrap round if too big
    patternInterval = intervals[pattern];  // set speed for this pattern
    wipe();                                // clear out the buffer
    delay(50);                             // debounce delay
    patternTimer = millis() + DELAY_BETWEEN_PATTERNS;
  }

  if (millis() - lastUpdate > patternInterval) updatePattern(pattern);
}

void updatePattern(int pat) {  // call the pattern currently being created
  switch (pat) {
    case 0:
      rainbow();
      break;
    case 1:
      rainbowCycle();
      break;
    case 2:
      theaterChaseRainbow();
      break;
    case 3:
      colorWipe(led_strip.Color(255, 0, 0));  // red
      break;
  }
}

void rainbow() {  // modified from Adafruit example to make it a state machine
  static uint16_t j = 0;
  for (int i = 0; i < led_strip.numPixels(); i++) {
    led_strip.setPixelColor(i, Wheel((i + j) & 255));
  }
  led_strip.show();
  j++;
  if (j >= 256) j = 0;
  lastUpdate = millis();  // time for next change to the display
}
void rainbowCycle() {  // modified from Adafruit example to make it a state machine
  static uint16_t j = 0;
  for (int i = 0; i < led_strip.numPixels(); i++) {
    led_strip.setPixelColor(i, Wheel(((i * 256 / led_strip.numPixels()) + j) & 255));
  }
  led_strip.show();
  j++;
  if (j >= 256 * 5) j = 0;
  lastUpdate = millis();  // time for next change to the display
}

void theaterChaseRainbow() {  // modified from Adafruit example to make it a state machine
  static int j = 0, q = 0;
  static boolean on = true;
  if (on) {
    for (int i = 0; i < led_strip.numPixels(); i = i + 3) {
      led_strip.setPixelColor(i + q, Wheel((i + j) % 255));  //turn every third pixel on
    }
  } else {
    for (int i = 0; i < led_strip.numPixels(); i = i + 3) {
      led_strip.setPixelColor(i + q, 0);  //turn every third pixel off
    }
  }
  on = !on;          // toggel pixelse on or off for next time
  led_strip.show();  // display
  q++;               // update the q variable
  if (q >= 3) {      // if it overflows reset it and update the J variable
    q = 0;
    j++;
    if (j >= 256) j = 0;
  }
  lastUpdate = millis();  // time for next change to the display
}

void colorWipe(uint32_t c) {  // modified from Adafruit example to make it a state machine
  static int i = 0;
  led_strip.setPixelColor(i, c);
  led_strip.show();
  i++;
  if (i >= led_strip.numPixels()) {
    i = 0;
    wipe();  // blank out led_strip
  }
  lastUpdate = millis();  // time for next change to the display
}


void wipe() {  // clear all LEDs
  for (int i = 0; i < led_strip.numPixels(); i++) {
    led_strip.setPixelColor(i, led_strip.Color(0, 0, 0));
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return led_strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return led_strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return led_strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
