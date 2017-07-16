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
#define EEPROM_I2C_ADDRESS 0x00
byte g_i2cAddress;
int ir_receive_pin = 7;

// Player
byte playerTeamId = 1;
byte playerId = 1;

// Infinitag Inits
Infinitag_Core infinitagCore;

// Vendor Inits
Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, PIN, NEO_GRBW + NEO_KHZ800);
decode_results ir_results;
IRrecv ir_recv(ir_receive_pin);

// Sensor Inits
uint32_t sensorColors[] = {strip.Color(0,255,0,0), strip.Color(0,255,255,0), strip.Color(255,255,0,0), strip.Color(255,0,255,0),strip.Color(0,255,0,255), strip.Color(0,255,255,255), strip.Color(255,255,0,255), strip.Color(255,0,255,255)};
uint32_t sensorColor = 0;

void setup() {  
  Serial.begin(57600);
  
  // LEDs
  strip.begin();
  waitLed(15);

  //Wait For Master to Boot 
  //g_i2cAddress = 0x22; //queryI2CAddressFromMaster();//(EEPROM_I2C_ADDRESS, false);
  g_i2cAddress = queryI2CAddressFromMaster();
  Wire.begin(g_i2cAddress);
  Wire.onReceive(receiveEvent);
  
  // IR
  ir_recv.enableIRIn();
  Serial.println("Los gehts");

  // Sensor
  sensorColor = sensorColors[g_i2cAddress - 0x22];
}

void loop() {
  setLedColor(strip.Color(0,0,0,255));

  if (ir_recv.decode(&ir_results)) {
    if (infinitagCore.ir_decode(ir_results.value)) {
      if(infinitagCore.ir_recv_team_id != playerTeamId)
      {
        byte demoSignal = B00000001;
        Wire.beginTransmission(DHCP_MASTER_ADDRESS);
        Wire.write(demoSignal);
        Wire.endTransmission();
        
        setLedColor(strip.Color(255,0,0,0));
      }
    } else {
      setLedColor(strip.Color(0,0,255,0));
    }
    
    delay(200);
    ir_recv.enableIRIn();
  } else {
    setLedColor(sensorColor);
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
  Serial.println("ReceiveEvent");
  Serial.println(playerTeamId);
  Serial.println(playerId);
  Serial.println("=========");
  if (Wire.available()) {
    playerTeamId = Wire.read();
    setLedColor(strip.Color(255,0,255,0));
    delay(50);
    setLedColor(strip.Color(0,0,0,0));
    delay(50);
  }
  Serial.println(playerTeamId);
  if (Wire.available()) {
    playerId = Wire.read();
    setLedColor(strip.Color(255,0,255,0));
    delay(50);
    setLedColor(strip.Color(0,0,0,0));
    delay(50);
  }
  Serial.println(playerId);
  Serial.println("=========");
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
    g_i2cAddress
  };
  sendCmd(data, 2);
}

void sendCmdIrShot(unsigned long code) {
  byte result[3] = {
    B0,
    B0,
    B0
  };
  infinitagCore.ir_to_bytes(code, result);
  byte data[4] = {
    0x06,
    result[0],
    result[1],
    result[2]
  };
  sendCmd(data, 4);
}
