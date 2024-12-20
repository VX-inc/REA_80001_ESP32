#include <EEPROM.h>

#define EEPROM_SIZE 1024  // Define the size of the EEPROM partition

// Function to write a string to EEPROM
void writeStringToEEPROM(int address, const String& data) {
  for (int i = 0; i < data.length(); i++) {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.write(address + data.length(), '\0');  // Null-terminate the string
  EEPROM.commit();                              // Save changes to EEPROM
}

// Function to read a string from EEPROM
String readStringFromEEPROM(int address) {
  String result = "";
  char readChar;
  while ((readChar = EEPROM.read(address++)) != '\0') {
    result += readChar;
  }
  return result;
}

// Function to write a uint8_t to EEPROM
void writeUint8ToEEPROM(int address, uint8_t value) {
  EEPROM.write(address, value);
  EEPROM.commit(); // Save changes to EEPROM
}

// Function to read a uint8_t from EEPROM
uint8_t readUint8FromEEPROM(int address) {
  return EEPROM.read(address);
}