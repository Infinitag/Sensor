/*
  Settings.h - Sensor Library for the Infinitag System.
  Created by Jani Taxidis & Tobias Stewen & Florian Kleene.
  Info: www.infinitag.io

  All files are published under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
  License: https://creativecommons.org/licenses/by-nc-sa/4.0/
*/
#define PIN 3
#define EEPROM_I2C_ADDRESS 0x00
byte gI2cAddress;
int irReceivePin = 7;

// Player
byte playerTeamId = 1;
byte playerId = 1;
bool playerAlive = true;
