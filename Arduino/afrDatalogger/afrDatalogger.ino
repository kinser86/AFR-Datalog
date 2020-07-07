// Libraries
#include <SdFat.h>    // Sd-Card
#include <RTClib.h>   // Real-time clock
#include <Wire.h>     // I2C
#include <LiquidCrystal_I2C.h>  // LCD
#include <FastLED.h>    // Neopixel

// Setup RTC
RTC_PCF8523 rtc;

// Setup LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);   //16 characters wide by 2 characters tall

// Setup NeoPixel
#define numLeds 8   // Number of available LEDs
#define neoPin 24   // Digital Pin Output

CRGB neoLeds[numLeds];

enum state {
  _idleState,   // Wait for overflow
  _readState,   // Read analog values
  _logState,    // Write data to file
  _errorState   // Stop the program if something is wrong
};

state _currentState;

// Declare Variables
struct sample {
  char const *_name;
  String const longName; // Long name
  const int pin;      // Input pin
  unsigned int raw;   // Raw value
  unsigned int prev;  // Previous value
  float value;        // Calculated value
};

sample TMP = {"TMP", "Temperature", 8, 0, 0, 25.0};    //
sample TPS = {"TPS", "Throttle Position", 9, 0, 0, 100.0};   // Voltage range 0.5 - 4.5 VDC
sample AFR = {"AFR", "Air-Fuel Ratio", 10, 0, 0, 14.7};   // Voltage range 0.0 - 5.0 VDC
sample VEL = {"VEL", "Velocity", 2, 0, 0, 0};

const int ledRecord = 7;  //LED
const int ledRead =  0;   //LED
const int ledError = 1;   //LED

const int pinSwitch = 7;
const int pinSD = 10;   // Sd-Card Chip Select

// Speed Counter
volatile unsigned long countVel, countDisp;

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

  // RPM counter
  attachInterrupt(digitalPinToInterrupt(VEL.pin), speedCount, CHANGE);

  // Start NeoPixel
  FastLED.addLeds<NEOPIXEL, neoPin>(neoLeds, numLeds);
  FastLED.clear();  // Clear the pixels

  // Start LCD
  lcd.init();   // initialize the lcd
  lcd.backlight();
  lcd.setBacklight(HIGH);
  lcd.home ();
  lcd.print("Init");
  // Define digital IO
  pinMode(pinSwitch, INPUT_PULLUP);
  lcd.print(".");
  neoLeds[0].setRGB( 50, 0, 0);
  FastLED.show();
  // Start RTC, perform some stuff while it starts
  Serial.print("\n\nStarting RTC...");
  rtc.begin();
  lcd.print(".");
  neoLeds[2].setRGB( 0, 50, 0);
  FastLED.show();
  // Throw away analog read
  pinState = digitalRead(pinSwitch);
  TPS.raw = analogRead(TPS.pin);
  AFR.raw = analogRead(AFR.pin);
  TMP.raw = analogRead(TMP.pin);
  lcd.print(".");
  neoLeds[4].setRGB( 0, 0, 50);
  FastLED.show();
  delay(500);
  // Clear the NeoPixels
  for (int i = 0; i < numLeds; i++) {
    neoLeds[i].setRGB( 0, 0, 0);
  }
  FastLED.show();
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
  sprintf(_header, "Time(ms),%s,%s,%s,%s", TPS._name, AFR._name, TMP._name, VEL._name);
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
  lcd.print("TPS:   % VEL:");
  lcd.setCursor(0, 1);
  lcd.print("AFR:     TMP:  C");
}

void loop() {
  switch (_currentState) {

    case _idleState:
      _currentState = _idleState;
      //      stateFunc();
      idleFunc();
      break;

    case _readState:
      _currentState = _readState;
      //      stateFunc();
      readFunc();
      speedFunc();
      displayFunc();
      _currentState = _idleState;
      break;

    case _logState:
      _currentState = _logState;
      //      stateFunc();
      readFunc();
      speedFunc();
      displayFunc();
      createFunc();
      writeFunc();
      _currentState = _idleState;
      break;

    case _errorState:
      _currentState = _errorState;
      //      stateFunc();
      errorFunc();
  }
}

void idleFunc() {
  while ((millis() - previousMillis) < interval) {
    pinState = digitalRead(pinSwitch);
  }
  previousMillis += interval;   // Makes a constant sample!
  if (!pinState) {
    _currentState = _logState;
  }
  else {
    _currentState = _readState;
  }
}

void readFunc(void) {
  neoLeds[ledRead].setRGB( 0, 0, 50);
  FastLED.show();
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

  neoLeds[ledRead].setRGB( 0, 0, 0);
  FastLED.show();
}

void displayFunc(void) {
  // Update display only if values have changed
    // Display TPS Value
    if (TPS.prev != TPS.raw) {
      lcd.setCursor(4, 0);
      // Format for number of characters to be displayed
      if (TPS.value < 9.5) {
        lcd.print("  ");
      }
      else {
        lcd.print(" ");
      }
      lcd.print(TPS.value, 0);
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
    // Display Velocity
    if (VEL.raw == 0){
      lcd.setCursor(13, 0);
      lcd.print("  0");
    }
    else if (VEL.prev != VEL.raw) {
      lcd.setCursor(13, 0);
      if (VEL.value < 9.5) {
        lcd.print("  ");
      }
      else {
        lcd.print(" ");
      }
      lcd.print(VEL.value, 0);
    }
    if (pinState) {
      if (filename != "") {
        Serial.print("Closing file...");
        dataFile.close();
        filename = "";
        Serial.println("file closed.");
        lcd.setCursor(8, 0);
        lcd.print(" ");
      }
    }
}

void createFunc(void) {
  if (filename == "") {
    DateTime now = rtc.now();
    filename = String(now.unixtime(), DEC);
    filename = filename + ".txt";
    Serial.print(filename);
    Serial.println(" created!");
    dataFile = SD.open(filename, O_CREAT | O_APPEND | O_WRITE);     // Open file
    dataFile.println(_header);
    lcd.setCursor(8, 0);
    lcd.print("~");
  }
}

void writeFunc(void) {
  neoLeds[ledRecord].setRGB( 0, 50, 0);
  FastLED.show();
  dataFile.print(previousMillis);
  dataFile.print(",");
  dataFile.print(TPS.raw);
  dataFile.print(",");
  dataFile.print(AFR.raw);
  dataFile.print(",");
  dataFile.print(TMP.raw);
  dataFile.print(",");
  dataFile.println(VEL.raw);
  neoLeds[ledRecord].setRGB( 0, 0, 0);
  FastLED.show();
}

void errorFunc(void) {
  Serial.println("Current State: _errorState");
  errorLed();
  while (1);  // Prevent flood of serial prints.
}

void stateFunc(void) {
  //  if (_prevState != _currentState) {
  //    Serial.print("Current State: ");
  //    Serial.println(_currentState);
  //  }
}

void errorLed(void) {
  for (int i = 0; i < numLeds; i++) {
    neoLeds[i].setRGB(50, 0, 0);
  }
  FastLED.show();
}

void speedCount(void){
  countVel++;
}

void speedFunc(void) {
  // Since the sample frequency is 20hz, 20 samples is 1 second.
  if (countDisp > 20){
    VEL.prev = VEL.raw;
    VEL.raw = countVel;
    VEL.value = ((VEL.raw * 60 * 60 * 3.14 * 2)/63360);
    countVel = 0;
    countDisp = 0; 
  }
  else {
    countDisp++;
  }
}