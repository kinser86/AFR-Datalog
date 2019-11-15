// Libraries
#include <SdFat.h>
#include <RTClib.h>

RTC_PCF8523 rtc;

enum state {
  _readState,     // Read analog values
  _displayState,  // Display values on LCD
  _createState,   // Create a file for datalogging
  _writeState,    // Write data to file
  _errorState     // Stop the program if something is wrong
};

state _currentState;

// Declare Variables
struct sample {
  char* _name;
  const int pin;      // Input pin
  unsigned int raw;   // Raw value
  float value;        // Calculated value
};

sample TPS = {"TPS", 2, 0, 100.0};   // Voltage range 0.5 - 4.5 VDC
sample AFR = {"AFR", 3, 0, 14.7};    // Voltage range 0.0 - 5.0 VDC

const int ledRecord = 6;
const int pinSwitch = 7;
const int ledRead =  8;
const int ledError = 9;
const int pinSD = 10;

unsigned long currentMillis;  // Used for storing the latest time
unsigned long previousMillis = 0;      // Store the last time the program ran
const unsigned long interval = 50;     // Sample frequency (milliseconds)

int pinState;

String filename;
char _header[20];
SdFat SD;
File dataFile;    // SD Card

void setup() {
  // Start serial
  Serial.begin(57600);
  while (!Serial);
  pinMode(ledError, OUTPUT);
  pinMode(pinSwitch, INPUT_PULLUP);
  pinMode(ledRecord, OUTPUT);
  pinMode(ledRead, OUTPUT);
  pinMode(pinSD, OUTPUT);
  // Start RTC
  Serial.print("\n\nStarting RTC...");
  rtc.begin();
  // Show all lights as part of boot sequence
  digitalWrite(ledRecord, HIGH);
  digitalWrite(ledRead, HIGH);
  digitalWrite(ledError, HIGH);
  // Throw away analog read
  pinState = digitalRead(pinSwitch);
  TPS.raw = analogRead(TPS.pin);
  AFR.raw = analogRead(AFR.pin);
  delay(500);
  digitalWrite(ledRecord, LOW);
  digitalWrite(ledRead, LOW);
  digitalWrite(ledError, LOW);
  Serial.println(" RTC started!");
  Serial.print("Initializing SD card...");
  if (!SD.begin(pinSD)) {
    Serial.println("Card failed, or not present");
    digitalWrite(ledError, HIGH);
    _currentState = _errorState;
    return;
  }
  Serial.println(" SD card initialized!");
  // Test Sd Card before continuing
  // Create header for files
  sprintf(_header, "Time(ms),%s,%s",TPS._name,AFR._name);
  // Print header for serial logging
  //Serial.println(_header);
  filename = "TEST.txt";
  dataFile = SD.open(filename, O_WRITE | O_CREAT);
  dataFile.println(_header);
  dataFile.println("Test");
  if (!dataFile){
    Serial.println("Unable to write to Sd Card.");
    digitalWrite(ledError, HIGH);
    _currentState = _errorState;
    return;
  }
  dataFile.remove();
  filename = "";
  Serial.println("Program ready.");
}

void loop() {
  pinState = digitalRead(pinSwitch);
  digitalWrite(ledRead, HIGH);
  digitalWrite(ledRecord, LOW);
  currentMillis = millis();

  if ((currentMillis - previousMillis) >= interval) {
    switch (_currentState) {

      case _readState:
        //Serial.println("Current State: _readState");
        _currentState = _readState;
        digitalWrite(ledRead, LOW);   // Green LED OFF
        //Read Values
        TPS.raw = analogRead(TPS.pin);
        AFR.raw = analogRead(AFR.pin);

      // Calculate Values
      //TPS.value = (TPS.raw * (4.0 / 1023.0)) + 0.5;    // TPS ADC
      //TPS.value = ((TPS_RAW * 5.0) / 1023.0);    // Voltage (VDC)
      //AFR.value = (AFR.raw * (10.0 / 1023.0)) + 10.0;  // AFR ADC
      //AFR.value = ((AFR_RAW *5.0) / 1023.0);    // Voltage (VDC)

      // Print to Serial
      //Serial.print(currentMillis);
      //Serial.print(",");
      //Serial.print(TPS.value);
      //Serial.print(",");
      //Serial.println(AFR.value);

      case _displayState:
        //Serial.println("Current State: _displayState");
        _currentState = _displayState;
        previousMillis = currentMillis;
        if (pinState != LOW) {
          if (filename != ""){
            Serial.print("Closing file...");
            dataFile.close();
            filename = "";
            Serial.println("file closed.");
          }
          _currentState = _readState;
          break;
        };

      case _createState:
        //Serial.println("Current State: _createState");
        _currentState = _createState;
        if (filename == "") {
          //Serial.print("Creating new file... ");
          DateTime now = rtc.now();
          filename = String(now.unixtime(), DEC);
          filename = filename + ".txt";
          Serial.print(filename);
          Serial.println(" created!");
          //Serial.print("Writing header to file... ");
          dataFile = SD.open(filename, O_CREAT | O_APPEND | O_WRITE);     // Open file
          dataFile.println(_header);
          //Serial.println("header written!");
        };

      case _writeState:
        //Serial.println("Current State: _writeState");
        _currentState = _writeState;
        digitalWrite(ledRecord, HIGH);
        dataFile.print(millis());
        dataFile.print(",");
        dataFile.print(TPS.raw);
        dataFile.print(",");
        dataFile.println(AFR.raw);
        _currentState = _readState;
        previousMillis = currentMillis;
        break;

      case _errorState:
        Serial.println("Current State: _errorState");
        digitalWrite(ledError, HIGH);
        _currentState = _errorState;
    }
  }
}