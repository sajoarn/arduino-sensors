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
  getTemperature();
}


