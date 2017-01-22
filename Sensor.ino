#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <EEPROM.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN 3

#define EEPROM_I2C_ADDRESS 0x00


#define MASTER_ADDRESS 0x20
#define SLAVE_ADDRESS 0x21

struct color
{
  byte r, g, b;
};

color Colors[] = {
  {0, 0, 0},
  {255, 0, 0},  
  {0, 255, 0},
  {0, 0, 255}
};

byte g_i2cAddress;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, PIN, NEO_GRBW + NEO_KHZ800);


byte queryI2CAddressFromMaster()
{
  byte i2cAddress = SLAVE_ADDRESS;
  Wire.begin(SLAVE_ADDRESS);

  while(Wire.requestFrom(MASTER_ADDRESS, 1) != 1)
  {
    //don't block i2c bus
    delay(50);
  }
  i2cAddress = Wire.read();

  while(Wire.available())
  {
    Wire.read();  
  }
  return i2cAddress;
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
    i2cAddress = SLAVE_ADDRESS;
  }
  if(keepAddress)
  {
    if(i2cAddress == SLAVE_ADDRESS)
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


void setup() {  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  //Wait For Master to Boot 
  delay(1000);

  g_i2cAddress = getI2CAddress(EEPROM_I2C_ADDRESS, false);
  
  Wire.begin(g_i2cAddress);
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(Colors[g_i2cAddress - SLAVE_ADDRESS].r, Colors[g_i2cAddress - SLAVE_ADDRESS].g, Colors[g_i2cAddress - SLAVE_ADDRESS].b), 0);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}