#include <SparkFunDMX.h>

#define PIN_DMX_EN1 20
#define PIN_DMX_RX1 19
#define PIN_DMX_TX1 18

SparkFunDMX dmx;
HardwareSerial dmxSerial(1);

uint16_t numChannels = 1;

bool DMXReadActive = false;

void initializeDMX() {
  dmxSerial.begin(DMX_BAUD, DMX_FORMAT, PIN_DMX_RX1, PIN_DMX_TX1);
  dmx.begin(dmxSerial, PIN_DMX_EN1, numChannels);
}

void writeDMX() {
  dmx.setComDir(DMX_WRITE_DIR);
  dmx.writeByte(55, 1);
  dmx.update();

  Serial.print("DMX: sent value to channel 1: ");
  Serial.println(55);
}

void readDMX() {
  dmx.setComDir(DMX_READ_DIR);

  dmx.update();
  if (dmx.dataAvailable() == true) {
    uint8_t data = dmx.readByte(1);
    Serial.print("DMX: read value from channel 1: ");
    Serial.println(data);
  }
}

void updateDMX() {
  if (DMXReadActive) {
    readDMX();
  }
}

void toggleDMXRead() {
  DMXReadActive = !DMXReadActive;
  if (DMXReadActive) {
    Serial.println("Reading DMX Enabled, run this command again to disable.");
  } else {
    Serial.println("Reading DMX Disabled");
  }
}