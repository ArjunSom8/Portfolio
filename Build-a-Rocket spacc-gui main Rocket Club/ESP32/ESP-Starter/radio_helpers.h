namespace RadioHelpers {                      // Namespace for grouping radio helper functions
  namespace {                                  // Anonymous namespace for internal linkage (private to this file)
    RF24 _radio;                                // Internal RF24 object to hold the radio instance
  }

  void setRadio(RF24 &radio_instance) {        // Store the provided RF24 instance for use by helper functions
    _radio = radio_instance;                   // Assign the passed radio instance to the internal variable
  }
  
  String received_message = "";                // Buffer to store the last received acknowledgment message
  
  void radioWrite(byte msg[], int len, bool fast) { // Low-level send: transmit raw bytes, optionally in fast mode
    if (fast) {                                 // If fast (non-blocking) mode is requested
      if (!_radio.writeFast(msg, len)) {        // Try sending quickly; returns false if TX FIFO is full
        _radio.txStandBy();                     // Wait until the current transmission completes
      }
    } else {                                    // If not using fast mode
      _radio.write(msg, len);                   // Perform a blocking write of the payload
    }
  
    if (_radio.available()) {                   // Check if an ACK payload has arrived from the receiver
      char ack[_radio.getDynamicPayloadSize()] = {0}; // Allocate buffer sized to the dynamic payload
      _radio.read(&ack, sizeof(ack));           // Read the ACK payload into the buffer
  
      received_message = String(ack);           // Convert the ACK bytes into a String
  
      Serial.println("Received: " + received_message); // Print the received ACK message to Serial
    }
  }
  
  void writeBytes(byte message[], int len) {    // Higher-level send: split a long byte array into 32-byte packets
    byte msg[32] = {0};                         // Temporary 32-byte buffer for each packet
    int i = 0;                                  // Packet counter
    if (len > 32) {                             // If the message exceeds one packet
      for (i = 0; i < int(len / 32); i++) {     // Send all full 32-byte chunks first
        memcpy(msg, message + (i * 32), 32);    // Copy next block into temp buffer
        radioWrite(msg, 32, true);              // Transmit block in fast mode
      }
      
      if (i * 32 < len) {                       // If there's a leftover partial block
        memcpy(msg, message + (i * 32), len - (i * 32)); // Copy remaining bytes
        radioWrite(msg, 32, true);              // Transmit final chunk in fast mode
      }
    } else {                                    // If the message fits in one packet
      memcpy(msg, message, 32);                 // Copy entire message into buffer (zero-padded)
      radioWrite(msg, 32, false);               // Transmit in blocking mode
    }
  }
  
  void writeMessage(String message, bool newLine = true) { // Convenience: send a String, optionally appending newline
    String packet = message + ((newLine) ? "\r\n" : "");   // Add CRLF if newLine is true
    writeBytes((byte *)packet.c_str(), strlen(packet.c_str())); // Convert to bytes and send via writeBytes
  }
}; // End of RadioHelpers namespace
