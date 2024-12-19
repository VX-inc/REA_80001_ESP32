

//Wifi settings
char ssid[] = "Reactance";  //Change these lines to an existing SSID and Password if you're trying to connect to an existing network
char password[] = "REA80001";


bool wifiConnected = false;

boolean connectWifi(void)  //Sets our ESP32 device up as an access point
{
  boolean state = true;

  state = WiFi.begin(ssid, password);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Wifi Connected!");
  Serial.println(WiFi.localIP());

  return state;
}

void disconnectWifi(void){
  WiFi.disconnect();
}
