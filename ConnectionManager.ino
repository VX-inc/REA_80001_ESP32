bool connectedToEthernet = false;
bool connectedToWifi = false;

uint8_t wifiTimer = 20;
uint8_t connectionTimer = 50;



// Variables to hold the current state and flags for entrance and exit routines
CONNECTION_ENUM currentState = DISCONNECTED;
bool hasEnteredState = false;
bool hasExitedState = true;

void initializeConnectionManager() {
  initializeEthernet();
  initializeArtnet();
  setConnectionState(DISCONNECTED);
}

void connectionManagerSlowHandler() {
  connectedToEthernet = ethernetConnected();
  connectionStateMachine();
}

void connectionManagerFastHandler() {
  if (ethernetConnected()) {
    artnetLoop();
  }
}

// Function to set the state and reset the entrance and exit flags
void setConnectionState(CONNECTION_ENUM newState) {
  if (currentState != newState) {
    if (!hasExitedState) {
      hasExitedState = true;  // Ensure the previous state exit routine is marked as done
    }
    currentState = newState;
    hasEnteredState = false;
    hasExitedState = false;
  }
}


void connectionStateMachine() {
  switch (currentState) {
    case DISCONNECTED:
      if (!hasEnteredState) {
        wifiTimer = 50;
        hasEnteredState = true;
        Serial.println("DISCONNECTED");
      }

      if (wifiTimer == 0) {
        setConnectionState(CONNECTING_TO_WIFI);
      } else {
        wifiTimer--;
      }

      if (ethernetConnected()) {
        setConnectionState(CONNECTED_TO_ETHERNET);
      }

      if (!hasExitedState) {
        hasExitedState = true;
      }
      break;

    case CONNECTED_TO_ETHERNET:
      if (!hasEnteredState) {
        hasEnteredState = true;
        Serial.println("CONNECTED_TO_ETHERNET");
      }

      if (!ethernetConnected()) {
        setConnectionState(DISCONNECTED);
      }

      if (!hasExitedState) {
        hasExitedState = true;
      }
      break;

    case CONNECTING_TO_WIFI:
      if (!hasEnteredState) {
        hasEnteredState = true;
        connectWifi();
        connectionTimer = 50;
        Serial.println("CONNECTING_TO_WIFI");
      }

      if (WiFi.status() == WL_CONNECTED) {
        setConnectionState(CONNECTED_TO_WIFI);
      }

      if (connectionTimer == 0) {
        setConnectionState(DISCONNECTED);
        Serial.println("Wifi Connection Timed Out");
      } else {
        connectionTimer--;
      }

      if (!hasExitedState) {
        hasExitedState = true;
      }
      break;

    case CONNECTED_TO_WIFI:
      if (!hasEnteredState) {
        hasEnteredState = true;
        Serial.println("CONNECTED_TO_WIFI");
      }

      if (WiFi.status() != WL_CONNECTED) {
        setConnectionState(DISCONNECTED);
      }

      if (ethernetConnected()){
        setConnectionState(DISCONNECTED);
        WiFi.disconnect();
      }

      if (!hasExitedState) {
        hasExitedState = true;
      }
      break;

    default:
      Serial.println("Unknown state!");
      break;
  }
}


void printConnectionStatus() {
  if (connectedToEthernet) {
    Serial.println("Ethernet: Connected");
  } else {
    Serial.println("Ethernet: Disconnected");
  }
  if (connectedToWifi) {
    Serial.println("WiFi: Connected");
  } else {
    Serial.println("WiFi: Disconnected");
  }
  Serial.print("ETH Got IP: ");
  Serial.println(WiFi.localIP());
}