# AFR-Datalog
AFR data-logger written for Arduino

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
## Output
Generates a csv file, named with the unix timestamp of when the file was created.

| Time(ms) | AFR | TPS |
| -------- | --- | --- |
| millis() | Raw 10-bit Value | Raw 10-bit Value |

* The time column contains the current millisecond, measured from when the program started to when the measurement was made.
* The AFR column contains the 10-bit value of the air-fuel ratio.
  * Translate the AFR 10-bit value into AFR:
  * Translate the AFR 10-bit value into voltage(VDC):
* The TPS column contains the 10-bit value of the throttle position sensor.
  * Translate the TPS 10-bit value into percent(%):
  * Translate the TPS 10-bit value into voltage(VDC)

# AFR_GUI.py
An interactive program designed to help review the data collect utilizing pyqtgraph and Qt4 Designer.

# References
* pyqtgraph | [Website](http://www.pyqtgraph.org) | [Github Repositoty](https://github.com/pyqtgraph/pyqtgraph)
* Qt Designer | [Manual](https://doc.qt.io/archives/qt-4.8/designer-manual.html)
  * [Build Reference](https://www.tutorialspoint.com/pyqt/pyqt_using_qt_designer.htm)
