import random                                     # Import Python’s built-in random module (not used here but likely for future features)

from PyQt6.QtCore import QThread                  # Import QThread for running tasks in a separate thread
from PyQt6.QtWidgets import QApplication, QWidget, QTextEdit  
                                                  # Import QApplication (app context), QWidget (base UI class), QTextEdit (multi-line text widget)
from PyQt6 import uic, QtCore                     # Import uic for loading .ui files and QtCore for signals/slots
from pyqtgraph import PlotWidget                  # Import PlotWidget to embed real-time plots
from serial import Serial, unicode                 # Import Serial for serial communication and unicode for decoding bytes

from serial_thread import SerialThread            # Import custom SerialThread class for non-blocking serial reads
from tele_graph import TelemetryGraph             # Import custom TelemetryGraph helper for plotting data


class UI(QWidget):                                # Define UI class inheriting from QWidget

    def __init__(self):
        super().__init__()                        # Initialize the base QWidget

        # loading the ui file with uic module
        uic.loadUi('gsw.ui', self)                # Load the Qt Designer .ui file into this class’s attributes

        # Initiate serial port
        self.serial_port = Serial('COM3', 2000000, dsrdtr=True)  
                                                  # Open COM3 at 2,000,000 baud with DSR/DTR flow control

        # Initiate Serial Thread
        self.serialThread = SerialThread(self.serial_port)  
                                                  # Create a SerialThread instance, passing in the Serial object
        self._thread = QThread()                  # Create a QThread to run the SerialThread
        self.serialThread.moveToThread(self._thread)  
                                                  # Move the SerialThread object into the new QThread

        # Connect SerialThread signals to UI slot methods
        self.serialThread.connectionSuccess.connect(self.connection_success)
        self.serialThread.connectionFailed.connect(self.connection_failed)
        self.serialThread.readFailed.connect(self.error_on_read)

        self._thread.started.connect(self.serialThread.run)  
                                                  # When the QThread starts, call SerialThread.run()

        self.telemetryBuffer = ''                  # Buffer string to accumulate incoming serial data

        # Find the QTextEdit widgets by name from the loaded .ui
        self.outputBox = self.findChild(QTextEdit, 'outputBox')
        self.messageBox = self.findChild(QTextEdit, 'messageBox')
        self.serialThread.dataReceived.connect(self.updateOutputBox)  
                                                  # Connect incoming data signal to updateOutputBox()

        # setup graphs: wrap each PlotWidget in a TelemetryGraph helper
        self.altitudeGraph = TelemetryGraph(self.findChild(PlotWidget, 'altitudeGraph'))
        self.altitudeGraph.setTitle('Altitude')    # Set title for altitude graph
        self.altitudeGraph.addLine()               # Add default data line

        self.tempGraph = TelemetryGraph(self.findChild(PlotWidget, 'tempGraph'))
        self.tempGraph.setTitle('Temperature')     # Set title for temperature graph
        self.tempGraph.addLine()                   # Add default data line

        self.accelGraph = TelemetryGraph(self.findChild(PlotWidget, 'accelGraph'), legend=True)
        self.accelGraph.setTitle('Acceleration')   # Title for acceleration graph
        # Add separate lines for x, y, z axes
        self.accelGraph.addLine('x', 'red')
        self.accelGraph.addLine('y', 'green')
        self.accelGraph.addLine('z', 'blue')

        self.gyroGraph = TelemetryGraph(self.findChild(PlotWidget, 'gyroGraph'), legend=True)
        self.gyroGraph.setTitle('Gyro')            # Title for gyro graph
        # Add separate lines for x, y, z rotational rates
        self.gyroGraph.addLine('x', 'red')
        self.gyroGraph.addLine('y', 'green')
        self.gyroGraph.addLine('z', 'blue')

        self.y = 0                                  # Initialize the time/index counter for plotting

        self._thread.start()  # do this last!!!! this will make the serial port start reading
                                                  # Start the QThread, which triggers SerialThread.run()

    @QtCore.pyqtSlot()
    def connection_success(self):
        print('Connected!')                        # Slot: called when serial connection succeeds

    @QtCore.pyqtSlot(str)
    def connection_failed(self, error):
        print(error)                               # Slot: called when serial connection fails

    @QtCore.pyqtSlot(str)
    def error_on_read(self, error):
        print(error)                               # Slot: called when a read error occurs

    @QtCore.pyqtSlot(bytes)
    def updateOutputBox(self, data):              # Slot: called when bytes are received from serial
        try:
            # Decode incoming bytes to a string, ignoring errors
            self.telemetryBuffer += unicode(data, errors='ignore')

            # If a telemetry packet start/end markers are present
            if self.telemetryBuffer.find('TSP') != -1 and self.telemetryBuffer.find('TEP') != -1:
                s = self.telemetryBuffer.find('TSP')  # Find start index
                e = self.telemetryBuffer.find('TEP')  # Find end index

                # Extract the comma-separated values between markers
                data = self.telemetryBuffer[s + 3:e + 3].split(',')
                # Remove processed segment from buffer
                self.telemetryBuffer = self.telemetryBuffer[e + 3:]

                # Format the telemetry text for display
                telemetry = 'Altitude: %s\nTemperature: %s\n'\
                            'Accel X: %s\nAccel Y: %s\nAccel Z: %s\n'\
                            'Gyro X: %s\nGyro Y: %s\nGyro Z: %s\n\n'\
                            % (data[1], data[2], data[3], data[4],
                               data[5], data[6], data[7], data[8])

                self.outputBox.insertPlainText(telemetry)   # Append telemetry text
                self.outputBox.ensureCursorVisible()        # Auto-scroll to bottom

                self.y += 1                                 # Increment time/index counter

                # Plot each telemetry value on its respective graph
                self.altitudeGraph.plotData(float(data[1]), self.y)
                self.tempGraph.plotData(float(data[2]), self.y)
                self.accelGraph.plotData(float(data[3]), self.y, name='x')
                self.accelGraph.plotData(float(data[4]), self.y, name='y')
                self.accelGraph.plotData(float(data[5]), self.y, name='z')
                self.gyroGraph.plotData(float(data[6]), self.y, name='x')
                self.gyroGraph.plotData(float(data[7]), self.y, name='y')
                self.gyroGraph.plotData(float(data[8]), self.y, name='z')

            # If a simple message packet is present
            if self.telemetryBuffer.find('MSP') != -1 and self.telemetryBuffer.find('MEP') != -1:
                s = self.telemetryBuffer.find('MSP')  # Find message start
                e = self.telemetryBuffer.find('MEP')  # Find message end

                message = self.telemetryBuffer[s + 3:e]  # Extract message text
                self.telemetryBuffer = self.telemetryBuffer[e + 3:]  # Remove from buffer

                if len(message) > 0:
                    self.messageBox.insertPlainText(message + '\n')  # Append message text
                    self.messageBox.ensureCursorVisible()            # Auto-scroll

        except Exception as e:
            print(str(e))                            # Print any exceptions during parsing

    def closeEvent(self, event):                  # Override QWidget.closeEvent
        self.serialThread.stop()                  # Stop the serial-reading thread
        self._thread.quit()                       # Quit the QThread event loop
        self._thread.wait()                       # Wait for the thread to finish cleanly


app = QApplication([])                          # Create the Qt application context
window = UI()                                   # Instantiate the UI class
window.show()                                   # Show the main window
app.exec()                                      # Enter Qt’s event loop
