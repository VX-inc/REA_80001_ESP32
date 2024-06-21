#define DELAY_BETWEEN_PATTERNS 5000

unsigned long patternInterval = 20;               // time between steps in the pattern
unsigned long lastUpdate = 0;                     // for millis() when last update occoured
unsigned long intervals[] = { 20, 20, 50, 100 };  // speed for each pattern
unsigned long patternTimer = 0;

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
  on = !on;      // toggel pixelse on or off for next time
  led_strip.show();  // display
  q++;           // update the q variable
  if (q >= 3) {  // if it overflows reset it and update the J variable
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
