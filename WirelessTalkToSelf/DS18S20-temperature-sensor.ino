#include "DS1820-temperature-init.h"

//////////////////////////////////////////////////
// Digital Temp DS18S20
//////////////////////////////////////////////////
// Source: Kaman K5 Sensor Kit, which points to http://osoyoo.com/?p=409 in the project
// Which apparently rips from the OneWire DS18x20_Temperature.ino example
void setupDS1820TemperatureSensor()
{
  pinMode(DS1820_DATA_PIN, OUTPUT);
}


void writeToTemperatureRegisters(byte addr[8])
{
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);  // start conversion, with parasite power on at the end
}

float readFromTemperatureRegisters(byte addr[8])
{
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
  byte present = 0;
  byte data[12];

  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);  // Read Scratchpad
 
  for (byte i = 0; i < 9; i++) { 
    data[i] = ds.read();
  }

  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  
  if (SignBit)
  {
    TReading = (TReading ^ 0xffff) + 1;
  }
  Tc_100 = (6 * TReading) + TReading / 4; 

  return (float)(Tc_100) / 100.0f;
}

void getTemperature()
{
  byte addr[8];
  float temperatureCelsius;
  float temperatureFahrenheit;
 
  if ( !ds.search(addr)) {
      ds.reset_search();
      return;
  }

  writeToTemperatureRegisters(addr);
  delay(1000);
  temperatureCelsius = readFromTemperatureRegisters(addr);
  temperatureFahrenheit = celsiusToFahrenheit(temperatureCelsius);
  printToSerial(temperatureCelsius, temperatureFahrenheit);

}

void printToSerial(float celsius, float fahrenheit)
{
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.print(celsius);
  Serial.print("C ");
  Serial.print(fahrenheit);
  Serial.print("F\n");
}

float celsiusToFahrenheit(float celsius)
{
  float fahrenheit = (celsius * 1.8) + 32;
  return fahrenheit;
}

