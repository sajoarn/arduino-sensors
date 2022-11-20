
//this include probalby isn't necessary, but I'm putting it here anyway
#include "seven-segment-pins.h"

//////////////////////////////////////////////////
// Four Digit Seven Segment Display (see seven-segment-pins.h)
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

//alternative option to delay() - during delay() digits will stop showing
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
