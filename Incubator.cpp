
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <Arduino.h>

// Define the pins
#define ONE_WIRE_BUS 12
#define ONE_WIRE_PWR 11
#define RELAY_PIN 2
#define BACKLIGHT_PIN 10

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

#define SafeBLon(pin) pinMode(pin, INPUT)
#define SafeBLoff(pin) pinMode(pin, OUTPUT)

// Globals
int count = 0;
float tempArray[2];
float targetArray[2];
float TARGET = 84.0;
bool isFirstRun = true;
bool relayOn = false;
const int delayVal = 3000;

void setup(void)
{
  Serial.begin(9600);
  pinMode(RELAY_PIN,OUTPUT);
  pinMode(ONE_WIRE_PWR, OUTPUT);
  digitalWrite(ONE_WIRE_PWR, HIGH);
  lcd.begin(16,2);
  lcd.print(" Kombucha Mode ");
  lcd.setCursor(0,1);
  sensors.begin();
}

void loop(void)
{
  delay(delayVal);
  // Request the temperature
  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempF = sensors.getTempFByIndex(0);
  targetArray[count] = TARGET;
  tempArray[count] = tempF;
  count++;
  if (count == 2)
  {
    count = 0;
  }
  // Makes it so only new temperature readings are displayed
  if (tempArray[0] != tempArray[1] || targetArray[0] != targetArray[1])
  {
    lcd.clear();
    lcd.print("F: ");
    lcd.print(tempF);
    lcd.setCursor(0,2);
    lcd.print("Target: ");
    lcd.print(TARGET);
  }
  // Turn the relay ON if the temperature is below target
  if (tempF < TARGET)
  {
    // Displays "Relay On" only once
    if (relayOn == false)
    {
      //Serial.println("Relay is ON");
      relayOn = true;
    }
    digitalWrite(RELAY_PIN, HIGH);
  }
  else
  {
    if (relayOn == true)
    {
      relayOn = true;
    }
    digitalWrite(RELAY_PIN, LOW); 
    relayOn == false;
  }

  //Check analog values from LCD Keypad Shield
  int x = analogRead (0);
  if (x < 100) {
    //Right
    SafeBLon(BACKLIGHT_PIN);
    lcd.display();
  } else if (x < 200) {
   	//Up
    TARGET += .5; 
  } else if (x < 400){
    //Down
    TARGET -= .5; 
  } else if (x < 600){
    //Left
    SafeBLoff(BACKLIGHT_PIN);
    lcd.noDisplay();
  } else if (x < 800){
    //Select
  }
}