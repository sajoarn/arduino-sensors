#ifndef DS1820_TEMPERATURE_INIT_H
#define DS1820_TEMPERATURE_INIT_H

//////////////////////////////////////////////////
// Digital Temp DS18S20
//////////////////////////////////////////////////
// Source: Kaman K5 Sensor Kit, which points to http://osoyoo.com/?p=409 in the project
// Which apparently rips from the OneWire DS18x20_Temperature.ino example

#include <OneWire.h>

#define DS1820_DATA_PIN 10 // for DS1820 Data line
OneWire ds(DS1820_DATA_PIN);

#endif