# AFR-Datalog
AFR data-logger written for Arduino

# Pinouts
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

# AFR_GUI.py
An interactive program designed to help review the data collect utilizing pyqtgraph and Qt4 Designer.

# References
* pyqtgraph | [Website](http://www.pyqtgraph.org) | [Github Repositoty](https://github.com/pyqtgraph/pyqtgraph)
* Qt Designer | [Manual[(https://doc.qt.io/archives/qt-4.8/designer-manual.html)
  * [Build Reference](https://www.tutorialspoint.com/pyqt/pyqt_using_qt_designer.htm)
