#include <SparkFunDMX.h>

#define PIN_DMX_EN1 20
#define PIN_DMX_RX1 19
#define PIN_DMX_TX1 18

SparkFunDMX dmx;
HardwareSerial dmxSerial(1);

uint16_t numChannels = 511;

bool DMXReadActive = false;

void initializeDMX() {
  dmxSerial.begin(DMX_BAUD, DMX_FORMAT, PIN_DMX_RX1, PIN_DMX_TX1);
  dmx.begin(dmxSerial, PIN_DMX_EN1, numChannels);
}

void testWriteDMX() {
  if (!DMXReadActive) {
    dmx.setComDir(DMX_WRITE_DIR);
    DMXReadActive = false;
  }
  dmx.writeByte(55, 1);
  dmx.update();

  Serial.print("DMX: sent value to channel 1: ");
  Serial.println(55);
}

void writeDMX(uint8_t data, uint16_t channel) {
  if (!DMXReadActive) {
    dmx.setComDir(DMX_WRITE_DIR);
    DMXReadActive = false;
  }
  dmx.writeByte(data, channel + 1);
}

void readDMX() {
  dmx.setComDir(DMX_READ_DIR);
  DMXReadActive = true;

  dmx.update();
  if (dmx.dataAvailable() == true) {
    uint8_t data = dmx.readByte(1);
    Serial.print("DMX: read value from channel 1: ");
    Serial.println(data);
  }
}

void DMXLoop() {
  if (DMXReadActive) {
    readDMX();
  }
}

void updateDMX() {
  dmx.update();
}

void toggleDMXRead() {
  DMXReadActive = !DMXReadActive;
  if (DMXReadActive) {
    Serial.println("Reading DMX Enabled, run this command again to disable.");
  } else {
    Serial.println("Reading DMX Disabled");
  }
}