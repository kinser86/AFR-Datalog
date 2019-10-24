// Libraries
#include <SdFat.h>
#include <RTClib.h>

RTC_PCF8523 rtc;

enum state {
  _readState,
  _displayState,
  _createState,
  _openState,
  _writeState,
  _saveState
};

state _currentState;

// Declare Variables
struct sample {
  String name;
  const int pin;      // Input pin
  unsigned int raw;   // Raw value
  float value;        // Calculated value
};

sample TPS = {"TPS", 2, 0, 100.0};   // Voltage range 0.5 - 4.5 VDC
sample AFR = {"AFR", 3, 0, 14.7};    // Voltage range 0.0 - 5.0 VDC

const int pinSwitch = 7;
const int pinRead =  8;
const int pinRecord = 9;
const int pinSD = 10;

unsigned long currentMillis;  // Used for storing the latest time
long previousMillis = 0;      // Store the last time the program ran
const long interval = 50;    // Sample frequency (milliseconds)

int pinState;
int ledGreen = LOW;
int ledRed = LOW;

String filename;
SdFat SD;
File dataFile;    // SD Card

void setup() {
  // Start serial
  Serial.begin(57600);
  delay(250);
  pinMode(pinSwitch, INPUT_PULLUP);
  pinMode(pinRecord, OUTPUT);
  pinMode(pinRead, OUTPUT);
  pinMode(pinSD, OUTPUT);
  // Start RTC
  Serial.print("Starting RTC...");
  rtc.begin();
  delay(250);
  Serial.println(" RTC started!");
  Serial.print("Initializing SD card...");
  if (!SD.begin(pinSD)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println(" SD card initialized!");
  // Print header for serial logging
  // Serial.println("Time(ms), TPS, AFR");
  Serial.println("Progream ready.");
}

void loop() {
  pinState = digitalRead(pinSwitch);
  digitalWrite(pinRead, HIGH);
  digitalWrite(pinRecord, LOW);
  currentMillis = millis();

  if ((currentMillis - previousMillis) >= interval) {
    switch (_currentState) {

      case _readState:
        //Serial.println("Current State: _readState");
        digitalWrite(pinRead, LOW);   // Green LED OFF
        _currentState = _readState;
        previousMillis = currentMillis;
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
          filename = "";
          _currentState = _readState;
          break;
        };

      case _createState:
        //Serial.println("Current State: _createState");
        digitalWrite(pinRecord, HIGH);
        _currentState = _createState;
        previousMillis = currentMillis;
        if (filename == NULL) {
          //Serial.print("Creating new file... ");
          DateTime now = rtc.now();
          filename = String(now.unixtime(), DEC);
          filename = filename + ".txt";
          //Serial.print(filename);
          //Serial.println(" created!");
          //Serial.print("Writing header to file... ");
          //dataFile = SD.open(filename, FILE_WRITE);
          dataFile = SD.open(filename, O_WRITE | O_CREAT);
          dataFile.println("Time(ms), TPS, AFR");
          //Serial.println("header written!");
          dataFile.close();
        };

      case _openState:
        //Serial.println("Current State: _openState");
        _currentState = _openState;
        dataFile = SD.open(filename, O_CREAT | O_APPEND | O_WRITE);     // Open filename.txt

      case _writeState:
        //Serial.println("Current State: _writeState");
        _currentState = _writeState;
        // if the file is available, write to it:
        if (dataFile) {
          String _stringMillis = String(millis());
          String _stringTPS = String(TPS.raw);
          String _stringAFR = String(AFR.raw);
          String _data = _stringMillis + "," + _stringTPS + "," + _stringAFR;
          dataFile.println(_data);
        }
        // if the file didn't open, print an error:
        else {
          Serial.print("error opening ");
          Serial.println(filename);
        }

      case _saveState:
        //Serial.println("Current State: _saveState");
        _currentState = _saveState;
        dataFile.flush();
        dataFile.close();
        _currentState = _readState;
        break;
    }
  }
}
