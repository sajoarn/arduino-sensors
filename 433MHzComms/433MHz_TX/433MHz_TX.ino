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
  float humd = TempAndHumid.readHumidity();
  float temp = TempAndHumid.readTemperature();  

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
  delay(10000);
}
