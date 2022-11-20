/*
Common Anode Display 3461BS-1
Pinouts:
12 11 10 9  8  7
|  |  |  |  |  |
-----------------
 _   _   _   _
|_| |_| |_| |_|
|_|.|_|.|_|.|_|.
-----------------
|  |  |  |  |  |
1  2  3  4  5  6

Segment Names:
    A
   ---
F |   | B
  | G |
   ---
E |   | C
  |   |
   --- *(DP)
    D

digit selects (pulled HIGH to enable digit):
digit 1 = pin 12
digit 2 = pin 9
digit 3 = pin 8
digit 4 = pin 6

segment pins (pulled LOW to turn on segment, HIGH to turn off segment):
segment A = pin 11
segment B = pin 7
segment C = pin 4
segment D = pin 2
segment E = pin 1
segment F = pin 10
segment G = pin 5
decimal point = pin 3

*/

#include <OneWire.h>
OneWire ds(A0);

#define TASK_INTERVAL_HEARTBEAT 1000 //milliseconds
#define TASK_INTERVAL_REQUEST_TEMP 1000 //milliseconds

//Arduino pin assignments:
enum pins {
  D1 = 2,
  D2 = 3,
  D3 = 4,
  D4 = 5,
  segA = 6,
  segB = 7,
  segC = 8,
  segD = 9,
  segE = 10,
  segF = 11,
  segG = 12,
  decpt = 13,
  NUM_OF_PINS = 14
};

//global display number to be used across loops instead of delay
float displayNumber = 0;
bool initFlag = 0;

//////////////////////////////////////////////////
// Digital Temp DS18S20
//////////////////////////////////////////////////
// Source: Kaman K5 Sensor Kit, which points to http://osoyoo.com/?p=409 in the project

void serialTempRead(int updateIntervalSeconds)
{
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
 
  if ( !ds.search(addr)) {
      ds.reset_search();
      return;
  }
 
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); 
 
  // delay(1000);  
  displayforDuration(displayNumber, 1000);
 
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);  
 
  for ( i = 0; i < 9; i++) { 
    data[i] = ds.read();
  }
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.print("Temperature: ");
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  
  if (SignBit)
  {
    TReading = (TReading ^ 0xffff) + 1;
  }
  Tc_100 = (6 * TReading) + TReading / 4; 
  float celsius = (float)(Tc_100) / 100;
  displayNumber = celsiusToFahrenheit(celsius);
  Whole = Tc_100 / 100; 
  Fract = Tc_100 % 100;

  if (SignBit)
  {
     Serial.print("-");
  }
  Serial.print(Whole);
  Serial.print(".");
  if (Fract < 10)
  {
     Serial.print("0");
  }
  Serial.print(Fract);
  Serial.print("C Fahrenheit: ");
  Serial.print(displayNumber);
  Serial.print("F ");
  Serial.print(celsius);
  Serial.print("C ");

  //TODO: clean up Serial output info
  Serial.print(" Raw Values, TReading: ");
  Serial.print(TReading);
  Serial.print(" Tc_100: ");
  Serial.print(Tc_100);

  Serial.print("\n");

  //sleep for updateIntervalSeconds
  displayforDuration(displayNumber, (updateIntervalSeconds * 1000));
}

//Includes initial pass of setting display temp without yet displaying
//Takes one sec to get first reading
void firstTempRead()
{
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
 
  if ( !ds.search(addr)) {
      ds.reset_search();
      return;
  }
 
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); 
 
  delay(1000);  
 
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);  
 
  for ( i = 0; i < 9; i++) { 
    data[i] = ds.read();
  }
  Serial.print("Temperature: ");
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  
  if (SignBit)
  {
    TReading = (TReading ^ 0xffff) + 1;
  }
  Tc_100 = (6 * TReading) + TReading / 4; 
  Whole = Tc_100 / 100; 
  Fract = Tc_100 % 100;

  float celsius = (float)(Tc_100) / 100;
  displayNumber = celsiusToFahrenheit(celsius);
  //TODO: clean up serial display info
  if (SignBit)
  {
     Serial.print("-");
  }
  Serial.print(Whole);
  Serial.print(".");
  if (Fract < 10)
  {
     Serial.print("0");
  }
  Serial.print(Fract);
  Serial.print("C Fahrenheit: ");
  Serial.print(displayNumber);
  Serial.print("F converCels ");
  Serial.print(celsius);

  Serial.print("C \n");
}

float celsiusToFahrenheit(float celsius)
{
  float fahrenheit = (celsius * 1.8) + 32;
  return fahrenheit;
}

//////////////////////////////////////////////////
// Four Digit Seven Segment Display
//////////////////////////////////////////////////

void setAllDigitsOff()
{
  for(int digit = D1; digit < D4 + 1; digit++)
  {
    digitalWrite(digit, LOW);
  }
}

void setAllSegmentsOff(){
    //Anode segments means they must be set HIGH for segments to be off when digit is on
    for(int segment = segA; segment < NUM_OF_PINS; segment++)
    {
    digitalWrite(segment, HIGH);  
    }
}

void setDigit(int dig, bool turnOn)
{
  int digSet = turnOn ? HIGH : LOW;

  //Digits share segments and so must be exclusive
  if(turnOn)
  {
    setAllDigitsOff();
  }
  else
  {
    //when turning off a digit, turn off all segments to avoid ghosting
    setAllSegmentsOff();
  }
  
  switch (dig)
  {
  case D1:
    digitalWrite(D1, digSet);
    break;
  case D2:
    digitalWrite(D2, digSet);
    break;
  case D3:
    digitalWrite(D3, digSet);
    break;
  case D4:
    digitalWrite(D4, digSet);
    break;
  default:
    break;
  }
}

void displayZeroToNine(int num){

  //Can only display 0-9
  int displayNum = num % 10;

  setAllSegmentsOff();

  switch (displayNum)
  {
  case 0:
    digitalWrite(segA, LOW);
    digitalWrite(segB, LOW);
    digitalWrite(segC, LOW);
    digitalWrite(segD, LOW);
    digitalWrite(segE, LOW);
    digitalWrite(segF, LOW);
    break;
  case 1:
    digitalWrite(segB, LOW);
    digitalWrite(segC, LOW);
    break;
  case 2:
    digitalWrite(segA, LOW);
    digitalWrite(segB, LOW);
    digitalWrite(segD, LOW);
    digitalWrite(segE, LOW);
    digitalWrite(segG, LOW);
    break;
  case 3:
    digitalWrite(segA, LOW);
    digitalWrite(segB, LOW);
    digitalWrite(segC, LOW);
    digitalWrite(segD, LOW);
    digitalWrite(segG, LOW);
    break;
  case 4:
    digitalWrite(segB, LOW);
    digitalWrite(segC, LOW);
    digitalWrite(segF, LOW);
    digitalWrite(segG, LOW);
    break;
  case 5:
    digitalWrite(segA, LOW);
    digitalWrite(segC, LOW);
    digitalWrite(segD, LOW);
    digitalWrite(segF, LOW);
    digitalWrite(segG, LOW);
    break;
  case 6:
    digitalWrite(segA, LOW);
    digitalWrite(segC, LOW);
    digitalWrite(segD, LOW);
    digitalWrite(segE, LOW);
    digitalWrite(segF, LOW);
    digitalWrite(segG, LOW);
    break;
  case 7:
    digitalWrite(segA, LOW);
    digitalWrite(segB, LOW);
    digitalWrite(segC, LOW);
    break;
  case 8:
    digitalWrite(segA, LOW);
    digitalWrite(segB, LOW);
    digitalWrite(segC, LOW);
    digitalWrite(segD, LOW);
    digitalWrite(segE, LOW);
    digitalWrite(segF, LOW);
    digitalWrite(segG, LOW);
    break;
  case 9:
    digitalWrite(segA, LOW);
    digitalWrite(segB, LOW);
    digitalWrite(segC, LOW);
    digitalWrite(segD, LOW);
    digitalWrite(segF, LOW);
    digitalWrite(segG, LOW);
    break;
  default:
    break;
  }
}

void displayDecimalPoint() {
    digitalWrite(decpt, LOW);
}

void displayNegativeSign(){
    digitalWrite(segG, LOW);
}

void displayUpToThousands(float value)
{
  //Max number to display is 9999
  //this is unnecessary but...eh...no overflow or something...
  int displayValue = (int)(abs(value)) % 10000;

  int ones = displayValue % 10;
  int tens = (displayValue / 10) % 10;
  int hundreds = (displayValue / 100) % 10; 
  int thousands = (displayValue / 1000) % 10;

  //Trim off leading zeroes
  //Display Ones:
  setDigit(D4, true);
  displayZeroToNine(ones);
  delay(1);
  setDigit(D4, false);

  //Display Tens:
  if(!(thousands == 0 && hundreds == 0 && tens == 0))
  {
    setDigit(D3, true);
    displayZeroToNine(tens);
    delay(1);
    setDigit(D3, false);
  }

  //Display Hundreds:
  if(!(thousands == 0 && hundreds == 0))
  {
    setDigit(D2, true);
    displayZeroToNine(hundreds);
    delay(1);
    setDigit(D2, false);  
  }
  
  //Display Thousands:
  if(thousands != 0)
  {
    setDigit(D1, true);
    displayZeroToNine(thousands);
    delay(1);
    setDigit(D1, false);    
  }
  
}

void displayHundredsWithOneDecimal(float value)
{
  //Max number to display is 999.9
  //this is unnecessary but...eh...no overflow or something...
  int displayValue = (int)(abs(value)) % 1000;

  //quirk of C++ where these steps need to be separate otherwise will be off by 0.1
  //may or may not be a problem with arduino's language...idk. Better safe than sorry
  float tenthsStep1 = value / 0.1;
  int tenths = (int)tenthsStep1 % 10;
  int ones = displayValue % 10;
  int tens = (displayValue / 10) % 10;
  int hundreds = (displayValue / 100) % 10; 

  //Display Tenths:
  setDigit(D4, true);
  displayZeroToNine(tenths);
  delay(1);
  setDigit(D4, false);
  
  //Display Ones:
  setDigit(D3, true);
  displayZeroToNine(ones);
  displayDecimalPoint();
  delay(1);
  setDigit(D3, false);

  //Trim off leading zeroes
  //Display Tens:
  if(!(hundreds == 0 && tens == 0))
  {
    setDigit(D2, true);
    displayZeroToNine(tens);
    delay(1);
    setDigit(D2, false);
  }

  //Display Hundreds:
  if(!(hundreds == 0))
  {
    setDigit(D1, true);
    displayZeroToNine(hundreds);
    delay(1);
    setDigit(D1, false);  
  }
  
}

void displayNegativeTensWithOneDecimal(float value)
{
  //Max number to display is 99.9
  //this is unnecessary but...eh...no overflow or something...
  int displayValue = (int)(abs(value)) % 100;

  //quirk of C++ where these steps need to be separate otherwise will be off by 0.1
  //may or may not be a problem with arduino's language...idk. Better safe than sorry
  float tenthsStep1 = abs(value) / 0.1;
  int tenths = (int)tenthsStep1 % 10;
  int ones = displayValue % 10;
  int tens = (displayValue / 10) % 10;
  bool negative = value < 0;

  //Display Tenths:
  setDigit(D4, true);
  displayZeroToNine(tenths);
  delay(1);
  setDigit(D4, false);
  
  //Display Ones:
  setDigit(D3, true);
  displayZeroToNine(ones);
  displayDecimalPoint();
  delay(1);
  setDigit(D3, false);

  //Trim off leading zeroes
  //Display Tens:
  if(tens != 0)
  {
    setDigit(D2, true);
    displayZeroToNine(tens);
    delay(1);
    setDigit(D2, false);
  }

  //Display negative sign
  if(negative)
  {
    setDigit(D1, true);
    displayNegativeSign();
    delay(1);
    setDigit(D1, false);  
  }

}

void displayNegativeUpToHundreds(float value)
{
  //Max number to display is 9999
  //this is unnecessary but...eh...no overflow or something...
  int displayValue = (int)(abs(value)) % 10000;

  int ones = displayValue % 10;
  int tens = (displayValue / 10) % 10;
  int hundreds = (displayValue / 100) % 10; 
  bool negative = value < 0;


  //Trim off leading zeroes
  //Display Ones:
  setDigit(D4, true);
  displayZeroToNine(ones);
  delay(1);
  setDigit(D4, false);


  //Display Tens:
  if(!(hundreds == 0 && tens == 0))
  {
    setDigit(D3, true);
    displayZeroToNine(tens);
    delay(1);
    setDigit(D3, false);
  }

  //Display Hundreds:
  if(!(hundreds == 0))
  {
    setDigit(D2, true);
    displayZeroToNine(hundreds);
    delay(1);
    setDigit(D2, false);  
  }
  
  //Display negative sign
  if(negative)
  {
    setDigit(D1, true);
    displayNegativeSign();
    delay(1);
    setDigit(D1, false);  
  }
  
}


//alternative to "delay"
void displayforDuration(float value, int milliseconds)
{
  int timestamp = millis();

  while(millis() < (timestamp + milliseconds))
  {
      float tempStep1 = abs(value) / 0.1;
      int tempTenths = (int)tempStep1 % 10;
      bool negative = value < 0;

      if(tempTenths != 0)
      {
        if(negative)
        {
          displayNegativeTensWithOneDecimal(value);
        }
        else
        {
          displayHundredsWithOneDecimal(value);
        }
      }
      else
      {
        if(negative)
        {
          displayNegativeUpToHundreds(value);
        }
        else
        {
          displayUpToThousands(value);
        }
      }
  }
}

void displayBasedOnNumber(float value)
{
  float tempStep1 = abs(value) / 0.1;
  int tempTenths = (int)tempStep1 % 10;
  bool negative = value < 0;

  if(tempTenths != 0)
  {
    if(negative)
    {
      displayNegativeTensWithOneDecimal(value);
    }
    else
    {
      displayHundredsWithOneDecimal(value);
    }
  }
  else
  {
    if(negative)
    {
      displayNegativeUpToHundreds(value);
    }
    else
    {
      displayUpToThousands(value);
    }
  }
}



//////////////////////////////////////////////////
// Examples
//////////////////////////////////////////////////

void example_loopThroughAllSegments()
{
  for(int digit = D1; digit < D4 + 1; digit++)
    {
      digitalWrite(digit, HIGH);

        for(int i = segA; i < NUM_OF_PINS; i++)
        {
        digitalWrite(i, LOW);  
        delay(1000);
        digitalWrite(i, HIGH); 
        }
        
      digitalWrite(digit, LOW);
    }
}

void example_countToNineOnEachDigit()
{
  for(int dig = D1; dig < D4 + 1; dig++)
    {
      setDigit(dig, true);
      for(int i = 0; i < 10; i++)
      {
        displayZeroToNine(i);
        delay(1000);
      }
      setDigit(dig, false);
    }
}

void example_displayRuntime()
{
  int runtime = millis() / 1000;
    
  displayUpToThousands(runtime);
}

void example_chooseDisplayTypeBasedOnNumber()
{
  float array[11] = {0, 0.3, -0.5, 10.3, -45.4, 487.5, -486.9, 1, 34, 645, 1947};

  for(int i = 0; i < 11; i++)
  {
    displayforDuration(array[i], 1000);
  }
}

void example_displayTemperatureForSetDuration()
{
  //display for five seconds then update
  serialTempRead(5);
}



/////////////////////////////////////////////////////////////////////
// Main Execution
////////////////////////////////////////////////////////////////////

// the setup routine runs once when you press reset:
void setup() {                
    // initialize the digital pins as outputs.
    for(int index = 0; index < NUM_OF_PINS; index++)
    {
      pinMode(index, OUTPUT);
    }

    //Digital Outs default to LOW and must be inited to HIGH on startup to be OFF
    setAllSegmentsOff();
    setAllDigitsOff();

    //Use A0 as digital in for DS18B20 digital temp sensor serial line
    pinMode(A0, OUTPUT);
    // digitalWrite(A0, HIGH);
    Serial.begin(9600); //A0 9600 baud serial out
}

// the loop routine runs over and over again forever:
void loop() {
  if(!initFlag)
  {
    firstTempRead();
    initFlag = true;
  }
  // //TODO: Bug: seems to be increasingly long between update intervals
  // serialTempRead(10);

  
}
