#include <SD.h>
#include <TimeLib.h>
#include <Adafruit_GPS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console (Used for LOCUS)
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false
#define file_extension ".txt"
#define GPSSerial Serial1
#define SEALEVELPRESSURE_HPA (1013.25)
#define DEBUG 1

const int CS = 10;
volatile int counter;

File currentFile;
Adafruit_GPS GPS(&GPSSerial);
Adafruit_BME280 bme; // I2C

bool BIL = false;
String filename = "";

// For setup, it needs to take < 10 seconds
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  initializeSD();
  delay(1000);
  initializeGPS();
  delay(1000);
  initializeBME();
  delay(1000);
  counter = 0;
}

void initializeBME(void) {
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
  Serial.println("-- BME 280 Initialized --");
}

void initializeSD(void) {
  // If it fails to start using the selected port (default D10)
  // Check to see if SD CARD exists, if SD Card exists then logging will be directly sent to SD CARD
  // If it fails, then logging will take place using LOCUS (in-built logging)
  if (!SD.begin(CS) & DEBUG ) {
    Serial.println("SD Card not connected");
    BIL = true;
  }
  else {
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

void initializeGPS(void) {
  // Initialize GPS
  Serial.println("Starting GPS...");
  GPS.begin(9600);

  // IF BIL(Built-in Logging) is true, start LOCUS "$PMTK622, 1*29"
  if (BIL) {

    GPS.sendCommand( PMTK_SET_NMEA_OUTPUT_OFF );

    while (GPSSerial.available())
      GPSSerial.read();

    delay(1000);
    GPS.sendCommand( "$PMTK622,1*29" );
    Serial.println("----------------------------------------------------");

    // Start logging usinh LOCUS
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
    
    GPS.println( PMTK_Q_RELEASE );
    
    delay(2000);
  }
  Serial.println("GPS Init Done");
}

void recordGPS(void) {
  char c = GPS.read();
  if ( GPS.newNMEAreceived() ) {
    if ( DEBUG ) {
      Serial.print(GPS.lastNMEA());
    }
    // write data to current file
    currentFile.print(GPS.lastNMEA());
    
    ++counter;
  }
  if ( counter == 3 ) {
    recordBME();
    counter = 0;
  }
}

// BME Data outputted as follows:
// 1. Temperature (Celsius)
// 2. Pressure (Pscals)
// 3. Altitude (Meters)
// 4. Humidity (Percentage)
void recordBME(void) {
  String data_bme = String( bme.readTemperature(), 4 ) + ", " + String( bme.readPressure() / 100.0F, 4 ) + ", " + String( bme.readAltitude( SEALEVELPRESSURE_HPA ), 4 )
                    + ", " + String( bme.readHumidity() , 4 );
  currentFile.println(data_bme);
  currentFile.println("---------");
  if ( DEBUG ) {
    Serial.println(data_bme);
    Serial.println("---------");  
  }
}

void loop() {

  // From PMTK_SET_NMEA_UPDATE_1HZ, it is recorded every 1 second
  currentFile = SD.open( filename.c_str(), FILE_WRITE );
  recordGPS();
  currentFile.close();

}
