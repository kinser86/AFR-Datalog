# -*- coding: utf-8 -*-
import sys
from PyQt4 import QtCore, QtGui
import pyqtgraph as pg  #PyQtGraph is a pure-python graphics and GUI library built on PyQt4 / PySide and numpy
from pyqtgraph import PlotWidget
import csv

# Variables
x = []
y_tps = []
y_afr = []

# Read the file
with open('/home/malfoy/Desktop/AFR/test.txt', 'rb') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    next(csvfile)
    for row in plots:

      #TPS.value = (TPS.raw * (4.0 / 1023.0)) + 0.5;    // TPS ADC
      #AFR.value = (AFR.raw * (10.0 / 1023.0)) + 10.0;  // AFR ADC
        x.append(int(row[0]))
        y_tps.append((int(row[1]) * (4.0 / 1023.0)) + 0.5)
        y_afr.append((int(row[2]) * (10.0 / 1023.0)) + 10.0)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        # This sets up the main window
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(800, 600)
        MainWindow.setWindowTitle("Air-Fuel Ratio Data Analyzer")
        MainWindow.setStyleSheet("background-color: rgb(0, 0, 0);")
        # This creates the central widget
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.centralgridLayout = QtGui.QGridLayout(self.centralwidget)
        self.centralgridLayout.setMargin(5)
        self.centralgridLayout.setSpacing(2)
        self.centralgridLayout.setObjectName("centralgridLayout")
        # This create the horizontal layout
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setSpacing(2)
        self.horizontalLayout.setObjectName("horizontalLayout")
        # Create the graphics layout widget
        self.win = pg.GraphicsLayoutWidget(self.centralwidget)
        self.win.setFrameShape(QtGui.QFrame.NoFrame)
        self.horizontalLayout.addWidget(self.win)

        # Generate afrPlot
        self.afrPlot = self.win.addPlot(
            row = 0,
            col = 0)
        # Format afrPlot
        self.afrPlot.setLabel(
            'left',
            'Air-Fuel Ratio')
        self.afrPlot.setRange(
            #xRange = [x[0], x[-1]],
            yRange = [10, 20],
            padding = 0.1)
        self.afrPlot.setLimits(
            xMin = x[0],
            xMax = x[-1],
            yMin = 10,
            yMax = 20)
        self.afrPlot.showGrid(
            x = True,
            y = True,
            alpha = 0.3)
        self.afrPlot.hideButtons()      #Disable auto-scale button
        self.afrPlotyAxis = self.afrPlot.getAxis('left')
        self.afrPlotyAxis.setTickSpacing(2, 2)

        # Generate tpsPlot
        self.tpsPlot = self.win.addPlot(
            row = 1,
            col = 0)
        # Format tpsPlot
        self.tpsPlot.setLabel(
            'left',
            'Throttle')
        self.tpsPlot.setRange(
            #xRange = [x[0], x[-1]],
            yRange = [0, 100],
            padding = 0.1)
        self.tpsPlot.setLimits(
            xMin = x[0],
            xMax = x[-1],
            yMin = 0,
            yMax = 100)
        self.tpsPlot.showGrid(
            x = True,
            y = True,
            alpha = 0.3)
        self.tpsPlot.setXLink(self.afrPlot)
        self.tpsPlot.hideButtons()      #Disable auto-scale button

        # Generate the rangePlot
        self.rangePlot = self.win.addPlot(
            row = 2,
            col = 0)
        # Format rangePlot
        self.rangePlot.setLabel(
            'left',
            'Dataset')
        self.rangePlot.setRange(
            xRange = [x[0], x[-1]],
            yRange = [10, 20],
            padding = 0.1)
        self.rangePlot.setLimits(
            xMin = x[0],
            xMax = x[-1],
            yMin = 10,
            yMax = 20)
        self.rangePlot.hideButtons()      #Disable auto-scale button
        self.region = pg.LinearRegionItem()
        self.region.setZValue(10)
        self.rangePlot.addItem(self.region, ignoreBounds = True)

        # Plot the information
        self.afrPlot.plot(
            x,
            y_afr,
            pen = 'c',
            name = 'AFR')
        self.tpsPlot.plot(
            x,
            y_tps,
            pen = 'm',
            name = 'TPS')
        self.rangePlot.plot(
            x,
            y_afr,
            pen = 'y',
            name = 'data')

        # Build the UI
        self.centralgridLayout.addLayout(self.horizontalLayout, 0, 0, 1, 1)
        MainWindow.setCentralWidget(self.centralwidget)

        def update():
            self.region.setZValue(10)
            minX, maxX = self.region.getRegion()
            self.afrPlot.setXRange(minX, maxX, padding = 0)

        self.region.sigRegionChanged.connect(update)

        def updateRegion(window, viewRange):
            self.rgn = viewRange[0]
            self.region.setRegion(self.rgn)

        self.afrPlot.sigRangeChanged.connect(updateRegion)
        self.tpsPlot.sigRangeChanged.connect(updateRegion)

        self.region.setRegion([x[0], x[50]])

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    QtGui.QApplication.setStyle(QtGui.QStyleFactory.create('Cleanlooks'))
    MainWindow = QtGui.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
