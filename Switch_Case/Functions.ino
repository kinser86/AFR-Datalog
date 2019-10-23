/*
 *  Analog Read Function
 */
void read() {
  //Read Values
  TPS.raw = analogRead(TPS.pin);
  AFR.raw = analogRead(AFR.pin);

  //Calculate Values
  TPS.value = (TPS.raw * (4.0 / 1023.0)) + 0.5;    // TPS ADC
  //TPS_VAL = ((TPS_RAW * 5.0) / 1023.0);    // Voltage (VDC)
  AFR.value = (AFR.raw * (10.0 / 1023.0)) + 10.0;  // AFR ADC
  //AFR_VAL = ((AFR_RAW *5.0) / 1023.0);    // Voltage (VDC)

  //Print to Serial
  Serial.print(currentMillis);
  Serial.print(",");
  Serial.print(TPS.value);
  Serial.print(",");
  Serial.println(AFR.value);
}
/* 
 *  SdCard Functions
 */
 // Open the file
void open() {
  dataFile = SD.open(filename, FILE_WRITE);
}

// Write to the file
void write() {
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

// Close the file
void close() {
  dataFile.close();  // close the file:
}
