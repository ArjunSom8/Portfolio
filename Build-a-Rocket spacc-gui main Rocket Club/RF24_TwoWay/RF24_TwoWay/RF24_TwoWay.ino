#include <SPI.h>                    // Include the SPI library for hardware SPI communication with the nRF24L01 module
#include <nRF24L01.h>              // Include definitions specific to the nRF24L01+ transceiver
#include <RF24.h>                  // Include the RF24 library for higher-level control of the radio

#define NRF24_SCLK 8               // Define the pin connected to the nRF24L01 SCLK line
#define NRF24_MISO 9               // Define the pin connected to the nRF24L01 MISO line
#define NRF24_MOSI 10              // Define the pin connected to the nRF24L01 MOSI line
#define NRF24_CS   0               // Define the pin connected to the nRF24L01 CSN (chip select) line
#define NRF24_CE   1               // Define the pin connected to the nRF24L01 CE (chip enable) line

RF24 radio(NRF24_CE, NRF24_CS);     // Create an RF24 object named “radio” using the CE and CS pins

// Addresses (5 bytes) for TX and RX pipes; must match on both modules to communicate
const uint8_t txa[5] = {0x01,0x00,0x00,0x00,0x00};  // Transmit address
const uint8_t rxa[5] = {0x02,0x00,0x00,0x00,0x00};  // Receive address

void setup()
{
  Serial.begin(2000000);            // Initialize Serial at 2,000,000 baud for debug output
  while (!Serial);                  // Wait here until the Serial port is ready (needed on some boards)

  if (!radio.begin())               // Attempt to initialize the nRF24L01 module
  {
    Serial.println("Failed to start radio...");  // Print error if initialization failed
  } 
  else
  {
    Serial.println("Success!");     // Print success message if radio.begin() succeeded
  }

  if (!radio.isChipConnected())     // Check if the SPI connection to the chip is working
  {
    Serial.println("Failed to detect radio");    // Error if no chip detected
  } 
  else
  {
    Serial.println("Success!");     // Success if chip connection is valid
  }

  if (!radio.isValid())             // Verify that the radio object contains valid configuration
  {
    Serial.println("Failed?? Not a real radio??");  // Error if radio configuration is invalid
  } 
  else
  {
    Serial.println("Success!");     // Success if radio.isValid() returns true
  }

  radio.powerUp();                  // Power up the radio transceiver

  // Configure the radio parameters
  radio.openReadingPipe(0, rxa);    // Open reading pipe #0 with the specified RX address
  radio.setDataRate(RF24_2MBPS);    // Set data rate to 2 Mbps for faster communication
  radio.setCRCLength(RF24_CRC_16);  // Use 16-bit CRC for error checking
  radio.setPALevel(RF24_PA_MAX);    // Set power amplifier level to maximum
  radio.enableDynamicPayloads();    // Enable dynamic payload lengths for more flexible packet sizes
  radio.enableAckPayload();         // Allow attaching a payload to the ACK (acknowledgment) packet
  radio.setChannel(125);            // Set the RF channel to 125 (2.525 GHz)
  radio.startListening();           // Put the radio into receive mode
}

void loop()
{
  // Continuously check for incoming data
  while (radio.available())
  {
    if (radio.getDynamicPayloadSize() < 1)  // Check that there is at least 1 byte in the payload
      return;                               // If not, exit to avoid processing empty packets
    
    byte text[radio.getDynamicPayloadSize()] = {0};  // Create a buffer sized to the incoming payload
    radio.read(&text, sizeof(text));                // Read the payload into the buffer
    
    if (Serial.available())                         // If there's data waiting on Serial (from PC)
    {
      char buf[Serial.available()];                 // Create a buffer for the Serial data
      size_t len = Serial.readBytes(buf, sizeof(buf));  // Read the Serial data into buf
      radio.writeAckPayload(0, buf, len);           // Send buf back as an ACK payload on pipe 0
    }
    
    Serial.write(text, sizeof(text));               // Write the received radio payload out to Serial
  }
}
