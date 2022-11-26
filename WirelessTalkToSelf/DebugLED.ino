#define PIN_DEBUG_LED A1

int ledState = LOW;      

void setupDebugLED()
{
    pinMode(PIN_DEBUG_LED, A1);
}

void setDebugLED(bool on)
{
    if(on)
        ledState = HIGH;
    else
        ledState = LOW;

    digitalWrite(PIN_DEBUG_LED, ledState);
}

void toggleDebugLED()
{
    if(ledState == LOW)
        ledState = HIGH;
    else
        ledState = LOW;
    
    digitalWrite(PIN_DEBUG_LED, ledState);
}

void DebugLEDOff()
{
    ledState = LOW;

    digitalWrite(PIN_DEBUG_LED, ledState);
}

void DebugLEDOn()
{
    ledState = HIGH;

    digitalWrite(PIN_DEBUG_LED, ledState);
}

