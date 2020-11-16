#include <SD.h>
#include <TimeLib.h>
#include <Adafruit_GPS.h>
#include <Wire.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define file_extension ".txt"
#define GPSSerial Serial1
#define SEALEVELPRESSURE_HPA (1013.25)
#define DEBUG 1
#define SD_DISABLED 0
#define TRANSMIT_ENABLED 1
#define TXLIMIT 500
#define TD 3000

#define PGCMD_NOANTENNA "$PGCMD,33,0*6D"

// Feather M0
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0

const int SD_CS = 10;
int counter;

File currentFile;
Adafruit_GPS GPS(&GPSSerial);
Adafruit_BME280 bme; // I2C
RH_RF95 rf95(RFM95_CS, RFM95_INT);

bool BIL = false;
String filename;
String txdata;
uint32_t TIM0 = millis();

// For setup, it needs to take < 10 seconds
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  // LoRA is on when RFM95_CS is LOW
  pinMode( RFM95_CS, OUTPUT );
  digitalWrite( RFM95_CS, HIGH );

  counter = 0; // counter is set to zero because GPS sends out data 3 times.
  filename = "";
  txdata = "";

  initSD();
  delay(1000);

  initGPS();
  delay(1000);

  initBME();
  digitalWrite( SD_CS, LOW ); // Turn low in order to initialize LoRA Properly
  delay(1000);

  initLoRA();
  digitalWrite( RFM95_CS, HIGH );
  delay(1000);
}

void initSD(void) {

  // If it fails to start using the selected port (default D10)
  // Check to see if SD CARD exists, if SD Card exists then logging will be directly sent to SD CARD
  // If it fails, then logging will take place using LOCUS (in-built logging)
  if (!SD.begin(SD_CS) & DEBUG ) {
    Serial.println("SD Card not connected");
    BIL = true;
  }
  else {
    BIL = false;
    Serial.println("Creating file name...");
    // Get the current date
    // MMDDYYNN, MM - Month, DD - day, YY - Year, NN - 00, 01, NN
    int n = 1;
    // Create temporary string to hold new file name
    //filename = String(month(), DEC) + String(day(), DEC) + String(year(), DEC) + String(n, DEC) + file_extension;
    filename = "DATA" + String (n, DEC) + file_extension;

    // Get new file name
    /*while (SD.exists(filename.c_str())) {
      ++n;
      filename = String(month(), DEC) + String(day(), DEC) + String(year(), DEC) + String(n, DEC) + file_extension;
      }*/
  }
}

void initGPS(void) {
  Serial.println("Starting GPS...");
  GPS.begin(9600);

  // IF BIL(Built-in Logging) is true, start LOCUS "$PMTK622, 1*29"
  if (BIL == true) {

    GPS.sendCommand( PMTK_SET_NMEA_OUTPUT_OFF );

    while ( GPSSerial.available() )
      GPSSerial.read();

    delay(1000);
    GPS.sendCommand( "$PMTK622,1*29" ); // dumps flash data
    Serial.println("----------------------------------------------------");

    // Start logging using LOCUS
    if (GPS.LOCUS_StartLogger())
      Serial.println(" STARTED! ");
    else {
      Serial.println(" no response :( ");
      return;

    }
  }

  // If BIL is off, then information will be saved directly to the SD Card under the file name
  else {

    // You can adjust which sentences to have the module emit, below
    // Default is RMC + GGA
    GPS.sendCommand( PMTK_SET_NMEA_OUTPUT_RMCGGA );

    // Default is 1 Hz update rate
    GPS.sendCommand( PMTK_SET_NMEA_UPDATE_1HZ );

    GPS.sendCommand(PGCMD_NOANTENNA); // Disable antenna requests

    //GPS.println( PMTK_Q_RELEASE );
    Serial.println(" SD Card detected, GPS data will be written to SD card. ");
    delay(500);
  }
  Serial.println("GPS Init Done");
}

void initBME(void) {
  unsigned status = bme.begin();
  if (!status & DEBUG) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }
  if ( DEBUG ) {
    Serial.println("-- BME 280 initialized successfully --");
  }

}

void initLoRA(void) {

  pinMode( RFM95_RST, OUTPUT );
  digitalWrite( RFM95_RST, HIGH );
  digitalWrite( RFM95_CS, LOW );

  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // Start - Pre-requisites to ensure that LoRA is initialized properly
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  // End -

  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);

}

// ================= Initialization Function Ends Here =================

String recordGPS(void) {
  char c = 0;
  String _data = "";

  while ( c != 10 ) {
    if ( GPSSerial.available() ) {
      c = GPS.read();
      _data += c;
    }
  }

  return _data;
}


// BME Data outputted as follows:
// 1. Temperature (Celsius)
// 2. Pressure (Pascal)
// 3. Altitude (Meters)
// 4. Humidity (Percentage)
String recordBME(void) {
  return String( bme.readTemperature(), 4 ) + "," + String( bme.readPressure() / 100.0F, 4 ) +
         "," + String( bme.readAltitude( SEALEVELPRESSURE_HPA ), 4 )
         + "," + String( bme.readHumidity() , 4 ) + '\n';
}

// When transmitting the following needs to occur:
// Send the GPS data which occurs 3 times
// Send the BME280 data which occurs once
// Wait for receive signal to confirm that information was sent.
// Try with char limit with 1200 first
// Similar code was found here: https://github.com/robertsmarty/adafruit3078gps/blob/master/Adafruit%20Feather%2032u4%20LoRa/GPS_TX.ino

/* @param total_data

   Converts the string with appended data into a char array to send it via LoRA.
*/
void transmit( String& tx_ready ) {

  char r_data[TXLIMIT];
  delay(500);
  Serial.println("Transmitting...");

  tx_ready.toCharArray( r_data, tx_ready.length() + 1 );

  rf95.send( ( uint8_t* )r_data, tx_ready.length() + 1 );
  rf95.waitPacketSent();
  delay(100);
}

void loop() {

  String _data = recordGPS();

  if ( ( millis() - TIM0 > TD ) && GPS.newNMEAreceived() ) {

    txdata += _data;


    // Time delay is a function of TD + counter*1000 +1/-1 + Time to receive new NMEA + Time to receive new fix.
    if ( ++counter > 1 ) {
      TIM0 = millis(); // restart counter once two data points have been recorded

      // For future reference, this will be converted from string to char[83*2 + 34] ~= 500 char limit
      txdata += recordBME();

      if ( DEBUG ) {
        Serial.print( txdata );
        Serial.println( "------" );
      }

      if ( !SD_DISABLED ) {
        // Since SD card is enabled here, prepare the SD Card to be written by turning RFM95_CS off and SD_CS on.
        Serial.println(" WRITING TO SD " );
        digitalWrite( RFM95_CS, HIGH );
        delay(500);
        digitalWrite( SD_CS, HIGH );

        // Write the retrieved data to the SD Card
        currentFile = SD.open( filename.c_str(), FILE_WRITE );
        currentFile.print( txdata );
        currentFile.println( "------" );
        currentFile.close();

        // Toggle the Chip Select pin for SD Card off, and turn the LoRA Chip Select on.
        digitalWrite( SD_CS, LOW );
        delay(500);
        digitalWrite( RFM95_CS, LOW );

        if ( TRANSMIT_ENABLED ) {
          // Transmit this data now.
          transmit( txdata );
          Serial.println(" TRANSMITTED SUCCESSFULLY " );
        }
      }
      counter = 0;
      txdata = "";
      Serial.println("RESET");
    }
  }
}
