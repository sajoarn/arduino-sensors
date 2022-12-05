//////////////////////////////////////////////////////////////////////
/*
Code altered/written by Sandra Arnold under the MIT license in 2022. 
Uses snippets from the following:

Uses HTU21D Humidity Sensor Example Code by Nathan Seidle 
for Sparkfrun Electronics in 2013 under the public domain Beerware License

Original Attribution:
 
Uses the HTU21D library to display the current humidity and temperature
 
Open serial monitor at 9600 baud to see readings. Errors 998 if not sensor is detected. Error 999 if CRC is bad.

For HTU21D:
 Hardware Connections (Breakoutboard to Arduino):
 -VCC = 3.3V
 -GND = GND
 -SDA = A4 (use inline 330 ohm resistor if your board is 5V)
 -SCL = A5 (use inline 330 ohm resistor if your board is 5V)

Also based on the SSD1306 I2C Display Example code by Adafruit under the BSD license. 

Original Attribution:

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information

 */
////////////////////////////////////////////////////////////////////////
#include <Wire.h>
#include "SparkFunHTU21D.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Create an instance of the object for HTU21D
HTU21D TempAndHumid;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO/nano:       A4(SDA), A5(SCL)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; usually 0x3D for 128x64, 0x3C for 128x32 (but not in this case - this is a 128x64)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer and display one pixel to indicate screen is on
  display.display(); //show splash
  delay(500);
  display.clearDisplay();
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();

  TempAndHumid.begin();
}

void loop()
{
  float humd = TempAndHumid.readHumidity();
  float temp = TempAndHumid.readTemperature();

  Serial.print("Time:");
  Serial.print(millis());
  Serial.print(" Temperature:");
  Serial.print(temp, 1);
  Serial.print("C");
  Serial.print(" Humidity:");
  Serial.print(humd, 1);
  Serial.print("%");

  displayHumidityAndTemperature(temp, humd);

  Serial.println();
  delay(60000);
}

void displayHumidityAndTemperature(float temperature, float humidity)
{
    float fahrenheit = temperature * 1.8 + 32;

    display.clearDisplay();
    display.setTextSize(1); // Size: 6px wide by 7px high
    display.setTextColor(SSD1306_WHITE);

    //Size 1 text monospace = 6px wide, 7px tall; use 3px margin vertically between lines
    //Center the two blocks of text horizontally and with equal padding between lines vertically
    //Horizontal left pad: (128px - (13 letters * 6px + 5 chars * 6px) / 2 = 10px
    //Humidity line num pad: 10px offset + 13 letters * 6 px  = 88px
    //Top line vert pad: (64px - ((7px + 3 pad + 7px) + 7px)) / (3 pad blocks) = ~13
    //Bottom line vert: 13px offset + (7px + 3px pad + 7px) + 13px+1 pad (round up) = 44px

    display.setCursor(10, 13);
    display.print(F("Temperature: "));
    display.print(temperature);
    display.print(F("C\n"));

    display.setCursor(88, 23);
    display.print(fahrenheit);
    display.print(F("F\n"));

    display.setCursor(10, 44);
    display.print(F("Humidity:"));
    display.setCursor(88, 44); //align with temperature
    display.print(humidity);
    display.print(F("%\n"));


    display.display();
}
