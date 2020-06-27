// Libraries
#include <SdFat.h>    // Sd-Card
#include <RTClib.h>   // Real-time clock
#include <Wire.h>     // I2C
#include <LiquidCrystal_I2C.h>  // LCD

// Setup RTC
RTC_PCF8523 rtc;

// Setup LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);   //16 characters wide by 2 characters tall

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
  unsigned int prev;  // Previous value
  float value;        // Calculated value
};

sample TPS = {"TPS", 2, 0, 100.0};   // Voltage range 0.5 - 4.5 VDC
sample AFR = {"AFR", 3, 0, 14.7};    // Voltage range 0.0 - 5.0 VDC
sample TMP = {"TMP", 1, 0, 25.0};     //

const int ledRecord = 6;
const int pinSwitch = 7;
const int ledRead =  8;
const int ledError = 9;
const int pinSD = 10;

unsigned long currentMillis;  // Used for storing the latest time
unsigned long previousMillis = 0;      // Store the last time the program ran
const unsigned long interval = 50;     // Sample frequency (milliseconds)

// SPST Switch
int pinState;

// SD-Card
String filename;
char _header[24];
SdFat SD;
#define SPI_SPEED SD_SCK_MHZ(50)  // 50 is the highest, 4 is the lowest
File dataFile;

void setup() {
  // Start serial
  Serial.begin(57600);
  while (!Serial);
  // Start LCD
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.setBacklight(HIGH);
  lcd.home ();
  lcd.print("Init");
  // Define digital IO
  pinMode(ledError, OUTPUT);
  pinMode(pinSwitch, INPUT_PULLUP);
  pinMode(ledRecord, OUTPUT);
  pinMode(ledRead, OUTPUT);
//  pinMode(pinSD, OUTPUT);
  lcd.print(".");
  // Start RTC, perform some stuff while it starts
  Serial.print("\n\nStarting RTC...");
  rtc.begin();
  lcd.print(".");
  // Show all lights as part of boot sequence
  digitalWrite(ledRecord, HIGH);
  digitalWrite(ledRead, HIGH);
  digitalWrite(ledError, HIGH);
  // Throw away analog read
  pinState = digitalRead(pinSwitch);
  TPS.raw = analogRead(TPS.pin);
  AFR.raw = analogRead(AFR.pin);
  TMP.raw = analogRead(TMP.pin);
  lcd.print(".");
  delay(500);
  digitalWrite(ledRecord, LOW);
  digitalWrite(ledRead, LOW);
  digitalWrite(ledError, LOW);
  lcd.print(".");
  Serial.println(" RTC started!");
  Serial.print("Initializing SD card...");
  if (!SD.begin(pinSD, SPI_SPEED)) {
    Serial.println("Card failed, or not present");
    digitalWrite(ledError, HIGH);
    lcd.clear();
    lcd.print("SD-Card Error!");
    _currentState = _errorState;
    return;
  }
  lcd.print(".");
  Serial.println(" SD card initialized!");
  // Test Sd Card before continuing
  // Create header for files
  sprintf(_header, "Time(ms),%s,%s,%s", TPS._name, AFR._name, TMP._name);
  // Print header for serial logging
  //Serial.println(_header);
  filename = "TEST.txt";
  dataFile = SD.open(filename, O_WRITE | O_CREAT);
  dataFile.println(_header);
  dataFile.println("Test");
  if (!dataFile) {
    Serial.println("Unable to write to Sd Card.");
    digitalWrite(ledError, HIGH);
    _currentState = _errorState;
    return;
  }
  lcd.print(".");
  dataFile.remove();
  filename = "";
  lcd.print(".");
  Serial.println("Program ready.");
  lcd.print("ready");
  delay(200);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TPS:   %");
  lcd.setCursor(0, 1);
  lcd.print("AFR:     TMP:  C");
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
        // Record previous values
        TPS.prev = TPS.raw;
        AFR.prev = AFR.raw;
        TMP.prev = TMP.raw;
        // Read Values
        TPS.raw = analogRead(TPS.pin);
        AFR.raw = analogRead(AFR.pin);
        TMP.raw = analogRead(TMP.pin);

        // Calculate Values
        TPS.value = (TPS.raw * (100 / 1023.0));    // TPS ADC
        AFR.value = (AFR.raw * (10.0 / 1023.0)) + 10.0;  // AFR ADC
        TMP.value = (((TMP.raw * 5.0) / 1024.0) - 0.5) * 100;

      //TPS.value = (TPS.raw * (4.0 / 1023.0)) + 0.5;;    // Voltage (VDC)
      //AFR.value = ((AFR.raw *5.0) / 1023.0);    // Voltage (VDC)

      // Print to Serial
      //Serial.print(currentMillis);
      //Serial.print(",");
      //Serial.print(TPS.value);
      //Serial.print(",");
      //Serial.println(AFR.value);

      case _displayState:
        //Serial.println("Current State: _displayState");
        _currentState = _displayState;
        // Update display only if values have changed
        // Display TPS Value
        if (TPS.prev != TPS.raw) {
          lcd.setCursor(4, 0);
          if (TPS.value < 9.5) {
            lcd.print(" ");
            lcd.print(" ");
            lcd.print(TPS.value, 0);
          }
          else if ((TPS.value >= 9.5) && (TPS.value < 100.0)) {
            lcd.print(" ");
            lcd.print(TPS.value, 0);
          }
          else {
            lcd.print(TPS.value, 0);
          }
        }
        // Display AFR Value
        if (AFR.prev != AFR.raw) {
          lcd.setCursor(4, 1);
          lcd.print(AFR.value, 1);
        }
        // Display TMP Value
        if (TMP.prev != TMP.raw) {
          lcd.setCursor(13, 1);
          lcd.print(TMP.value, 0);
        }
        previousMillis = currentMillis;
        if (pinState != LOW) {
          if (filename != "") {
            Serial.print("Closing file...");
            dataFile.close();
            filename = "";
            Serial.println("file closed.");
            lcd.setCursor(15, 0);
            lcd.print(" ");
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
          lcd.setCursor(15, 0);
          lcd.print("R");
        };

      case _writeState:
        //Serial.println("Current State: _writeState");
        _currentState = _writeState;
        digitalWrite(ledRecord, HIGH);
        dataFile.print(millis());
        dataFile.print(",");
        dataFile.print(TPS.raw);
        dataFile.print(",");
        dataFile.print(AFR.raw);
        dataFile.print(",");
        dataFile.println(TMP.raw);
        _currentState = _readState;
        previousMillis = currentMillis;
        break;

      case _errorState:
        Serial.println("Current State: _errorState");
        digitalWrite(ledError, HIGH);
        _currentState = _errorState;
        while(1);   // Prevent flood of serial prints.
    }
  }
}
