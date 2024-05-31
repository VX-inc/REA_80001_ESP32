#include <ESP32-TWAI-CAN.hpp>
#include <Adafruit_NeoPixel.h>

#define LED_STATUS_PIN 1
#define LED_STATUS_ADDRESS 0

#define CAN_TX 5
#define CAN_RX 4

#define PIN_LED_DO 21
#define PIN_LED_CLK 22

#define PIN_DMX_EN1 20
#define PIN_DMX_RX1 19
#define PIN_DMX_TX1 18

#define CAN_STUFFING_FRAME 0xAA
#define CAN_IDENTIFIER 0x0A

#define LED_STRIP_LED_COUNT 60

CanFrame rxFrame;

enum PSUState {
  PSU_POWER_OFF = 1,
  PSU_20V = 2,
  PSU_12V = 3,
  PSU_5V = 4
};

enum CANDataType {
  CAN_PSU_VOLTAGE = 1,
  CAN_TEST_PATTERN = 2
};

static bool testPatternState = false;

PSUState psuState = PSU_POWER_OFF;

Adafruit_NeoPixel status_led(1, LED_STATUS_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel led_strip(LED_STRIP_LED_COUNT, PIN_LED_DO, NEO_RGB + NEO_KHZ800);

void setup() {
  delay(2000);
  initializeStatusLED();
  initializeLEDStrip();
  initializeSerial();
  initCAN();
}

void loop() {
  checkCANMessages();
  serialParser();
  if(testPatternState){
    runTestPattern();
  }

}

