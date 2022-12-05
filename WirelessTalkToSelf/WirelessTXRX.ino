//Source: https://lastminuteengineers.com/433mhz-rf-wireless-arduino-tutorial/

// Include RadioHead Amplitude Shift Keying Library
#include <RH_ASK.h>
// Include dependant SPI Library 
#include <SPI.h> 

#define PIN_TRANSMIT A0
#define PIN_RECEIVE A3
#define PIN_TX_CONTROLLER 5 //not used at the moment - should enable/disable the TX chip
#define TX_RX_BIT_RATE 2000 //bits per second
#define PTT_INVERTED false //if true, LOW would enable the TX chip WITH the PIN_TX_CONTROLLER line

// Create Amplitude Shift Keying Object
// RH_ASK(uint16_t speed = 2000, uint8_t rxPin = 11, uint8_t txPin = 12, uint8_t pttPin = 10, bool pttInverted = false);
RH_ASK rf_driver(TX_RX_BIT_RATE, PIN_RECEIVE, PIN_TRANSMIT, PIN_TX_CONTROLLER, PTT_INVERTED);

// Set buffer to size of expected message
#define BUFF_SIZE 32
uint8_t buf[BUFF_SIZE];
uint8_t buflen = sizeof(buf);
bool firstloop = true;

void WirelessTXRXSetup()
{
    // Initialize ASK Object
    rf_driver.init();

    pinMode(PIN_TRANSMIT, OUTPUT);
    pinMode(PIN_RECEIVE, INPUT);
}

///////////////////////////////////////////
// Transmit
///////////////////////////////////////////

void WirelessTXMsg()
{
    DebugLEDOn();
    const char *msg = "Hello World";
    rf_driver.send((uint8_t *)msg, strlen(msg));
    rf_driver.waitPacketSent();
    DebugLEDOff();
}

///////////////////////////////////////////
// Receive
///////////////////////////////////////////

void WirelessRXMsg()
{
  //DEBUG:
    if(firstloop)
    {
      String debugLine1 = "buff contains ";
      // String debugLine2 = "buff length: " + &buflen;

      for(uint8_t i = 0; i < 11; ++i)
      {
          debugLine1 += buf[i];
          debugLine1 += " ";
      }
      

      Serial.println(debugLine1);
      // Serial.println(debugLine2);
      firstloop = false;
    }
    
    // Check if received packet is correct size
    rf_driver.recv(buf, &buflen);
    if(buf[0] != 0)
    {
      Serial.println((char*)buf);
    }
    // if (rf_driver.recv(buf, &buflen))
    // {
      
    //   // Message received with valid checksum
    //   Serial.print("Message Received: ");
    //   Serial.println((char*)buf);         
    // }
}