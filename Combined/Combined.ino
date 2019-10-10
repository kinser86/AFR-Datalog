// Libraries
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

#define length 12.0 // Define how many columns for the bar

//Custom percent characters
byte p1[8] = {
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10};

byte p2[8] = {
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18};

byte p3[8] = {
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C};

byte p4[8] = {
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E};

byte p5[8] = {
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F};
  
//Custom column characters
byte c1[8] = {
  0x00,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x00};
  
byte c2[8] = {
  0x00,
  0x08,
  0x08,
  0x08,
  0x08,
  0x08,
  0x08,
  0x00};
  
byte c3[8] = {
  0x00,
  0x04,
  0x04,
  0x04,
  0x04,
  0x04,
  0x04,
  0x00};
  
byte c4[8] = {
  0x00,
  0x02,
  0x02,
  0x02,
  0x02,
  0x02,
  0x02,
  0x00};
  
byte c5[8] = {
  0x00,
  0x01,
  0x01,
  0x01,
  0x01,
  0x01,
  0x01,
  0x00};

// Declare Variables
long previousMillis = 0;    // Store the last time the program ran
long interval = 100;        // Sample Frequency (milliseconds)

unsigned int TPS_RAW = 0;  // Initial value for TPS_RAW
float TPS_BAR = 100.0;     // Initial value for TPS_BAR
float TPS_VAL = 0;         // Range: 0 VDC - 4.5 VDC
float TPS_a;
unsigned int AFR_RAW = 0;  // Initial value for AFR_RAW
float AFR_BAR = 100.0;     // Initial value for AFR_BAR
float AFR_VAL = 14.7;      // Range 10.0 - 20.0
float AFR_a;

unsigned char TPS_b, AFR_b;
unsigned int TPS_segment, AFR_segment;

void setup()   {
  lcd.begin(16, 2); //Must be the first line!
  
  Serial.begin(57600);   // Initiate serial
  lcd.setBacklight(YELLOW);  
  
  //Create custom characters
  lcd.createChar(0, p1);
  lcd.createChar(1, p2);
  lcd.createChar(2, p3);
  lcd.createChar(3, p4);
  lcd.createChar(4, p5);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > interval) {
    // save the last time the program executed
    previousMillis = currentMillis;
  
    //Read Values
    TPS_RAW = analogRead(3);
    AFR_RAW = analogRead(2);
  
    //Calculate Values
    TPS_BAR = TPS_RAW / 1023.0 * 100.0;            // Used for generating the TPS bar
    TPS_VAL = (TPS_RAW * (4.0 / 1024.0)) + 0.5;    // TPS ADC
    AFR_BAR = AFR_RAW / 1023.0 * 100.0;            // Used for generating AFR bar
    AFR_VAL = (AFR_RAW * (10.0 / 1024.0)) + 10.0;  // AFR ADC
  
    Serial.print(TPS_VAL);
    Serial.print(",");
    Serial.println(AFR_VAL);
  
    // TPS INFORMATION - TOP ROW OF LCD
    // Print top row of LCD
    lcd.setCursor(0,0);
    if (TPS_BAR < 9.5){
      lcd.print("  ");
      lcd.print(TPS_BAR,0); 
    }
    else if (TPS_BAR >= 9.5 && TPS_BAR < 99.5){
      lcd.print(" ");
      lcd.print(TPS_BAR,0); 
    }
    else {
      lcd.print(TPS_BAR,0); 
    }
    lcd.print("%");
  
  TPS_a = length / 100 * TPS_BAR;
  
   // Draw black rectangles on LCD top row
    if (TPS_a >= 1) {
      for (int i=1; i <= TPS_a; i++) {
        lcd.write(char(4));
        TPS_b = i;
      }
      TPS_a = TPS_a - TPS_b;
    }
    TPS_segment = TPS_a * 5;
  
  // Draw top row segments
    switch (TPS_segment) {
    case 0:
      //Serial.println("TPS_CASE: 0");
      break;
    case 1:
      //Serial.println("TPS_CASE: 1");
      lcd.write(char(0));
      break;
    case 2:
      //Serial.println("TPS_CASE: 2");
      lcd.write(char(1));
      break;
    case 3:
      //Serial.println("TPS_CASE: 3");
      lcd.write(char(2));
      break;
    case 4:
      //Serial.println("TPS_CASE: 4");
      lcd.write(char(3));
      break;
    }

  // Clear line
  for (int i=0; i<(length-TPS_b); i++) {
    lcd.print(" ");
  };

  // AFR INFORMATION - BOTTOM ROW
  // Print bottom row of LCD
  lcd.setCursor(0,1);
  lcd.print(AFR_VAL,1);
  lcd.setCursor(4,1);
  
  AFR_a = length / 100 * AFR_BAR;
      
   // Draw black rectangles on LCD bottom row
    if (AFR_a >= 1) {
      for (int i = 1; i <= AFR_a; i++) {
        lcd.write(char(4));
        AFR_b = i;
      }
      AFR_a = AFR_a - AFR_b;
    }
    AFR_segment = AFR_a * 5;
  
  // Draw bottom row segments
    switch (AFR_segment) {
    case 0:
      //Serial.print("AFR_CASE: 0");
      break;
    case 1:
      //Serial.print("AFR_CASE: 1");
      lcd.write(char(0));
      break;
    case 2:
      //Serial.print("AFR_CASE: 2");
      lcd.write(char(1));
      break;
    case 3:
      //Serial.print("AFR_CASE: 3");
      lcd.write(char(2));
      break;
    case 4:
      //Serial.print("AFR_CASE: 4");
      lcd.write(char(3));
      break;
    }

//clearing line
  for (int i=0; i<(length-AFR_b); i++) {
    lcd.print(" ");
  };
  }
}
