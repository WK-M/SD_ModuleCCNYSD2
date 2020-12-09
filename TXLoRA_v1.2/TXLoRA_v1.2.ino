#include <SD.h>
#include <Wire.h>
#include <Adafruit_GPS.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_Sensor.h>  
#include <Adafruit_BME280.h>
#include <Adafruit_SleepyDog.h>

// User Variables 
// Add/Remove/Modify variables that can vary from day to day.
#define INHG 29.74 // Pressure of the area that BME280 Sensor will be testing in.

// ----------------------------------------------------------------------------------------------
// Code below this line does not need to touched! Only developers need to touch the code below.
// Users only need to touch "User Variables"
// ----------------------------------------------------------------------------------------------

// Developer Debugging Definitions
#define DEBUG 0 // 0 - Turn off Serial prints, 1 - Turn on Serial Prints
#define SD_DISABLED 0 // Set 0 to turn SD ONM or Set 1 to turn SD OFF
#define TRANSMIT_ENABLED 1 // Set 0 to turn TRANSMISSION OFF or Set 1 to turn TRANSMISSION ON
#define GPS_ENABLED 0 // Set 0 to turn ENABLE PIN OFF, or Set 1 to turn ENABLE PIN ON

// MAX Sizes LoRA, GPS, BME
#define TXLIMIT 450
#define NMEALIMIT 100
#define BMELIMIT 50

// SD Card Definitions
#define file_extension ".txt"

// GPS Definitions
#define GPSSerial Serial1
#define EN_PIN 6
#define PGCMD_NOANTENNA "$PGCMD,33,0*6D"

// BME Definitions
#define CURRENT_HPA ( 33.8638816 * INHG )
#define SEALEVELPRESSURE_HPA (CURRENT_HPA) // MUST MODIFY THE VALUE IN HERE

// Time Delay
#define TD 3000

// Feather M0 Definitions
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
#define TXPOWER 13
#define SD_CS 10
int counter;

File currentFile;
Adafruit_GPS GPS(&GPSSerial);
Adafruit_BME280 bme; // Connected through I2C
RH_RF95 rf95(RFM95_CS, RFM95_INT);

bool BIL = false;
String filename;
char txdata[TXLIMIT];
uint32_t TIM0 = millis();

// For setup, it needs to take < 10 seconds
void setup() {
  Serial.begin(115200);
#if ( DEBUG )
  while (!Serial) {
    ;
  }
#endif

  // Turn Featherboard LED 13 off
  pinMode( 13, OUTPUT );
  digitalWrite( 13, LOW );

#if ( DEBUG )
  Serial.println("Turning LORA OFF");
#endif

  // LoRA is on when RFM95_CS is LOW
  pinMode( RFM95_CS, OUTPUT );
  digitalWrite( RFM95_CS, HIGH );

  #if ( GPS_ENABLED )
  pinMode( EN_PIN, OUTPUT );
  digitalWrite( EN_PIN, HIGH );
  #endif

  pinMode( SD_CS, OUTPUT );
  digitalWrite( SD_CS, LOW );

  delay(2000);

  counter = 0; // counter is set to zero because GPS sends out data 3 times.
  filename = "";

  initSD();
  initGPS();
  initBME();
  //digitalWrite( SD_CS, LOW ); // Turn low in order to initialize LoRA Properly
  initLoRA();
  digitalWrite( RFM95_CS, HIGH ); // Write high to turn LoRA off after initialization
  #if ( GPS_ENABLED )
  digitalWrite( EN_PIN, LOW );
  #endif
  rf95.sleep(); // Sleep radio in the meantime
  delay(3000);
}

void initSD(void) {
  // If it fails to start using the selected port (default D10)
  // Check to see if SD CARD exists, if SD Card exists then logging will be directly sent to SD CARD
  // If it fails, then logging will take place using LOCUS (in-built logging)
  if ( !SD.begin( SD_CS ) ) {

#if ( DEBUG )
    Serial.println("SD Card not connected");
#endif

    BIL = true;
  }
  else {
    BIL = false;

#if ( DEBUG )
    Serial.println("Creating file name...");
#endif
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
#if ( DEBUG )
  Serial.println("Starting GPS...");
#endif

  GPS.begin(9600);

  // IF BIL(Built-in Logging) is true, start LOCUS "$PMTK622, 1*29"
  if (BIL == true) {

    /*GPS.sendCommand( PMTK_SET_NMEA_OUTPUT_OFF );

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
      }*/
#if ( DEBUG )
    Serial.println("Broke");
#endif

  }

  // If BIL is off, then information will be saved directly to the SD Card under the file name
  else {

    // You can adjust which sentences to have the module emit, below
    // Default is RMC + GGA
    GPS.sendCommand( PMTK_SET_NMEA_OUTPUT_RMCGGA );

    // Default is 1 Hz update rate
    GPS.sendCommand( PMTK_SET_NMEA_UPDATE_1HZ );

    GPS.sendCommand(PGCMD_NOANTENNA); // Disable antenna requests

#if ( DEBUG )
    Serial.println(" SD Card detected, GPS data will be written to SD card. ");
#endif
    delay(500);
  }

#if ( DEBUG )
  Serial.println("GPS Init Done");
#endif

}

void initBME(void) {
  unsigned status = bme.begin();
  if ( !status ) {

#if ( DEBUG )
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
#endif

    while (1) delay(10);
  }
#if ( DEBUG )
  Serial.println("-- BME 280 initialized successfully --");
#endif

  bme.setSampling(bme.MODE_FORCED, 
                    bme.SAMPLING_X1,
                    bme.SAMPLING_X1,
                    bme.SAMPLING_X1,
                    bme.FILTER_OFF);

}

void initLoRA(void) {

  pinMode( RFM95_RST, OUTPUT );
  digitalWrite( RFM95_RST, HIGH );
  digitalWrite( RFM95_CS, LOW );

  delay(100);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // Start - Pre-requisites to ensure that LoRA is initialized properly
  while (!rf95.init()) {
#if ( DEBUG )
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
#endif
    while (1);
  }

#if ( DEBUG )
  Serial.println("LoRa radio init OK!");
#endif

  if (!rf95.setFrequency(RF95_FREQ)) {
#if ( DEBUG )
    Serial.println("setFrequency failed");
#endif
    while (1);
  }
  // End -

#if ( DEBUG )
  Serial.print("Set Freq to: ");
  Serial.println( RF95_FREQ );
#endif
  rf95.setTxPower( TXPOWER, false ); // Second parameter is true to prevent use of PA_BOOST

}

// ================= Initialization Function Ends Here =================

char* recordGPS(void) {
  char c = GPS.read();
  char* _data = ( char* )malloc( sizeof(char) * NMEALIMIT );
  memset( _data, 0, sizeof _data );

  while ( c != 10 ) {
    if ( GPSSerial.available() ) {
      c = GPS.read();
      strncat( _data, &c, sizeof(char) );
    }
  }
  return _data;
}

// BME Data outputted as: Temperature ( Celsius ), Pressure ( Pascal ), Altitude ( Meters ), Humidity ( Percentage )
char* recordBME(void) {
  // String written like this to reduce heap allocations mentioned here: https://cpp4arduino.com/2018/11/21/eight-tips-to-use-the-string-class-efficiently.html
  bme.takeForcedMeasurement();
  String _BMEdata = String( bme.readTemperature(), 4 );
  _BMEdata += ",";
  _BMEdata += String( bme.readPressure() / 100.0F, 4 );
  _BMEdata += ",";
  _BMEdata += String( bme.readAltitude( SEALEVELPRESSURE_HPA ), 4 );
  _BMEdata += ",";
  _BMEdata += String( bme.readHumidity() , 4 );
  _BMEdata += '\n';

  char* _data = ( char* )malloc( sizeof(char) * BMELIMIT );
  strncpy( _data, _BMEdata.c_str(), _BMEdata.length() + 1 );
  return _data;
}

// When transmitting the following needs to occur:
// Send the GPS data which occurs 3 times
// Send the BME280 data which occurs once
// Wait for receive signal to confirm that information was sent.
// Try with char limit with 500 first
// Similar code was found here: https://github.com/robertsmarty/adafruit3078gps/blob/master/Adafruit%20Feather%2032u4%20LoRa/GPS_TX.ino
/* @param total_data

   Converts the string with appended data into a char array to send it via LoRA.
*/
void transmit( char* tx_ready ) {

  delay(500);

#if ( DEBUG )
  Serial.println("Data at transmit():");
  Serial.println( tx_ready );
  Serial.println("--------");
#endif

  #if ( TRANSMIT_ENABLED )
    #if ( DEBUG )
    Serial.println("Transmitting...");
    #endif

    rf95.send( ( uint8_t* )tx_ready, TXLIMIT );
    rf95.waitPacketSent();
  #endif
  
#if ( DEBUG )
    Serial.println(" TRANSMITTED SUCCESSFULLY " );
#endif
}

void loop() {

  #if ( GPS_ENABLED )
  digitalWrite( EN_PIN, HIGH );
  #endif
  
  char* _data = recordGPS();

  // Time delay is a function of TD + counter*1000 +1/-1 + Time to receive new NMEA + Time to receive new fix.
  if ( ( millis() - TIM0 > TD ) && GPS.newNMEAreceived() ) {

    strncat( txdata, _data, sizeof(char) * NMEALIMIT );
    memset( _data, 0, sizeof _data );
    free( _data );

    if ( ++counter > 1 ) {
      TIM0 = millis(); // restart counter once two data points have been recorded

      // For future reference, this will be converted from string to char[83*2 + 34] ~= 500 char limit
      _data = recordBME();
      strncat( txdata, _data, sizeof(char) * BMELIMIT );
      memset( _data, 0, sizeof _data );
      free( _data );

#if ( DEBUG )
      Serial.print( txdata );
      Serial.println( "------" );
#endif

      if ( !SD_DISABLED ) {
        // Since SD card is enabled here, prepare the SD Card to be written by turning RFM95_CS off and SD_CS on.

#if ( DEBUG )
        Serial.println(" WRITING TO SD " );
#endif

        digitalWrite( RFM95_CS, HIGH );
        // Write the retrieved data to the SD Card
        //SD.begin( SD_CS );
        currentFile = SD.open( filename.c_str(), FILE_WRITE );
        #if ( DEBUG ) 
          if ( !currentFile ) {
            Serial.println(" SD NOT FOUND " );
          }
        #endif
        currentFile.print( txdata );
        currentFile.println( "------" );
        currentFile.close();

        // Toggle the Chip Select pin for SD Card off, and turn the LoRA Chip Select on.
        //digitalWrite( SD_CS, LOW );
        digitalWrite( RFM95_CS, LOW );
        delay(500);

        // Transmit this data now.
        transmit( txdata );
        if ( rf95.sleep() == false ) {
        //if ( !rf95.sleep() ) {
          #if ( DEBUG )
          Serial.println( " ERROR WITH SLEEP " );
          #endif
          
          #if ( GPS_ENABLED )
          digitalWrite( EN_PIN, LOW );
          #endif
        }

        Watchdog.sleep(4000);
        #if ( DEBUG ) 
          Serial.println(" SLEEP COMPLETED " );
          #endif
        
      }
      counter = 0;
      // TX data does not need to be freed since it is just an array of TXLIMIT SIZE
      // Clear txdata array
      memset( txdata, 0, sizeof txdata );
    }
  }
  else {
    free( _data );
  }
}
