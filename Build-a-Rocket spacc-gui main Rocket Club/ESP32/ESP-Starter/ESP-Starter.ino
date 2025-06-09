// Include Libraries
#include <Wire.h>                   // I2C communication library
#include <SPI.h>                    // SPI communication library
#include <nRF24L01.h>               // nRF24L01 register definitions
#include <RF24.h>                   // RF24 library for nRF24L01 module
#include "radio_helpers.h"          // Custom helper functions for radio messaging
#include "Adafruit_BMP3XX.h"        // Library for the BMP3XX barometric sensor
#include <Adafruit_LSM6DSOX.h>      // Library for the LSM6DSOX IMU sensor

// I2C Pins
#define I2C_SCL 2                   // Define pin 2 as I2C clock (SCL)
#define I2C_SDA 4                   // Define pin 4 as I2C data (SDA)

// nRF24 Pins
#define NRF24_SCLK 14               // SPI clock pin for nRF24 (green wire)
#define NRF24_MISO 13               // SPI MISO pin for nRF24 (purple wire)
#define NRF24_MOSI 12               // SPI MOSI pin for nRF24 (blue wire)
#define NRF24_CS   15               // SPI chip select (CSN) pin for nRF24 (orange wire)
#define NRF24_CE   16               // Chip enable (CE) pin for nRF24 (yellow wire, not used)

// nRF24 Values
#define RF24_CHANNEL 125            // RF channel number for communication

SPIClass SPI2(VSPI);               // Instantiate secondary SPI bus on VSPI
RF24 radio(NRF24_CE, NRF24_CS);    // Create RF24 object using CE and CS pins on SPI2

// Addresses for transmission and reception (5 bytes each)
const uint8_t txa[5] = {0x01, 0x00, 0x00, 0x00, 0x00};  // Transmit pipe address
const uint8_t rxa[5] = {0x02, 0x00, 0x00, 0x00, 0x00};  // Receive pipe address

// Barometer
#define SEALEVEL_HPA 1013.25       // Sea-level pressure for altitude calculation
Adafruit_BMP3XX bmp;               // Instantiate BMP3XX sensor object

// IMU
Adafruit_LSM6DSOX lsm;             // Instantiate LSM6DSOX sensor object

void setup()
{
  SPI2.begin(NRF24_SCLK, NRF24_MISO, NRF24_MOSI, NRF24_CS);  
                                  // Initialize SPI2 with the specified pins

  Serial.begin(115200);           // Start Serial at 115200 baud for debug output

  if (!radio.begin(&SPI2))        // Initialize the nRF24L01 radio over SPI2
  {
    Serial.println("Failed to start radio...");  // Error if init fails
  } else {
    Serial.println("Success!");   // Confirmation if init succeeds
  }

  if (!radio.isChipConnected())   // Verify SPI link to the radio chip
  {
    Serial.println("Failed to detect radio");    // Error if not detected
  } else {
    Serial.println("Success!");   // Confirmation if detected
  }

  if (!radio.isValid())           // Check that radio object has valid configuration
  {
    Serial.println("Failed?? Not a real radio??"); // Warning if invalid
  } else {
    Serial.println("Success!");   // Confirmation if valid
  }
   
  radio.powerUp();                // Power up the radio module

  // Configure radio parameters for transmitting
  radio.openWritingPipe(rxa);     // Set the writing pipe address
  radio.setDataRate(RF24_2MBPS);  // Use 2 Mbps data rate
  radio.setCRCLength(RF24_CRC_16);// Use 16-bit CRC for error checking
  radio.setPALevel(RF24_PA_MAX);  // Set max power level
  radio.enableDynamicPayloads();  // Allow variable payload sizes
  radio.enableAckPayload();       // Enable attaching payloads to ACKs
  radio.setChannel(RF24_CHANNEL); // Set RF channel
  radio.stopListening();          // Switch to transmit mode

  RadioHelpers::setRadio(radio);              // Pass radio object to helper library
  RadioHelpers::writeMessage("Radio Initialized"); // Notify via radio that init is done

  Wire.begin(I2C_SDA, I2C_SCL);   // Initialize I2C with SDA and SCL pins

  // Initialize BMP3XX sensor over I2C, retry until found
  while (!bmp.begin_I2C())
  {
    Serial.println("BMP388 Not Found");           // Log error
    RadioHelpers::writeMessage("BMP388 Not Found"); // Notify via radio
    delay(1000);                                  // Wait before retry
  }

  Serial.println("BMP Found");    // Log success
  RadioHelpers::writeMessage("BMP Found"); // Notify via radio

  // Improve BMP readings: set oversampling, filter, and data rate
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X); // 8× temperature oversampling
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);    // 4× pressure oversampling
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);       // IIR filter coefficient 3
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);                // 50 Hz output data rate

  // Initialize LSM6DSOX IMU over I2C, retry until found
  while (!lsm.begin_I2C())
  {
    Serial.println("LSM Not Found");                // Log error
    RadioHelpers::writeMessage("LSM Not Found");    // Notify via radio
    delay(1000);                                    // Wait before retry
  }

  Serial.println("LSM Found");      // Log success
  RadioHelpers::writeMessage("LSM Found"); // Notify via radio
  
  Serial.println("Setup Complete"); // Indicate setup is finished
  RadioHelpers::writeMessage("Setup Complete"); // Notify via radio
  delay(50);                        // Short delay before entering loop
}

void loop()
{ 
  bmp.performReading();             // Trigger a new BMP3XX measurement

  // Read accelerometer, gyroscope, and temperature events from IMU
  sensors_event_t a, g, t;
  lsm.getEvent(&a, &g, &t);         // Populate a/g/t with sensor data
  
  Serial.println("Transmitting..."); // Debug log before sending telemetry

  double altitude = bmp.readAltitude(SEALEVEL_HPA); // Calculate altitude from pressure
  double temperature = bmp.temperature;             // Read temperature in °C
  
  // Build a comma-separated telemetry string
  String telemetry = "START,";
  telemetry += String(altitude) + ",";              // Append altitude
  telemetry += String(temperature) + ",";           // Append temperature
  telemetry += String(a.acceleration.x) + ",";      // Append X-axis acceleration
  telemetry += String(a.acceleration.y) + ",";      // Append Y-axis acceleration
  telemetry += String(a.acceleration.z) + ",";      // Append Z-axis acceleration
  telemetry += String(g.gyro.x) + ",";              // Append X-axis gyro rate
  telemetry += String(g.gyro.y) + ",";              // Append Y-axis gyro rate
  telemetry += String(g.gyro.z) + ",";              // Append Z-axis gyro rate
  telemetry += "END\n";                             // Mark end of message
  
  RadioHelpers::writeMessage(telemetry);            // Send telemetry via radio
  
  delay(100);                                       // Wait 100 ms before next loop
}
