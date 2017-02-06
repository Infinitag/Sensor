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
#define PIN 3
#define TEAM 1
#define EEPROM_I2C_ADDRESS 0x00
byte g_i2cAddress;
int ir_receive_pin = 7;

// Infinitag Inits
Infinitag_Core infinitagCore;

// Vendor Inits
Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, PIN, NEO_GRBW + NEO_KHZ800);
decode_results ir_results;
IRrecv ir_recv(ir_receive_pin);

void setup() {  
  // LEDs
  strip.begin();
  waitLed(8);

  //Wait For Master to Boot 
  g_i2cAddress = queryI2CAddressFromMaster();//(EEPROM_I2C_ADDRESS, false);
  Wire.begin(g_i2cAddress);
  
  // IR
  ir_recv.enableIRIn();
}

void loop() {

  if (ir_recv.decode(&ir_results)) {
    if (infinitagCore.ir_decode(ir_results.value)) {
      if(infinitagCore.ir_recv_team_id != TEAM)
      {
        setLedColor(strip.Color(255,0,0,0));
      }
    } else {
      setLedColor(strip.Color(0,0,255,0));
    }
    
    delay(200);
    ir_recv.enableIRIn();
  } else {
    setLedColor(strip.Color(0,255,0,0));
  }
  
  delay(10);
}

void setLedColor(uint32_t c) {
  for (uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}


byte queryI2CAddressFromMaster()
{
  byte i2cAddress = DHCP_DEFAULT_SLAVE_ADDRESS;
  Wire.begin(DHCP_DEFAULT_SLAVE_ADDRESS);

  while(Wire.requestFrom(DHCP_MASTER_ADDRESS, 1) != 1)
  {
    //don't block i2c bus
    setLedColor(strip.Color(255,0,0,0));
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
    setLedColor(strip.Color(0,0,0,255));
    delay(250);
    setLedColor(strip.Color(0,0,0,0));
    delay(250);
  }
}
