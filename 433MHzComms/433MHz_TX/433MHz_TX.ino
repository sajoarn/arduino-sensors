// ask_transmitter.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to transmit messages
// with a simple ASK transmitter in a very simple way.
// Implements a simplex (one-way) transmitter with an TX-C1 module
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy, ESP-12

#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

#include <string.h>

RH_ASK driver;

// Temperature Sensor:
#include "SparkFunHTU21D.h"
//Create an instance of the object for HTU21D
HTU21D TempAndHumid;

enum msgBytes 
{
  TempNegative, //1 if negative, 0 if positive
  TempHundreds,
  TempTens,
  TempOnes,
  TempTenths,
  RHHundreds,
  RHTens,
  RHOnes,
  RHTenths,
  MAX_NUM_MSG_BYTES
};

void setup()
{
  Serial.begin(9600);	  // Debugging only
  Serial.println("433MHz TX Init");

  if (!driver.init())
    Serial.println("init failed");

  TempAndHumid.begin();
}

void loop()
{
  float temp = TempAndHumid.readTemperature();  
  float humd = TempAndHumid.readHumidity();

  uint8_t msgBuf[MAX_NUM_MSG_BYTES];

  for(uint8_t i = 0; i < MAX_NUM_MSG_BYTES; ++i)
  {
    msgBuf[i] = 0;
  }

  ////////////////// Temperature Bits
  //Watch the implicit and explicit type conversions here; abs() outputs an (int)
  msgBuf[TempNegative] = (uint8_t)(temp < 0);
  int absTemp = abs(temp);
  msgBuf[TempHundreds] = (uint8_t)((absTemp / 100) % 10);
  msgBuf[TempTens] = (uint8_t)((absTemp/10) % 10);
  msgBuf[TempOnes] = (uint8_t)((absTemp) % 10);
  float tenthsStep1 =   temp / 0.1; //need to separate this step out, otherwise it will be off by 0.1
  msgBuf[TempTenths] = (uint8_t)((abs(tenthsStep1)) % 10);

  ////////////////// RH Bits
  int intRH = (int)humd;
  msgBuf[RHHundreds] = (uint8_t)((intRH / 100) % 10);
  msgBuf[RHTens] = (uint8_t)((intRH/10) % 10);
  msgBuf[RHOnes] = (uint8_t)((intRH) % 10);
  tenthsStep1 = humd / 0.1; //need to separate this step out, otherwise it will be off by 0.1
  msgBuf[RHTenths] = (uint8_t)(((int)(tenthsStep1)) % 10);
    
  // Transmit message
  driver.printBuffer("MsgBuf: ", msgBuf, sizeof(msgBuf));
  driver.send((char*)msgBuf, sizeof(msgBuf));
  driver.waitPacketSent();  

  delay(10000);
}

void sendTempRHString()
{
  float temp = TempAndHumid.readTemperature();  
  float humd = TempAndHumid.readHumidity();

  char buf[RH_ASK_MAX_MESSAGE_LEN];
  dtostrf(temp, 5, 1, buf); //strip down float to important digits and convert to char array
  String str = String(buf); //convert temperature char array to string to concatenate later
  dtostrf(humd, 4, 1, buf); //strip down float
  str = str + "," + String(buf); //concatenate temperature and relative humidity to send in message  
  str.toCharArray(buf, sizeof(buf)); //put finished string back in buffer to send over message
  Serial.println((char*)buf);

  // Transmit message
  driver.send((char*)buf, sizeof(buf));
  driver.waitPacketSent();  
}
