
ArtnetWiFiReceiver artnet_wifi;
ArtnetReceiver artnet_eth;
uint16_t universe1 = 1;  // 0 - 32767
uint8_t net = 0;         // 0 - 127
uint8_t subnet = 0;      // 0 - 15
uint8_t universe2 = 2;   // 0 - 15

#define MAX_CHANNEL 511

static bool artnetPrintEnable = false;
static bool artnetWiFiEnabled = false;
static bool artnetETHEnabled = false;

void initializeArtnet() {
  // artnet_eth.begin();
  // artnet_eth.subscribeArtDmxUniverse(net, subnet, universe1, callback_universe1);
  // artnet_eth.subscribeArtDmxUniverse(net, subnet, universe2, callback_universe2);
  // artnet_wifi.begin();
  // artnet_wifi.subscribeArtDmxUniverse(net, subnet, universe1, callback_universe1);
  // artnet_wifi.subscribeArtDmxUniverse(net, subnet, universe2, callback_universe2);
}

void initializeArtnetETH() {
  artnet_eth.begin();
  artnet_eth.subscribeArtDmxUniverse(net, subnet, universe1, callback_universe1);
  artnet_eth.subscribeArtDmxUniverse(net, subnet, universe2, callback_universe2);
  artnetETHEnabled = true;
}

void disableArtnetETH() {
  artnetETHEnabled = false;
}

void initializeArtnetWiFi() {
  artnet_wifi.begin();
  artnet_wifi.subscribeArtDmxUniverse(net, subnet, universe1, callback_universe1);
  artnet_wifi.subscribeArtDmxUniverse(net, subnet, universe2, callback_universe2);
  artnetWiFiEnabled = true;
}

void disableArtnetWiFi() {
  artnetWiFiEnabled = false;
}


void callback_universe1(const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote) {
  printArtnetData(data, size, universe1, metadata, remote);
  for (int channel = 0; channel < size; channel++) {
    writeDMX(data[channel], channel);
  }
  updateDMX();
}

void callback_universe2(const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote) {
  printArtnetData(data, size, universe2, metadata, remote);
  outputLEDArtnetData(data, size);
}

void enableArtnetPrint() {
  artnetPrintEnable = true;
}

void disableArtnetPrint() {
  artnetPrintEnable = false;
}

void printArtnetData(const uint8_t *data, uint16_t size, uint8_t universe, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote) {
  if (artnetPrintEnable) {
    Serial.print("lambda : artnet data from ");
    Serial.print(remote.ip);
    Serial.print(":");
    Serial.print(remote.port);
    Serial.print(", universe = ");
    Serial.print(universe);
    Serial.print(", size = ");
    Serial.print(size);
    Serial.print(") :");
    for (size_t i = 0; i < size; ++i) {
      Serial.print(data[i]);
      Serial.print(",");
    }
    Serial.println();
  }
}

void artnetLoop() {
  if (artnetETHEnabled) {
    artnet_eth.parse();
  }
  if (artnetWiFiEnabled) {
    artnet_wifi.parse();
  }
}