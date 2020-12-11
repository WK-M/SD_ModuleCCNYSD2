// When using this script, make sure that it is either in .csv or .txt format
// Also, ensure that PMTK Sentences are removed, the first line should eithe be a NMEA sentence (GPGGA/GPRMC)
// This script allows data to be read from the .txt file from the SD Card stored during the TX_LoRAvX_X.ino file and 
// outputs it to a specific .csv file for further specification.
#include <iostream>
#include "NMEA_Helpers.h"

/* GPGGA contains the following:
 * Time (UTC)
 * Latitude
 * Longitude
 * Fix
 * Number of Satellites
 * HDOP
 * Altitude
 * Height of Geoid above WGS84
 * Time since last DGPS (Differential GPS) update
 * DGPS Reference Station ID
 * Checksum
 */
typedef struct NMEA1 {
    struct Time UTC;
    struct Lat Latitude;
    struct Long Longitude;
    int fix;
    int num_sat;
    float HDOP;
    float altitude;
    float geo_separation;
    float last_DGPS;
    char station_id[4];
} GPGGA;

/* GPRMC contains the following:
 * Time (UTC)
 * Status
 * Latitude
 * Longitude
 * Speed Over Ground
 * Course Over Ground
 * Date
 * Magnetic Variation
 * East/West Indicator
 * Mode
 * Checksum
 */
typedef struct NMEA2 {
    struct Time UTC;
    char status;
    struct Lat Latitude;
    struct Long Longitude;
    float SOG;
    float COG;
    struct Date current_date;
    char mag_var[5];
    char ew_indicator;
    char mode;
} GPRMC;

