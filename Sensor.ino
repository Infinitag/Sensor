/*
  Sensor.ino - Sensor Library for the Infinitag System.
  Created by Jani Taxidis & Tobias Stewen & Florian Kleene.
  Info: www.infinitag.io

  All files are published under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
  License: https://creativecommons.org/licenses/by-nc-sa/4.0/
*/
// Infinitag Libs
#include <Infinitag_Core.h>

// Vendor Libs
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <EEPROM.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <IRremote.h>
#include <sensor_dhcp.h>

// Settings
#include "Settings.h"

// Infinitag Inits
Infinitag_Core infinitagCore;

// Vendor Inits
Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, PIN, NEO_GRBW + NEO_KHZ800);
decode_results irResults;
IRrecv irRecv(irReceivePin);

// Color
uint32_t sensorColors[] = {strip.Color(0,255,0,0), strip.Color(0,0,255,0), strip.Color(0,255,255,0), strip.Color(255,255,0,0), strip.Color(255,0,255,0),strip.Color(0,255,0,255), strip.Color(0,255,255,255), strip.Color(255,255,0,255), strip.Color(255,0,255,255)};
uint32_t sensorColor = 0;

void setup() {  
  Serial.begin(57600);
  
  // LEDs
  strip.begin();
  waitLed(15);

  //Wait For Master to Boot 
  //gI2cAddress = 0x22; //queryI2CAddressFromMaster();//(EEPROM_I2C_ADDRESS, false);
  gI2cAddress = queryI2CAddressFromMaster();
  Wire.begin(gI2cAddress);
  Wire.onReceive(receiveEvent);
  
  // IR
  irRecv.enableIRIn();

  // Sensor
  allocateSensorColor();
  setLedColor(sensorColor);
}

void loop() {
  if (playerAlive) {
    allocateSensorColor();
    setLedColor(sensorColor);
  } else {
    setLedColor(strip.Color(0,0,0,1));
  }
  
  if (irRecv.decode(&irResults)) {
    infinitagCore.irDecode(irResults.value);
    
    if(infinitagCore.irRecvTeamId != playerTeamId)
    {
      sendCmdIrShot(irResults.value);
    }
    
    delay(200);
    irRecv.enableIRIn();
  }
  
  delay(10);
}

void setLedColor(uint32_t c) {
  for (uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}


byte queryI2CAddressFromMaster() {
  byte i2cAddress = DHCP_DEFAULT_SLAVE_ADDRESS;
  Wire.begin(DHCP_DEFAULT_SLAVE_ADDRESS);

  while(Wire.requestFrom(DHCP_MASTER_ADDRESS, 1) != 1)
  {
    //don't block i2c bus
    setLedColor(strip.Color(100,0,0,0));
    delay(250);
    setLedColor(strip.Color(0,0,0,0));
    delay(250);
  }
  i2cAddress = Wire.read();

  setLedColor(strip.Color(0,0,0,255));
  while(Wire.available())
  {
    Wire.read();  
  }
  return i2cAddress;
}

void allocateSensorColor() {
  sensorColor = sensorColors[playerTeamId - 1];
}

/**
 * Before calling this function the master should be given some appropriate 
 * amount of time to start the i2c interface
 */
byte getI2CAddress(int addressOfAddress, bool keepAddress)
{
  byte i2cAddress = EEPROM.read(addressOfAddress);
  if(i2cAddress == 255) //override EEPROM default value
  {
    i2cAddress = DHCP_DEFAULT_SLAVE_ADDRESS;
  }
  if(keepAddress)
  {
    if(i2cAddress == DHCP_DEFAULT_SLAVE_ADDRESS)
    {
      i2cAddress = queryI2CAddressFromMaster();
      EEPROM.write(addressOfAddress, i2cAddress);
    }  
    else
    {
      return i2cAddress;  
    }
  }
  else
  {    
    i2cAddress = queryI2CAddressFromMaster();
  }
  
  return i2cAddress;
}

void waitLed(int loops){
  for (int i = 0; i < (loops * 2); i++) {
    strip.setPixelColor(0, strip.Color(0,0,0,255));
    strip.setPixelColor(1, strip.Color(0,0,0,0));
    strip.setPixelColor(2, strip.Color(0,0,0,0));
    strip.show();
    delay(166);
  
    strip.setPixelColor(0, strip.Color(0,0,0,0));
    strip.setPixelColor(1, strip.Color(0,0,0,255));
    strip.setPixelColor(2, strip.Color(0,0,0,0));
    strip.show();
    delay(167);
  
    strip.setPixelColor(0, strip.Color(0,0,0,0));
    strip.setPixelColor(1, strip.Color(0,0,0,0));
    strip.setPixelColor(2, strip.Color(0,0,0,255));
    strip.show();
    delay(167);
  }
}

void receiveEvent(int howMany) {
  Serial.println("receiveEvent");
  int byteCounter = 0;
  byte data[9] = {
    B0,
    B0,
    B0,
    B0,
    B0,
    B0,
    B0,
    B0,
    B0,
  };
  
  while (Wire.available()) {
    data[byteCounter] = Wire.read();
    Serial.println(data[byteCounter]);
    byteCounter++;
  }

  switch (data[0]) {
    // Set TeamId
    case 0x02:
      if (byteCounter == 2) {
        playerTeamId = data[1];
        allocateSensorColor();
        setLedColor(sensorColor);
      }
      break;
      
    // Set PlayerId
    case 0x03:
      if (byteCounter == 2) {
        playerId = data[1];
      }
      break;
      
    // Set Alive
    case 0x07:
      if (byteCounter == 2) {
        setAlive((data[1] == 0x01));
      }
      break;
  }
}


void sendCmd(byte data[], unsigned int byteLength) {
  // DHCP_MASTER_ADDRESS das richtige Ziel?
  Wire.beginTransmission(DHCP_MASTER_ADDRESS);
  Wire.write(data, byteLength);
  Wire.endTransmission();
}

void sendCmdGetSensorID() {
  byte data[1] = {
    0x07
  };
  sendCmd(data, 1);
}

void sendCmdPong() {
  byte data[2] = {
    0x08,
    gI2cAddress
  };
  sendCmd(data, 2);
}

void sendCmdIrShot(unsigned long code) {
  byte result[3];
  
  infinitagCore.irToBytes(code, &result[0]);
  
  byte data[4] = {
    0x06,
    result[0],
    result[1],
    result[2]
  };
  sendCmd(data, 4);
}

void setAlive(bool alive) {
  playerAlive = alive;
}

