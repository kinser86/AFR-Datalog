# AFR-Datalog
AFR data-logger written for Arduino that records the throttle position sensor (TPS) and air-fuel ratio (AFR) at 20-Hz. The data is analyzed in the File_AFR_GUI.py program which converts the raw 10-bit data into readable information. 

# Arduino
## Pinouts
* Board - [Adafruit Data Logger Shield](https://learn.adafruit.com/adafruit-data-logger-shield/overview)
  * RTC Clock
    * IORef
    * GND
    * SCL
    * SDA
  * SD Card
    * Digital 10
    * VCC
    * MOSI
    * GND
    * MISO
    * SCK
* Analog Inputs
  * A3 - TPS Signal
  * A4 - AFR Signal
* Sensors
  * TPS: [Amphenol Piher Sensing Systems, SENSOR ANGLE 90DEG WIRE LEADS, PSC360G2-F1A-C0011-ERA090-05K](https://www.digikey.com/product-detail/en/amphenol-piher-sensing-systems/PSC360G2-F1A-C0011-ERA090-05K/1993-1007-ND/9555845)
  * AFR: [Spartan Lambda Controller 2](https://www.14point7.com/products/spartan-lambda-controller-2)
  * TMP: [TMP36 Temperature Sensor](https://cdn.sparkfun.com/datasheets/Sensors/Temp/TMP35_36_37.pdf)
## Output
Generates a csv file named with the unix timestamp of when the file was created. Converting the time will provide the exact time the datalogging started.

| Time(ms) | TPS | AFR | TMP |
| :---: | :---: | :---: | :---: |
| millis() | Raw 10-bit Value | Raw 10-bit Value | Raw 10-bit Value

* The time column contains the current millisecond, measured from when the program started to when the measurement was made. The actual time in milliseconds isn't as important as the time difference between samples (Sample<sub>n+1</sub> - Sample<sub>n</sub>). Ideally, the difference between Sample<sub>n+1</sub> and Sample<sub>n</sub> should be = 50-ms.
* The TPS column contains the 10-bit value of the throttle position sensor.
  * Translate the TPS 10-bit value into percent(%):
    * TPS.value = TPS.raw * (100.0 / 1023.0)
  * Translate the TPS 10-bit value into voltage(VDC)
    * TBD
* The AFR column contains the 10-bit value of the air-fuel ratio.
  * Translate the AFR 10-bit value into AFR:
    * AFR.value = AFR.raw * (10.0 / 1023.0)) + 10.0)
  * Translate the AFR 10-bit value into voltage(VDC):
    * TBD
* The TMP column contains the 10-bit value of the temperature.
  * Translate the TMP 10-bit value into Degree C:
    * TMP.value = (((TMP.raw * 5.0) / 1024.0) - 0.5) * 100;

# AFR_GUI.py
An interactive program designed to help review the data collect utilizing pyqtgraph and Qt5 Designer.

# References
* pyqtgraph | [Website](http://www.pyqtgraph.org) | [Github Repositoty](https://github.com/pyqtgraph/pyqtgraph)
* Qt Designer | [Manual](https://doc.qt.io/qt-5/qtdesigner-manual.html)
  * [Build Reference](https://www.tutorialspoint.com/pyqt/pyqt_using_qt_designer.htm)
