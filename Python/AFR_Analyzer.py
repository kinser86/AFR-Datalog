# -*- coding: utf-8 -*-
import sys
from PyQt5 import QtCore, QtGui
import pyqtgraph as pg  #PyQtGraph is a pure-python graphics and GUI library built on PyQt4 / PySide and numpy
from pyqtgraph import PlotWidget
import csv
import numpy as np

# Variables
x = []      # Timestamp
yTps = []   # Throttle Position Sensor
yAfr = []   # Air Fuel Ratio
tmpC = []   # Temperature Degree Celsius
velocity = []   # Velocity (MpH)

class Ui_MainWindow(QtGui.QMainWindow):

    def __init__(self):
        super(Ui_MainWindow, self).__init__()
        # This sets up the main window
        self.resize(600, 300)
        self.setWindowTitle("Air-Fuel Ratio Analyzer")
        #self.setWindowIcon(QtGui.QIcon('/home/malfoy/Desktop/AFR/AFR.png'))
        
        # Center the window in the middle of the screen    
        frame = self.frameGeometry()
        centerPoint = QtGui.QDesktopWidget().availableGeometry().center()
        frame.moveCenter(centerPoint)
        self.move(frame.topLeft())

        # Setup the menu
        openFile = QtGui.QAction("&Open File", self)
        openFile.setShortcut("Ctrl+O")
        openFile.setStatusTip('Open file.')
        openFile.triggered.connect(self.file_open)

        quitApplication = QtGui.QAction("&Quit", self)
        quitApplication.setShortcut("Ctrl+Q")
        quitApplication.setStatusTip('Quit the application.')
        quitApplication.triggered.connect(self.close_application)

        self.clearData = QtGui.QAction("&Clear Data", self)
        self.clearData.setShortcut("Ctrl+L")
        self.clearData.setStatusTip('Clear the loaded data.')
        self.clearData.setEnabled(False)
        self.clearData.triggered.connect(self.data_clear)

        self.infoData = QtGui.QAction("&Information", self)
        self.infoData.setShortcut("Ctrl+I")
        self.infoData.setStatusTip('Information about the current dataset.')
        self.infoData.setEnabled(False)
        self.infoData.triggered.connect(self.data_info)

        self.statusBar = QtGui.QStatusBar(self)
        self.statusBar.setStyleSheet(
            "background-color: rgb(60, 60, 60); color: rgb(184, 181, 178);")
        self.setStatusBar(self.statusBar)

        # Create the menuBar
        mainMenu = self.menuBar()
        # Add the fileMenu to the menuBar
        fileMenu = mainMenu.addMenu('&File')
        fileMenu.addAction(openFile)
        fileMenu.addAction(self.clearData)
        fileMenu.addAction(quitApplication)
        # Add the dataMenu to the menuBar
        dataMenu = mainMenu.addMenu('&Data')
        dataMenu.addAction(self.infoData)

        # run the setupUi method
        self.setupUi()

    def setupUi(self):
        # Show the mainWindow
        self.show()

    def plot(self):
        self.resize(900, 600)

        # Center the window in the middle of the screen    
        frame = self.frameGeometry()
        centerPoint = QtGui.QDesktopWidget().availableGeometry().center()
        frame.moveCenter(centerPoint)
        self.move(frame.topLeft())
       
        # This creates the central widget
        self.centralwidget = QtGui.QWidget()
        self.centralwidget.setStyleSheet("background-color: rgb(0, 0, 0);")
        self.centralgridLayout = QtGui.QGridLayout(self.centralwidget)
        self.centralgridLayout.setMargin(0)
        # This create the horizontal layout
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setSpacing(2)
        # Create the graphics layout widget
        self.win = pg.GraphicsLayoutWidget(self.centralwidget)
        pg.setConfigOptions(antialias=True)
        self.horizontalLayout.addWidget(self.win)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)

        # Generate afrPlot
        self.afrPlot = self.win.addPlot(row = 0, col = 0)
        # Format afrPlot
        self.afrPlot.setLabel('left', 'Air-Fuel Ratio')
        self.afrPlot.setRange(yRange = [10, 20], padding = 0.1)
        self.afrPlot.setLimits(yMin = 10, yMax = 20)
        self.afrPlot.showGrid(x = True, y = True, alpha = 0.3)
        self.afrPlot.hideButtons()      #Disable auto-scale button
        self.afrPlot.setContentsMargins(0, 0, 0, 0)
        self.afrPlot.hideAxis('bottom')

        # Generate tpsPlot
        self.tpsPlot = self.win.addPlot(row = 1, col = 0)
        # Format tpsPlot
        self.tpsPlot.setLabel('left', 'Throttle (%)')
        self.tpsPlot.setRange(yRange = [0, 100], padding = 0.1)
        self.tpsPlot.setLimits(yMin = 0, yMax = 100)
        self.tpsPlot.showGrid(x = True, y = True, alpha = 0.3)
        self.tpsPlot.setXLink(self.afrPlot)
        self.tpsPlot.hideButtons()      #Disable auto-scale button
        self.tpsPlot.setContentsMargins(0, 0, 0, 0)
        self.tpsPlot.hideAxis('bottom')

        # Generate the velocityPlot
        self.velocityPlot = self.win.addPlot(row = 2, col = 0)
        # Format velocityPlot
        self.velocityPlot.setLabel('left', 'Velocity (MpH)')
        self.velocityPlot.setRange(yRange = [0, 100], padding = 0.1)
        self.velocityPlot.setLimits(yMin = 0, yMax = 100)
        self.velocityPlot.showGrid(x = True, y = True, alpha = 0.3)
        self.velocityPlot.setXLink(self.afrPlot)
        self.velocityPlot.hideButtons()      #Disable auto-scale button
        self.velocityPlot.setContentsMargins(0, 0, 0, 0)
        self.velocityPlot.hideAxis('bottom')

        # Generate the rangePlot
        self.rangePlot = self.win.addPlot(row = 3, col = 0)
        # Format rangePlot
        self.rangePlot.setLabel('left', 'Dataset')
        self.rangePlot.setRange(yRange = [10, 20], padding = 0.1)
        self.rangePlot.setLimits(yMin = 10, yMax = 20)
        self.rangePlot.showGrid(x = True, y = True, alpha = 0.3)
        self.rangePlot.hideButtons()      #Disable auto-scale button
        self.rangePlot.setContentsMargins(0, 0, 0, 0)

        # Build the UI
        self.centralgridLayout.addLayout(self.horizontalLayout, 0, 0, 1, 1)
        self.setCentralWidget(self.centralwidget)

        # Adjust the plot range to the data
        self.afrPlot.setRange(xRange = [x[0], x[-1]])
        self.afrPlot.setLimits(xMin = x[0], xMax = x[-1])
        self.tpsPlot.setRange(xRange = [x[0], x[-1]])
        self.tpsPlot.setLimits(xMin = x[0], xMax = x[-1])
        self.velocityPlot.setRange(xRange = [x[0], x[-1]])
        self.velocityPlot.setLimits(xMin = x[0], xMax = x[-1])
        self.rangePlot.setRange(xRange = [x[0], x[-1]])
        self.rangePlot.setLimits(xMin = x[0], xMax = x[-1])
        self.region = pg.LinearRegionItem()
        self.region.setZValue(10)
        self.rangePlot.addItem(self.region, ignoreBounds = True)
        # Adjust the range selector

        # Generate curve data
        self.afrPlot.plot(
            x,
            yAfr,
            pen = 'c',
            name = 'AFR')
        self.tpsPlot.plot(
            x,
            yTps,
            pen = 'm',
            name = 'TPS')
        self.velocityPlot.plot(
            x,
            velocity,
            pen = 'r',
            name = 'data')
        self.rangePlot.plot(
            x,
            yAfr,
            pen = 'y',
            name = 'data')

        # Crosshair
        self.lineAfr = pg.InfiniteLine(movable=True, angle = 90)
        self.lineTps = pg.InfiniteLine(movable=True, angle=90)
        self.lineVel = pg.InfiniteLine(movable=True, angle=90)
        self.lineRange = pg.InfiniteLine(movable=True, angle=90)
        self.lineAfr.setPos(x[200])
        self.afrPlot.addItem(self.lineAfr)
        self.tpsPlot.addItem(self.lineTps)
        self.velocityPlot.addItem(self.lineVel)
        self.rangePlot.addItem(self.lineRange)
        self.lineRange.setZValue(20)

        self.labelAFR = pg.InfLineLabel(
            self.lineAfr,
            text = "Drag",
            movable = True,
            position = 0.5,
            color = (200, 200, 100),
            fill = (200, 200, 200, 50))

        def afr_line_pos():
            self.linePos = self.lineAfr.getPos()
            self.lineTps.setPos(self.linePos[0])
            self.lineRange.setPos(self.linePos[0])
            self.interpAfr = np.interp(self.linePos[0], x, yAfr)
            self.interpTps = np.interp(self.linePos[0], x, yTps)
            self.interpTmp = np.interp(self.linePos[0], x, tmpC)
            self.interpVel = np.interp(self.linePos[0], x, velocity)
            self.interp = [self.interpAfr, self.interpTps, self.interpTmp, self.interpVel]
            #print("%0.2f, %0.2f" %(self.interpAfr, self.interpTps))
            self.labelAFR.setText("AFR: %0.2f\nTPS: %0.1f%%\nTMP: %0.1fC\nVEL: %0.1f" % (self.interp[0], self.interp[1], self.interp[2], self.interp[3]))

        def tps_line_pos():
            self.linePos = self.lineTps.getPos()
            self.lineAfr.setPos(self.linePos[0])
            self.lineVel.setPos(self.linePos[0])
            self.lineRange.setPos(self.linePos[0])

        def vel_line_pos():
            self.linePos = self.lineVel.getPos()
            self.lineAfr.setPos(self.linePos[0])
            self.lineTps.setPos(self.linePos[0])
            self.lineRange.setPos(self.linePos[0])

        def range_line_pos():
            self.linePos = self.lineRange.getPos()
            self.lineAfr.setPos(self.linePos[0])
            self.lineTps.setPos(self.linePos[0])
            self.lineVel.setPos(self.linePos[0])

        def update():
            self.region.setZValue(10)
            minX, maxX = self.region.getRegion()
            self.afrPlot.setXRange(minX, maxX, padding = 0)
            self.lineAfr.setBounds((minX, maxX))
            self.lineTps.setBounds((minX, maxX))
            self.lineVel.setBounds((minX, maxX))
            self.lineRange.setBounds((minX, maxX))

        self.lineAfr.sigPositionChanged.connect(afr_line_pos)
        self.lineTps.sigPositionChanged.connect(tps_line_pos)
        self.lineVel.sigPositionChanged.connect(vel_line_pos)
        self.lineRange.sigPositionChanged.connect(range_line_pos)
        self.region.sigRegionChanged.connect(update)

        def updateRegion(window, viewRange):
            self.rgn = viewRange[0]
            self.region.setRegion(self.rgn)

        self.afrPlot.sigRangeChanged.connect(updateRegion)
        self.region.setRegion([x[100], x[-100]])

    # menuBar Actions
    def file_open(self):
        global x, yTps, yAfr, tmpC, velocity, file

        name = QtGui.QFileDialog.getOpenFileName(self, 'Open File')
        file = open(name[0], 'r')

        with file:
            # Do stuff
            plots = csv.reader(file, delimiter=',')
            next(file)
            for row in plots:
                #print(row)
                x.append(int(row[0]))
                yTps.append(int(row[1]) * (100.0 / 1023.0))
                yAfr.append((int(row[2]) * (10.0 / 1023.0)) + 10.0)
                tmpC.append((((int(row[3]) * 5.0) / 1024.0) - 0.5) * 100)
                velocity.append((int(row[4]) * 60.0 * 60.0 * 3.14 * 2.0)/63360.0)
            #print(str(x))
        self.plot()
        # Enable option since data now exist.
        self.clearData.setEnabled(True)
        self.infoData.setEnabled(True)

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

    def data_clear(self):
        global x, yTps, yAfr, velocity

        file.close()

        x = []
        yTps = []
        yAfr = []
        velocity = []

        self.afrPlot.clear()
        self.tpsPlot.clear()
        self.rangePlot.clear()

        # Disable option since data no longer exist
        self.clearData.setEnabled(False)
        self.infoData.setEnabled(False)

    def data_info(self):
        # deltaWin = Ui_MainWindow()
        # deltaWin()
        timeDelta = [x[i + 1] - x[i] for i in range(len(x)-1)]

        self.deltaPlot = pg.plot(
            timeDelta,
            title = "Dataset Information",
            pen = 'r')
        self.deltaPlot.setTitle("Time difference")
        self.deltaPlot.setLabel('left', "Time", units = 'ms')
        self.deltaPlot.setLabel('bottom', "Sample Number")
        self.deltaPlot.setRange(xRange = [0, len(timeDelta)-1])
        self.deltaPlot.setLimits(
            xMin = 0,
            xMax = len(timeDelta)-1,
            yMin = 0)
        self.deltaPlot.showGrid(x = True, y = True, alpha = 0.3)
        ax = self.deltaPlot.getAxis('left')

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    #Define the program
    Gui = Ui_MainWindow()
    # Execute the application
    sys.exit(app.exec_())