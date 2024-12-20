


//Wifi settings
char ssid[] = "Reactance";  //Change these lines to an existing SSID and Password if you're trying to connect to an existing network
char password[] = "REA80001";


bool wifiConnected = false;
bool wifiConnecting = false;

uint16_t connectionTimeout = 0;
uint8_t unsuccessfulConnectionAttempts = 0;

void initializeEEPROM() {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialize EEPROM");
    return;
  }
}

boolean connectWifi(void)  //Sets our ESP32 device up as an access point
{
  boolean state = true;

  //state = WiFi.begin(ssid, password);
  state = WiFi.begin(readStringFromEEPROM(EEPROM_ID_SSID), readStringFromEEPROM(EEPROM_ID_PASSWORD));

  Serial.print("Connecting to: ");
  Serial.println(readStringFromEEPROM(EEPROM_ID_SSID));
  Serial.print("With the Passkey: ");
  Serial.println(readStringFromEEPROM(EEPROM_ID_PASSWORD));

  wifiConnecting = true;
  connectionTimeout = 0;

  return state;
}

void disconnectWifi(void) {
  Serial.println("Disconnecting WiFi");
  WiFi.disconnect();
}


