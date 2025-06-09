import serial                                  # Import pyserial for serial port communication
from PyQt6 import QtCore                       # Import QtCore for Qt’s core functionality (signals, slots, threading)
from PyQt6.QtCore import pyqtSignal, QObject    # Import pyqtSignal to define custom signals, QObject as base class

class SerialThread(QObject):                    # Define SerialThread class inheriting from QObject
    connectionFailed = pyqtSignal(str)          # Signal: emitted when connection fails, carries an error message
    connectionSuccess = pyqtSignal()            # Signal: emitted when connection succeeds (no payload)
    readFailed = pyqtSignal(str)                # Signal: emitted when a read operation fails, carries an error message
    dataReceived = pyqtSignal(bytes)            # Signal: emitted when data is received, carries raw bytes

    def __init__(self, serial_instance):        # Constructor takes a configured serial port instance
        """\
        Initialize thread.
        Note that the serial_instance' timeout is set to one second!
        Other settings are not changed.
        """                                     # Docstring: describes initialization and timeout behavior
        QObject.__init__(self)                  # Initialize the base QObject
        self.serial = serial_instance           # Store the passed serial port instance
        self.serial.close()                     # Ensure the serial port is closed initially
        self.alive = True                       # Flag to control the read loop

    def stop(self):                             # Method to stop the reader thread
        """Stop the reader thread"""            # Docstring for stop method
        self.alive = False                     # Clear the alive flag to break the loop
        if hasattr(self.serial, 'cancel_read'): # If the serial object supports cancel_read...
            self.serial.cancel_read()          # Cancel any blocking read in progress
        self.serial.close()                    # Close the serial port

    def run(self):                              # Main method to open the port and read incoming data
        if not hasattr(self.serial, 'cancel_read'):  # If cancel_read is not supported by this serial object...
            self.serial.timeout = 1                  # Set a 1-second timeout for blocking reads
        try:
            self.serial.open()                       # Attempt to open the serial port
        except Exception as e:                       # On failure to open...
            self.alive = False                       # Mark the thread as no longer alive
            self.connectionFailed.emit(str(e))       # Emit connectionFailed signal with error text
            return                                   # Exit the run method
        error = None                                 # Placeholder for any errors that occur after opening
        self.connectionSuccess.emit()                # Emit connectionSuccess signal to notify success

        while self.alive and self.serial.is_open:    # Loop while thread is alive and port remains open
            try:
                # Read all waiting bytes or block until at least one arrives
                data = self.serial.read(self.serial.in_waiting or 1)
            except serial.SerialException as e:       # On I/O error (e.g., disconnect)...
                error = e                             # Store the exception
                self.readFailed.emit(str(e))         # Emit readFailed signal with error text
                break                                 # Exit the read loop
            else:
                if data:                              # If data was actually read...
                    try:
                        self.dataReceived.emit(data)  # Emit dataReceived signal with the raw bytes
                    except Exception as e:            # If the slot handling raises an exception...
                        error = e                     # Store the exception
                        self.readFailed.emit(str(e))  # Emit readFailed signal
                        break                         # Exit the read loop
        self.alive = False                            # Ensure alive flag is cleared on exit
        self.connectionFailed.emit()                  # Emit connectionFailed to signal thread termination

    @QtCore.pyqtSlot(bytes)                         # Decorate write() as a Qt slot accepting bytes
    def write(self, data):                          # Slot method to send bytes back over serial
        print('Called')                             # Debug print when write() is invoked
        self.serial.write(data)                     # Write the given bytes to the serial port
