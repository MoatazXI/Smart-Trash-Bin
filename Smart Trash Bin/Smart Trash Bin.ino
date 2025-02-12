#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include <NewPing.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include "time.h"


// WiFi credentials
const char *ssid = "Wokwi-GUEST";
const char *password = "";

// ThingSpeak API credentials
const char *server = "http://api.thingspeak.com/update";
const char *apiKey = "RXJA928FH9JF1QMT";

// ntp server for credentials for time/date handling
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7200; // GMT offset in seconds (e.g., +1 hour = 3600 seconds)



// Define pins for components
const int redLedPin = 4;
const int greenLedPin = 0;
const int servoPin = 13;
const int outsideUS_triggerPin = 26;
const int outsideUS_echoPin = 25;
const int insideUS_triggerPin = 33;
const int insideUS_echoPin = 32;
const int CS_PIN = 5; // Chip select pin for SD card
int lcdColumns = 16;
int lcdRows = 2;
Servo servo;

// // LCD and Servo setup
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// // File for SD card logging
// File logFile;

// Ultrasonic sensors
NewPing pingOutsideSensor(outsideUS_triggerPin, outsideUS_echoPin);
NewPing pingInsideSensor(insideUS_triggerPin, insideUS_echoPin);



// Function to connect to WiFi
void connect_wifi()
{
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password, 6);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("waiting to connect ...");
  }
  Serial.println("\nConnected to WiFi!");
}

// // Initialize SD card
void connect_sd()
{
  Serial.print("Initializing SD card...");
  if (!SD.begin(CS_PIN))
  {
    Serial.println("Card initialization failed!");
    while (true)
      ; // Halt the system if SD fails
  }
  Serial.println("SD card initialized.");
}

// Log events to serial console
void logEvent(const String &event)
{
    Serial.println("Logged: " + event);
    printCurrentTime();
  
}


// Servo control functions
void openServo()
{
  servo.write(90);
  logEvent("Servo opened at ");
}

void closeServo()
{
  servo.write(0);
  logEvent("Servo closed at ");
}
// to control led , turn on/off
void turnOnGreenLed()
{
  digitalWrite(greenLedPin, HIGH);
  logEvent("Green Led ON ");

}
void turnOffGreenLed()
{
  digitalWrite(greenLedPin, LOW);
  logEvent("Green Led OFF ");

}
void turnOnRedLed()
{
  digitalWrite(redLedPin, HIGH);
  logEvent("Red Led ON ");

}
void turnOffRedLed()
{
  digitalWrite(redLedPin, LOW);
  logEvent("Red Led Off ");

}



void printCurrentTime() {
    struct tm timeinfo; // Structure to hold time data
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}


//Send data to ThingSpeak
void sendDataToThingSpeak(float outsideUltrasonic, float insideUltrasonic, int servoStatus)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    String url = String(server) + "?api_key=" + apiKey +
                 "&field1=" + String(outsideUltrasonic) +
                 "&field2=" + String(insideUltrasonic) +
                 "&field3=" + servoStatus;
    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
      Serial.println("Data sent to ThingSpeak: " + String(httpResponseCode));
    }
    else
    {
      Serial.println("Failed to send data: " + http.errorToString(httpResponseCode));
    }

    http.end();
  }
  else
  {
    Serial.println("WiFi disconnected, cannot send data.");
  }
}

// Measure distance with ultrasonic sensors
float readDistanceFromOutsideBin()
{
  return pingOutsideSensor.ping_cm();
}

float readDistanceFromInsideBin()
{
  return pingInsideSensor.ping_cm();
}


void setup()
{
  // Serial and WiFi initialization
  Serial.begin(9600); 
  connect_wifi();

  
  connect_sd(); // SD card initialization

  // LCD initialization
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);



  // Pin initialization
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  servo.attach(servoPin);

  configTime(gmtOffset_sec, 0, ntpServer);

}


void loop()
{
  // Simulate distance data
  int outsideUltrasonicDistance = readDistanceFromOutsideBin();
  int insideUltrasonicDistance = readDistanceFromInsideBin();
 
  // no one is near
  if (outsideUltrasonicDistance > 15) {
      lcd.setCursor(0, 0);
      lcd.print("Waiting...");
      logEvent("Waiting.....");
      turnOffGreenLed();
      turnOffRedLed();
      lcd.clear();
      delay(10000);
  }

  //  someone here and the bin is free 
  if(outsideUltrasonicDistance <= 15 && insideUltrasonicDistance > 10){
      
     
      sendDataToThingSpeak(outsideUltrasonicDistance , insideUltrasonicDistance , 90);
      openServo();
      delay(2000);
      turnOnGreenLed();
      lcd.setCursor(0, 0);
      lcd.print("Opened Bin");
      delay(10000); 
      closeServo();

      sendDataToThingSpeak(outsideUltrasonicDistance , insideUltrasonicDistance , 0);
      turnOffGreenLed();
     
      lcd.clear();

  }
  // someone here but the bin is full (in threshold 90%)
  if(outsideUltrasonicDistance <= 15 && insideUltrasonicDistance < 10){
    
      sendDataToThingSpeak(outsideUltrasonicDistance , insideUltrasonicDistance , 0);
      turnOnRedLed();
      lcd.setCursor(0, 0);
      lcd.print("Bin is Full");
      logEvent("The Bin Is Full");
      delay(10000);
      turnOffRedLed();

    
      lcd.clear();
  }


  

  // Wait before next loop iteration
  delay(10000);
  Serial.println("----------------------------------");
}



