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

class Ui_MainWindow(QtGui.QMainWindow):

    def __init__(self):
        super(Ui_MainWindow, self).__init__()
        # This sets up the main window
        self.resize(800, 600)
        self.setWindowTitle("Air-Fuel Ratio Data Analyzer")
        self.setStyleSheet("background-color: rgb(0, 0, 0);")

        # Setup the menu
        openFile = QtGui.QAction("&Open File", self)
        openFile.setShortcut("Ctrl+O")
        openFile.setStatusTip('Open File')
        openFile.triggered.connect(self.file_open)

        quitApplication = QtGui.QAction("&Quit", self)
        quitApplication.setShortcut("Ctrl+Q")
        quitApplication.setStatusTip('Quit the application.')
        quitApplication.triggered.connect(self.close_application)

        self.statusBar()    # Call the status bar, define once

        # add items to the menubar
        mainMenu = self.menuBar()
        fileMenu = mainMenu.addMenu('&File')
        fileMenu.addAction(openFile)
        fileMenu.addAction(quitApplication)

        # run the setupUi method
        self.setupUi()

    def setupUi(self):
        # This creates the central widget
        self.centralwidget = QtGui.QWidget()
        self.centralgridLayout = QtGui.QGridLayout(self.centralwidget)
        self.centralgridLayout.setMargin(5)
        self.centralgridLayout.setSpacing(2)
        # This create the horizontal layout
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setSpacing(2)
        # Create the graphics layout widget
        self.win = pg.GraphicsLayoutWidget(self.centralwidget)
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
            yRange = [10, 20],
            padding = 0.1)
        self.afrPlot.setLimits(
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
        self.tpsPlot.setRange(yRange = [0, 100], padding = 0.1)
        self.tpsPlot.setLimits(
            yMin = 0,
            yMax = 100)
        self.tpsPlot.setLabel(
            'left',
            'Throttle')
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
            yRange = [10, 20],
            padding = 0.1)
        self.rangePlot.setLimits(
            yMin = 10,
            yMax = 20)
        self.rangePlot.hideButtons()      #Disable auto-scale button

        # Build the UI
        self.centralgridLayout.addLayout(self.horizontalLayout, 0, 0, 1, 1)
        self.setCentralWidget(self.centralwidget)

        # Show the mainWindow
        self.show()

    def plot(self):
        # Adjust the plot range to the data
        self.afrPlot.setRange(xRange = [x[0], x[-1]])
        self.afrPlot.setLimits(xMin = x[0], xMax = x[-1])
        self.tpsPlot.setRange(xRange = [x[0], x[-1]])
        self.tpsPlot.setLimits(xMin = x[0], xMax = x[-1])
        self.rangePlot.setRange(xRange = [x[0], x[-1]])
        self.rangePlot.setLimits(xMin = x[0], xMax = x[-1])
        self.region = pg.LinearRegionItem()
        self.region.setZValue(10)
        self.rangePlot.addItem(self.region, ignoreBounds = True)
        # Adjust the range selector

        # Generate curve data
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

        def update():
            self.region.setZValue(10)
            minX, maxX = self.region.getRegion()
            self.afrPlot.setXRange(minX, maxX, padding = 0)

        self.region.sigRegionChanged.connect(update)

        def updateRegion(window, viewRange):
            self.rgn = viewRange[0]
            self.region.setRegion(self.rgn)

        self.afrPlot.sigRangeChanged.connect(updateRegion)
        self.region.setRegion([x[0], x[50]])

    # menuBar Actions
    def file_open(self):
        global x, y_tps, y_afr
        name = QtGui.QFileDialog.getOpenFileName(self, 'Open File')
        file = open(name, 'r')

        with file:
            # Do stuff
            plots = csv.reader(file, delimiter=',')
            next(file)
            for row in plots:
                #print(row)
                x.append(int(row[0]))
                y_tps.append((int(row[1]) * (4.0 / 1023.0)) + 0.5)
                y_afr.append((int(row[2]) * (10.0 / 1023.0)) + 10.0)
        self.plot()
    def close_application(self):
        choice = QtGui.QMessageBox.question(self,
            'Quit',     # Window title
            "Quit the application?",    # Window content
            QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
        if choice == QtGui.QMessageBox.Yes:
            print("Closing")
            sys.exit()
        else:
            pass

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    #Define the program
    Gui = Ui_MainWindow()
    # Execute the application
    sys.exit(app.exec_())
