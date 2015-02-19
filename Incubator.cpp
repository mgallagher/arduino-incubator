
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <Arduino.h>

// Pins
#define ONE_WIRE_BUS 12
#define ONE_WIRE_PWR 11
#define RELAY_PIN 2
#define BACKLIGHT_PIN 10
#define BackLightOn(pin) pinMode(pin, INPUT)
#define BackLightOff(pin) pinMode(pin, OUTPUT)

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

float lastTemp = 0.0;
unsigned long prevMillisForInterval = 0;
unsigned long buttonDownTime = 0;
int buttonState = 0;
int lastButtonState = 0;
bool isBackLightOn = true;    // Can't use digitalRead with the backlight pin, so this will do
bool isButtonDown = false;

// Variables user might want to adjust
float targetTemp = 80.0;            // Default target temperature
const float tempAllowance = 0.0;    // How much below the target temp we'll allow before heating
const int interval = 4000;          // How often we grab the temperature. Adjust as needed.
const int shortButtonHold = 1000;   // How long we'll require for short holds

void setup(void)
{
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(ONE_WIRE_PWR, OUTPUT);
  digitalWrite(ONE_WIRE_PWR, HIGH);
  sensor.setResolution(10);   // Decreases time to grab the temperature to ~200 ms.
                              // Default is 12, which would normally take ~700 ms 
                              // and frequently resulted in missed button presses
  sensor.begin();
  sensor.requestTemperatures();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("    Incubator   ");  // Welcome screen
  delay(1000);
  lcd.clear();
  lcd.print("F: ");
  lcd.print(sensor.getTempFByIndex(0));
  lcd.setCursor(0,2);
  lcd.print("Target: ");
  lcd.print(targetTemp);  
}

void loop()
{
  //Check analog values from LCD Keypad Shield
  int keypad = analogRead(0);
  switch(keypad)
  {
    // RIGHT
    case 0 ... 100:
      buttonState = 1;
      break;
    // UP
    case 101 ... 200:
      buttonState = 2;
      break;
    // DOWN
    case 201 ... 400:
      buttonState = 3;
      break;
    // LEFT
    case 401 ... 600:
      buttonState = 4;
      break;
    // SELECT - Requires a short hold to activate
    case 601 ... 800:
      if (!isButtonDown)
      {
        buttonDownTime = millis();
        isButtonDown = true;
      }
      else if (millis() - buttonDownTime > shortButtonHold)
      {
        buttonState = 5;
        isButtonDown = false; // Not working as intended. See line 92
      }
      break;
    default:
      isButtonDown = false;   // This is the only way I can get the short button toggle to work
      buttonState = -1;
  }

  // We only want to perform the action once for each button press
  // That's why we have this second switch
  if (buttonState != lastButtonState)
  {
    switch (buttonState)
    {
      // RIGHT - Increase target 10 degrees
      case 1:
        targetTemp += 10.0;
        lcd.setCursor(0,1);
        lcd.print("Target: ");
        lcd.print(targetTemp);
        break;
      // UP - Increase target half degree
      case 2:
        targetTemp += .5;
        lcd.setCursor(0,1);
        lcd.print("Target: ");
        lcd.print(targetTemp);
        break;
      // DOWN - Decrease target half degree
      case 3:
        targetTemp -= .5;
        lcd.setCursor(0,1);
        lcd.print("Target: ");
        lcd.print(targetTemp);
        break;
      // LEFT - Decrease target 10 degrees
      case 4:
        targetTemp -= 10.0;
        lcd.setCursor(0,1);
        lcd.print("Target: ");
        lcd.print(targetTemp);
        break;
      // SELECT - Toggle LCD (with backlight) on/off. 
      case 5:
        if (isBackLightOn)
        {
          BackLightOff(BACKLIGHT_PIN);
          lcd.noDisplay();
          isBackLightOn = false;
        }
        else
        {
          BackLightOn(BACKLIGHT_PIN);
          lcd.display();
          isBackLightOn = true;
        }
        break;
    }
  }
  lastButtonState = buttonState;

  unsigned long currentMillis = millis();
  // Once we're above our interval, we poll the temperature sensor
  if(currentMillis - prevMillisForInterval > interval) 
  {
    prevMillisForInterval = currentMillis;

    // Grab the temperature
    sensor.requestTemperatures();
    float currentTemp = sensor.getTempFByIndex(0);

    if (currentTemp != lastTemp)
    {
      lcd.setCursor(3,0);
      lcd.print(currentTemp);
      lcd.setCursor(0,1);
      lcd.print("Target: ");
      lcd.print(targetTemp);      
    }
    lastTemp = currentTemp;

    // Toggle relay
    if (currentTemp - tempAllowance < targetTemp)
    {
      if (digitalRead(RELAY_PIN) == LOW)
      {
        digitalWrite(RELAY_PIN, HIGH);
        lcd.setCursor(15,0);
        lcd.print("*");   // An indicator on the LCD for when the relay is on
      }
    }
    else
    {
      digitalWrite(RELAY_PIN, LOW);
      lcd.setCursor(15,0);
      lcd.print(" ");     // Remove indicator if relay is off, duh
    }
    // Used these for timing operations
    // unsigned long before = millis();
    // unsigned long after = millis();
    // Serial.println(after - before);
  }
}