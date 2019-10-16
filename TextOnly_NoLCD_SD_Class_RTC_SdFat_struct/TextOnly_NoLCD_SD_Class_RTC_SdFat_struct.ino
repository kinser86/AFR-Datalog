/*  Description:
 The purpose of this program is to test the speed of the underlying script.
 */

// Libraries
//#include <SD.h>
#include <SdFat.h>
#include <RTClib.h>

RTC_Millis rtc;
SdFat SD;

// Declare Variables
struct sample{
  const int pin;      // Input pin
  unsigned int raw;   // Raw value
  float value;        // Calculated value
};

sample TPS = {2, 0, 100.0};   // Voltage range 0.5 - 4.5 VDC
sample AFR = {3, 0, 14.7};    // Voltage range 0.0 - 5.0 VDC

const int PIN_SWITCH = 7;
const int PIN_READ =  8;
const int PIN_RECORD = 9;
const int PIN_SD = 10;

unsigned long currentMillis;  // Used for storing the latest time
long previousMillis = 0;      // Store the last time the program ran
const long interval = 100;    // Sample frequency (milliseconds)

int PIN_STATE;
int STATE, PREV_STATE;
int LED_READ = LOW;    // LED_READ used to set the LED
int LED_RECORD = LOW;

String filename;

File dataFile;    // SD Card

void setup() {
  Serial.begin(57600);   // Initiate serial
  Serial.println("Initializing SD card...");
  pinMode(PIN_SWITCH, INPUT_PULLUP);
  pinMode(PIN_RECORD, OUTPUT);
  pinMode(PIN_READ, OUTPUT);
  pinMode(PIN_SD, OUTPUT);
  
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));

  // See if the card is present and can be initialized:
  if (!SD.begin(PIN_SD)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.print("initialization done.");
  Serial.println("Time(ms), TPS, AFR");   // Header for CSV file
}

void loop() {
  // Read to see if the switch has changed states
  PIN_STATE = digitalRead(PIN_SWITCH);

  currentMillis = millis();

  // State 0 - Idle
  if(currentMillis - previousMillis < interval) {
    STATE = STATE;   // State flag set
    led_status(HIGH, LOW);
    digitalWrite(PIN_READ, LED_READ);
  }

  // State 1 - Read Only
  if((PIN_STATE == HIGH) && (currentMillis - previousMillis >= interval)) {
    STATE = 1;   // State flag set
    Serial.println(STATE);
    previousMillis = currentMillis;  // Remember the time
    led_status(LOW, LOW);  // Update LEDs
    read();  // Read the pins
  }

  //State 2 - Read and Record
  else if((PIN_STATE == LOW) && (currentMillis - previousMillis >= interval)){
    previousMillis = currentMillis;  // Remember the time
    led_status(LOW, HIGH);  // Update LEDs
    read();   // Read the pins
    
    if (STATE < 2){   // Was a file already created?    
      Serial.print("..Create new file: ");
      DateTime now = rtc.now();
      filename = String(now.unixtime(), DEC);
      filename = filename + ".txt";
      Serial.println(filename);
      // Create the header
      dataFile = SD.open(filename, FILE_WRITE);
      dataFile.println("Time(ms), TPS, AFR");
      close();
    }
    filename = filename;
    STATE = 2;   // State flag set
    Serial.println(STATE + filename);
   
    open();   // Open the file
    write();  // Write to the SD Card
    close();  // Close the file
  }
}

void read(){
  //Read Values
  TPS.raw = analogRead(TPS.pin);
  AFR.raw = analogRead(AFR.pin);

  //Calculate Values
  TPS.value = (TPS.raw * (4.0 / 1023.0)) + 0.5;    // TPS ADC
  //TPS_VAL = ((TPS_RAW * 5.0) / 1023.0);    // Voltage (VDC)
  AFR.value = (AFR.raw * (10.0 / 1023.0)) + 10.0;  // AFR ADC
  //AFR_VAL = ((AFR_RAW *5.0) / 1023.0);    // Voltage (VDC)

  //Print to Serial
  //Serial.print(currentMillis);
  //Serial.print(",");
  //Serial.print(TPS_VAL);
  //Serial.print(",");
  //Serial.println(AFR_VAL);
}

// Open the file now
void open(){
  dataFile = SD.open(filename, FILE_WRITE);
}

// Write to the file now  
void write(){
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(millis());
    dataFile.print(",");
    dataFile.print(TPS.raw);
    dataFile.print(",");
    dataFile.println(AFR.raw);
  }
  // if the file didn't open, print an error:
  else {
    //Serial.println("error opening test.txt");
  }
}

// Close the file now
void close(){
  dataFile.close();  // close the file:
  //Serial.println("done.");
}

void led_status(int led_one, int led_two){
  LED_READ = led_one;
  LED_RECORD = led_two;
  digitalWrite(PIN_READ, LED_READ);
  digitalWrite(PIN_RECORD, LED_RECORD); 
}
