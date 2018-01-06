/*
  Settings.h - Sensor Library for the Infinitag System.
  Created by Jani Taxidis & Tobias Stewen & Florian Kleene.
  Info: www.infinitag.io

  All files are published under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
  License: https://creativecommons.org/licenses/by-nc-sa/4.0/
*/
#define ledPin 4
#define ledAmount 4
#define EEPROM_I2C_ADDRESS 0x00
byte gI2cAddress;
int irReceivePin = 7;

// Player
byte playerTeamId = 1;
byte playerId = 1;
bool playerAlive = true;

byte lastShot[4] = {0, 0, 0, 0};

// Animations
byte animatePattern[7][20][4] = {
  {
    {0, 0, 0, 0},
  },
  {
    {100, 100, 100, 100},
  },
  {
    {5, 5, 5, 5},
  },
  {
    {100, 0, 1, 15},
    {15, 100, 0, 1},
    {1, 15, 100, 0},
    {0, 1, 15, 100}
  },
  {
    {100, 0, 0, 0},
    {0, 100, 0, 0},
    {0, 0, 100, 0},
    {0, 0, 0, 100},
    {0, 0, 100, 0},
    {0, 100, 0, 0}
  },
  {
    {100, 100, 0, 0},
    {0, 0, 0, 0},
    {100, 100, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 100, 100},
    {0, 0, 0, 0},
    {0, 0, 100, 100},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {100, 100, 100, 100},
    {90, 90, 90, 90},
    {80, 80, 80, 80},
    {70, 70, 70, 70},
    {60, 60, 60, 60},
    {50, 50, 50, 50},
    {40, 40, 40, 40},
    {30, 30, 30, 30},
    {20, 20, 20, 20},
    {10, 10, 10, 10},
    {1, 1, 1, 1},
    {10, 10, 10, 10},
    {20, 20, 20, 20},
    {30, 30, 30, 30},
    {40, 40, 40, 40},
    {50, 50, 50, 50},
    {60, 60, 60, 60},
    {70, 70, 70, 70},
    {80, 80, 80, 80},
    {90, 90, 90, 90}
  }
};
byte animateSteps[7] = {1, 1, 1, 4, 6, 10, 20};
byte animateCurrentStep = 0;
unsigned long animateNextStep = 0;
byte animateCurrentAnimation = 0;
unsigned int animateTime = 100;
uint8_t animateColor[4] = {0, 0 , 0, 0};
