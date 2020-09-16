#include <SD.h>
#include <TimeLib.h>
#include <Adafruit_GPS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO true
#define file_extension ".txt"
#define GPSSerial Serial1
#define SEALEVELPRESSURE_HPA (1013.25)

const int CS = 10;
volatile int counter;

File currentFile;
Adafruit_GPS GPS(&GPSSerial);
Adafruit_BME280 bme; // I2C

bool BIL = false;
String filename = "";

// For setup, it needs to take < 10 seconds
void setup() {
  // 9600 for SD CARD
  // 115200 for GPS, probably need to use 115200
  Serial.begin(115200);
  initializeSD();
  delay(1000);
  initializeGPS();
  delay(1000);
  initializeBME();
  delay(1000);
}

void initializeBME(void) {
  unsigned status = bme.begin();
  
  if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
  }

  Serial.println("-- Default Test --");
}

void initializeSD(void) {
  pinMode(CS, OUTPUT); // Slave Select for SPI

  // If it fails to start using the selected port (default D10)
  // Check to see if SD CARD exists, if SD Card exists then logging will be directly sent to SD CARD
  // If it fails, then logging will take place using LOCUS (in-built logging)
  if (!SD.begin(CS)) {
    Serial.println("SD Card not connected");
    BIL = true;
  }
  else {
    Serial.println("Creating file name...");
    // Get the current date
    // MMDDYYNN, MM - Month, DD - day, YY - Year, NN - 00, 01, NN
    int n = 0;
    // Create temporary string to hold new file name
    filename = String(month(), DEC) + String(day(), DEC) + String(year(), DEC) + String(n, DEC) + file_extension;

    // Get new file name
    while (SD.exists(filename.c_str())) {
      ++n;
      filename = String(month(), DEC) + String(day(), DEC) + String(year(), DEC) + String(n, DEC) + file_extension;
    }
    //currentFile = SD.open(filename.c_str(), FILE_WRITE);
  } 
}

void initializeGPS(void) {
  // Initialize GPS
  Serial.println("Starting GPS...");
  GPS.begin(9600);

  // IF BIL(Built-in Logging) is true, start LOCUS "$PMTK622, 1*29"
  if (BIL) {

    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);

    while (GPSSerial.available())
       GPSSerial.read();
  
    delay(1000);
    GPS.sendCommand("$PMTK622,1*29");
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
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    
    // Default is 1 Hz update rate
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    
    GPS.sendCommand(PGCMD_ANTENNA);
    delay(1000);
    
    GPS.println(PMTK_Q_RELEASE);
  }
  Serial.println("Init Done");
}

void recordGPS(void) {
  char c = GPS.read();
  if ( GPS.newNMEAreceived() ) {
    // write data to current file
    currentFile.println(GPS.lastNMEA());
  }
}

void recordBME(void) {
  String data_bme = String( bme.readTemperature(), 4 ) + ", " + String( bme.readPressure() / 100.0F, 4 )+ ", " + String( bme.readAltitude( SEALEVELPRESSURE_HPA ), 4 ) 
              + ", " + String( bme.readHumidity() , 4 );
  currentFile.println(data_bme);
}

void loop() {

  // From PMTK_SET_NMEA_UPDATE_1HZ, it is recorded every 1 second
  currentFile = SD.open(filename.c_str(), FILE_WRITE);
  recordGPS();
  recordBME();
  currentFile.close();
}
