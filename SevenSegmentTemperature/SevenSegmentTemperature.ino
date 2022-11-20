#include "seven-segment-pins.h"
#include "DS1820-temperature-init.h"

#define TASK_INTERVAL_HEARTBEAT 1000 //milliseconds
#define TASK_INTERVAL_REQUEST_TEMP 10000 //milliseconds (actual temperature display update time is double this because task requires two passes to retrieve temperature)
#define  TASK_INTERVAL_DISPLAY_SEVEN_SEG 1 //milliseconds
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
unsigned long taskCounterHeartbeat, taskCounterDisplay, taskCounterTemperature, subtaskCounterWait = 0;
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

  digitalWrite(A1, heartbeatState);
}

void task_DisplayTemperature()
{
  displayBasedOnNumber(displayValue);
}

void task_GetTemperature(currentTemperPass pass, unsigned long currentTime)
{
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
    //set value to display in seven segment task:
    displayValue = temperatureFahrenheit;
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
}

//////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////

//setup runs once
void setup() {

    // initialize the digital pins as outputs.
    for(int index = 0; index < NUM_OF_PINS; index++)
    {
      pinMode(index, OUTPUT);
    }

    pinMode(LED_BUILTIN, OUTPUT);

    //Digital Outs default to LOW and must be inited to HIGH on startup to be OFF
    setAllSegmentsOff();
    setAllDigitsOff();

    //Use A0 as digital in for DS18B20 digital temp sensor serial line
    pinMode(A0, OUTPUT);
    //Use A1 as digital for heartbeat LED
    pinMode(A1, OUTPUT);
    Serial.begin(9600); //A0 9600 baud serial out
}

//loop repeated once per tick
void loop() {

  unsigned long currentMillis = millis();

  //Initialize display value
  if(!initFlag && displayValue == 0)
  {
    //Start the retrieve on the first loop
    if(firstLoop)
    {
      task_GetTemperature(Request, currentMillis);
      firstLoop = false;
    }
    
    //Repeat this initialization until a non-zero display value is found
    task_GetTemperature(nextPass, currentMillis);
    if(displayValue != 0)
      initFlag = true;
  }

  //Tasks:

  if(currentMillis >= taskCounterHeartbeat + TASK_INTERVAL_HEARTBEAT)
  {
    task_Heartbeat();

    taskCounterHeartbeat = currentMillis;
  }

  if(currentMillis >= taskCounterTemperature + TASK_INTERVAL_REQUEST_TEMP)
  {
    task_GetTemperature(nextPass, currentMillis);

    taskCounterTemperature = currentMillis;
  }

  //don't display until initial temp is retrieved
  if(initFlag && currentMillis >= taskCounterDisplay + TASK_INTERVAL_DISPLAY_SEVEN_SEG)
  {
    task_DisplayTemperature();

    taskCounterDisplay = currentMillis;
  }


}
