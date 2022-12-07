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

//From RadioHead ask_transmitter.pde example:
#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

RH_ASK rf_driver; //defaults to D11 for RX and D12 for TX

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

  //Init 433MHz driver
  if (!rf_driver.init())
     Serial.println("init failed");

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

  char buffer[RH_ASK_MAX_MESSAGE_LEN];
  int temp_rh_msg = snprintf(buffer, sizeof buffer, "%f", temp);

  const char *msg = "hello";

    rf_driver.send((uint8_t *)temp_rh_msg, strlen(temp_rh_msg));
    rf_driver.waitPacketSent();
    delay(200);

  delay(60000);
}
