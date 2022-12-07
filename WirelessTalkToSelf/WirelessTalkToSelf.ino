#include "DS1820-temperature-init.h"

//////////////////////////////////////////////////////


// // Include RadioHead Amplitude Shift Keying Library
// #include <RH_ASK.h>
// // Include dependant SPI Library 
// #include <SPI.h> 
 
// // Create Amplitude Shift Keying Object
// RH_ASK rf_driver;
 
////////////////////////////////////////////

#define HEARTBEAT_LED_PIN LED_BUILTIN // to a 1kOhm in series with an LED if not builtin

#define TASK_INTERVAL_HEARTBEAT 1000 //milliseconds
#define TASK_INTERVAL_REQUEST_TEMP 2000 //milliseconds (actual temperature display update time is double this because task requires two passes to retrieve temperature)
#define TASK_WIRLESS_MSG_TX 1000 //milliseconds
#define TASK_WIRELESS_MSG_RECEIVE 1 //milliseconds
#define TEMPERATURE_WAIT_DURATION 750 //milliseconds

enum currentTemperPass {
  Request,
  Retrieve,
  Wait,
  NUM_OF_TEMPERPASS_OPTIONS
};

//global display number to be used across loops instead of delay
float displayValue = 0;
bool initFlag = false;
bool firstLoop = true;
unsigned long taskCounterHeartbeat, taskCounterMsgTX, taskCounterMsgRX, taskCounterTemperature, subtaskCounterWait = 0;
int heartbeatState = LOW;
currentTemperPass nextPass = NUM_OF_TEMPERPASS_OPTIONS;

//////////////////////////////////////////////////
// Task Definitions
//////////////////////////////////////////////////
void task_Heartbeat()
{
  if(heartbeatState == LOW)
  {
    heartbeatState = HIGH;
  }
  else
  {
    heartbeatState = LOW;
  }

  digitalWrite(HEARTBEAT_LED_PIN, heartbeatState);
}

void task_DisplayTemperature()
{
  // displayBasedOnNumber(displayValue);
}

void task_WirelessMsgTX()
{
  WirelessTXMsg();
  Serial.println("WirlesMsgTX");
}

void task_WirelessMsgRX()
{
  WirelessRXMsg();
  // Serial.println("WirlesMsgRX");
}

void task_GetTemperature(currentTemperPass pass, unsigned long currentTime)
{
  String debug = "task_GetTemperature ";

  byte addr[8];
  float temperatureCelsius;
  float temperatureFahrenheit;

  if ( !ds.search(addr)) {
      ds.reset_search();
      return;
  }

  switch (pass)
  {
  case Request:
    writeToTemperatureRegisters(addr);
    nextPass = Wait;
    break;

  case Retrieve:
    temperatureCelsius = readFromTemperatureRegisters(addr);
    temperatureFahrenheit = celsiusToFahrenheit(temperatureCelsius);
    printToSerial(temperatureCelsius, temperatureFahrenheit);
    //set value to display
    displayValue = temperatureFahrenheit;
    debug += displayValue;
    nextPass = Request;
    break;
  
  case Wait:
    if(currentTime >= subtaskCounterWait + TEMPERATURE_WAIT_DURATION)
    {
      nextPass = Retrieve;
      subtaskCounterWait = currentTime;
    }
    break;
  default:
    break;
  }

  Serial.println(debug);
}

//////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////

//setup runs once
void setup() {

    Serial.begin(9600); //9600 baud serial out
    
    WirelessTXRXSetup();
    setupDebugLED();
    setupDS1820TemperatureSensor();

    // pinMode(DS1820_DATA_PIN, OUTPUT);
    pinMode(HEARTBEAT_LED_PIN, OUTPUT);
}

//loop repeated once per tick
void loop() {

  unsigned long currentMillis = millis();

  String debug = "loop ";

  // //Initialize display value
  // if(!initFlag && displayValue == 0)
  // {
  //   //Start the retrieve on the first loop
  //   if(firstLoop)
  //   {
  //     task_GetTemperature(Request, currentMillis);
  //     firstLoop = false;
  //   }
    
  //   //Repeat this initialization until a non-zero display value is found
  //   task_GetTemperature(nextPass, currentMillis);
  //   if(displayValue != 0)
  //     initFlag = true;
  // }

  //Tasks:

  if(currentMillis >= taskCounterHeartbeat + TASK_INTERVAL_HEARTBEAT)
  {
    task_Heartbeat();

    taskCounterHeartbeat = currentMillis;
  }

  if(currentMillis >= taskCounterMsgTX + TASK_WIRLESS_MSG_TX)
  {
    task_WirelessMsgTX();

    taskCounterMsgTX = currentMillis;
  }

  if(currentMillis >= taskCounterMsgRX + TASK_WIRLESS_MSG_TX)
  {
    task_WirelessMsgRX();

    taskCounterMsgRX = currentMillis;
  }

  // if(currentMillis >= taskCounterTemperature + TASK_INTERVAL_REQUEST_TEMP)
  // {
  //   task_GetTemperature(nextPass, currentMillis);

  //   taskCounterTemperature = currentMillis;
  //   debug += "taskCounterTemperature ";
  //   debug += displayValue;
  // Serial.println(debug);
  // }

    task_WirelessMsgRX();


}
