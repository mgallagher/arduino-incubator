
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <Arduino.h>

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 4000;           // interval at which to blink (milliseconds)

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
  sensors.setResolution(10);
  Serial.println(sensors.getResolution());
}

void loop()
{
  // here is where you'd put code that needs to be running all the time.
    //Check analog values from LCD Keypad Shield
  int x = analogRead (0);
  switch(x)
  {
    // Right
    case 0 ... 100:
      SafeBLon(BACKLIGHT_PIN);
      lcd.display();
      break;
    // Up
    case 101 ... 200:
      TARGET += .5;
      break;
    // Down
    case 201 ... 400:
      TARGET -= .5;
      break;
    // Left
    case 401 ... 600:
      SafeBLoff(BACKLIGHT_PIN);
      lcd.noDisplay();
      break;
  }
  // if (x < 100) {
  //   //Right
  //   SafeBLon(BACKLIGHT_PIN);
  //   lcd.display();
  // } else if (x < 200) {
  //   //Up
  //   TARGET += .5; 
  // } else if (x < 400){
  //   //Down
  //   TARGET -= .5; 
  // } else if (x < 600){
  //   //Left
  //   SafeBLoff(BACKLIGHT_PIN);
  //   lcd.noDisplay();
  // } else if (x < 800){
  //   //Select
  // }
  // check to see if it's time to blink the LED; that is, if the 
  // difference between the current time and last time you blinked 
  // the LED is bigger than the interval at which you want to 
  // blink the LED.
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    // // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // // if the LED is off turn it on and vice-versa:
    // if (ledState == LOW)
    //   ledState = HIGH;
    // else
    //   ledState = LOW;
    
    // set the LED with the ledState of the variable:
    unsigned long before = millis();
    sensors.requestTemperatures(); // Send the command to get temperatures
    
    float tempF = sensors.requestTemperaturesByAddress(0);
    unsigned long after = millis();
    targetArray[count] = TARGET;
    tempArray[count] = tempF;
    count++;
    if (count == 2)
    {
      count = 0;
    }
    // Makes it so only new temperature readings are displayed
    // if (tempArray[0] != tempArray[1] || targetArray[0] != targetArray[1])
    // {
      lcd.clear();
      lcd.print("F: ");
      lcd.print(tempF);
      lcd.setCursor(0,2);
      lcd.print("Target: ");
      lcd.print(TARGET);

      
      Serial.println(after - before);
    // }
  }
}