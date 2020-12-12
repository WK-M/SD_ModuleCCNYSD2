// Takes in textfile as argument, delimiter is a comma
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include "NMEA_Sentences.h"

#define DEBUG 1

void add_GGA( std::stringstream& , std::vector< GPGGA >&  );
void add_RMC( std::stringstream& , std::vector< GPRMC >&  );
void add_BME( std::stringstream& , std::vector< BME >&  );
void parse_NMEA_GGA( const char*, const std::vector< GPGGA >&, const std::vector< BME >& );
void parse_NMEA_RMC( const char*, const std::vector< GPRMC >&, const std::vector< BME >& );
void print_GGA( GPGGA& );
void print_RMC( GPRMC& );

int main( int argc, char* argv[] ) {
    /*if ( argc < 2 ) {
      std::cout << "Incorrect number of arguments." << std::endl;
      exit(0);
      }*/

    std::vector< GPGGA > GPGGA_data;
    std::vector< GPRMC > GPRMC_data;
    std::vector< BME > BME_data;

    std::ifstream file("data.txt");
    std::string line;
    while ( std::getline( file, line ) ) {
        if ( line.empty() == false ) {
            std::stringstream ss( line );
            std::string GGA_RMC;
            std::getline( ss, GGA_RMC, ',');

#if ( DEBUG )
            std::cout << line << std::endl;
#endif 
            if ( GGA_RMC.compare("$GPGGA") == 0) {
                add_GGA( ss, GPGGA_data ); 
            }
            else if ( GGA_RMC.compare("$GPRMC") == 0 ) {
                add_RMC( ss, GPRMC_data );
            }
            else {
                // BMEhere
                add_BME( ss, BME_data );
            }

        }
    }

    int option = 0;

    // Get time to setup for file
    time_t t = time(0);
      struct tm* now = localtime( &t );

      char buffer[80];

    // Create new csv for GPGGA specifically
    if ( option == 0 ) {
    strftime( buffer, 80, "%Y-%m-%d-%S_GPGGA.txt", now );
    }

    // Create new csv for GPRMC specifically
    else {
    strftime( buffer, 80, "%Y-%m-%d-%S_GPRMC.txt", now );
    }

    // Open new file and write GPGGA only or GPRMC only
    parse_NMEA_GGA( buffer, GPGGA_data , BME_data );
    parse_NMEA_RMC( buffer, GPRMC_data , BME_data );

    return 0;
}

void add_BME( std::stringstream& ss_BME, std::vector< BME >& RMC_GGA_data, std::vector< BME > BME_data) {
    BME data;
    std::string temp; 

    // Get UTC data
    std::getline( ss_BME, temp,  ',' );
    data.pressure = std::stof( temp );

    std::getline( ss_BME, temp,  ',' );
    data.temperature = std::stof( temp );

    std::getline( ss_BME, temp,  ',' );
    data.altitude = std::stof( temp );

    std::getline( ss_BME, temp,  ',' );
    data.humidity = std::stof( temp );

    RMC_GGA_data.push_back( data );
}

void parse_NMEA_GGA( const char* filename, const std::vector< GPGGA >& NMEA_GGA ) {
    std::ofstream file( filename );
    // Iterate through each vector
    if ( file.is_open() ) {
        for ( auto& NMEA : NMEA_GGA ) {
            std::string r_string = format_UTC( NMEA.UTC ) + "," + format_LAT( NMEA.Latitude ) + "," + format_LONG( NMEA.Longitude ) + "," + std::to_string( NMEA.fix ) + "," + std::to_string( NMEA.num_sat ) + "," +  NMEA.HDOP + "," + NMEA.altitude + "," +  NMEA.geo_separation + "," +  NMEA.last_DGPS + "," + NMEA.station_id;
            /*r_string += NMEA.station_id[0];
              r_string += NMEA.station_id[1];
              r_string += NMEA.station_id[2];
              r_string += NMEA.station_id[3];*/
            r_string += '\n';

            file << r_string;
        }

        file.close();
    }
    else {
        std::cout << "Error" << std::endl;
    }
}

void parse_NMEA_RMC( const char* filename, const std::vector< GPRMC >& NMEA_RMC ) {
    // Iterate through each vector
    std::ofstream file( filename );
    if ( file.is_open() ) {
        for ( auto& NMEA : NMEA_RMC ) {
            std::string r_string = format_UTC( NMEA.UTC ) + "," + NMEA.status + "," + format_LAT( NMEA.Latitude ) + "," + format_LONG( NMEA.Longitude ) + "," + std::to_string( NMEA.SOG) + "," + std::to_string( NMEA.COG ) + "," + format_date( NMEA.current_date ) + ",";
            r_string += NMEA.mag_var;
            r_string += ","; 
            r_string += NMEA.ew_indicator; 
            r_string += ","; 
            r_string += NMEA.mode;
            r_string += '\n';

            file << r_string;
        }
        file.close();
    }
    else {
        std::cout << "Error" << std::endl;
    }
}

void add_GGA( std::stringstream& pggas_ss, std::vector< GPGGA >& pgs_GPGGA_data ) {
    GPGGA GGA;
    std::string temp; 

    // Get UTC data
    std::getline( pggas_ss, temp,  ',' );
    if ( temp.length() == 10 ) { 
        GGA.UTC.hour[0] = temp.at(0);
        GGA.UTC.hour[1] = temp.at(1);

        GGA.UTC.minute[0] = temp[2];
        GGA.UTC.minute[1] = temp[3];

        GGA.UTC.seconds[0] = temp[4];
        GGA.UTC.seconds[1] = temp[5];
        // 6 = '.' can be ignored
        GGA.UTC.dec_seconds[0] = temp[7];
        GGA.UTC.dec_seconds[1] = temp[8];
        GGA.UTC.dec_seconds[2] = temp[9];
    }


    std::getline( pggas_ss, temp, ',' );
    // Latitude
    if ( temp.length() == 9) { 
        GGA.Latitude.degrees[0] = temp[0];
        GGA.Latitude.degrees[1] = temp[1];

        GGA.Latitude.minutes_bd[0] = temp[2];
        GGA.Latitude.minutes_bd[1] = temp[3];

        GGA.Latitude.minutes_ad[0] = temp[5];
        GGA.Latitude.minutes_ad[1] = temp[6];
        GGA.Latitude.minutes_ad[2] = temp[7];
        GGA.Latitude.minutes_ad[3] = temp[8];

    }

    // Get North/South
    std::getline( pggas_ss, temp, ',' ); // get N or S
    GGA.Latitude.vert = temp[0];

    // Get Longitude
    std::getline( pggas_ss, temp, ',' );
    if ( temp.length() == 10 ) {
        GGA.Longitude.degrees[0] = temp[0];
        GGA.Longitude.degrees[1] = temp[1];
        GGA.Longitude.degrees[2] = temp[2];

        GGA.Longitude.minutes_bd[0] = temp[3];
        GGA.Longitude.minutes_bd[1] = temp[4];

        GGA.Longitude.minutes_ad[0] = temp[6];
        GGA.Longitude.minutes_ad[1] = temp[7];
        GGA.Longitude.minutes_ad[2] = temp[8];
        GGA.Longitude.minutes_ad[3] = temp[9];
    }

    std::getline( pggas_ss, temp, ',' );
    GGA.Longitude.horiz = temp[0];

    std::getline( pggas_ss, temp, ',' );
    GGA.fix = std::stoi( temp );

    std::getline( pggas_ss, temp, ',' );
    GGA.num_sat = std::stoi( temp );

    std::getline( pggas_ss, temp, ',' );
    GGA.HDOP = temp;

    std::getline( pggas_ss, temp, ',' );
    GGA.altitude = temp;

    // Skip M
    std::getline( pggas_ss, temp, ',' );

    std::getline( pggas_ss, temp, ',' );
    GGA.geo_separation = temp;

    // Skip M
    std::getline( pggas_ss, temp, ',' );

    std::getline( pggas_ss, temp, ',' );
    GGA.last_DGPS = temp;

    std::getline( pggas_ss, temp, ',' );
    strncpy( GGA.station_id, temp.c_str(), 4 );

    pgs_GPGGA_data.push_back( GGA );
}

void add_RMC( std::stringstream& prmcs_ss, std::vector< GPRMC >& pgs_GPRMC_data ) {
    GPRMC RMC;
    std::string temp; 

    // Get UTC data
    std::getline( prmcs_ss, temp,  ',' );
    if ( temp.length() == 10 ) { 
        RMC.UTC.hour[0] = temp[0];
        RMC.UTC.hour[1] = temp[1];

        RMC.UTC.minute[0] = temp[2];
        RMC.UTC.minute[1] = temp[3];

        RMC.UTC.seconds[0] = temp[4];
        RMC.UTC.seconds[1] = temp[5];
        // 6 = '.' can be ignored
        RMC.UTC.dec_seconds[0] = temp[7];
        RMC.UTC.dec_seconds[1] = temp[8];
        RMC.UTC.dec_seconds[2] = temp[9];
    }

    std::getline( prmcs_ss, temp, ',' );
    RMC.status = temp[0];

    std::getline( prmcs_ss, temp, ',' );
    // Latitude
    if ( temp.length() == 9) { 
        RMC.Latitude.degrees[0] = temp[0];
        RMC.Latitude.degrees[1] = temp[1];

        RMC.Latitude.minutes_bd[0] = temp[2];
        RMC.Latitude.minutes_bd[1] = temp[3];

        RMC.Latitude.minutes_ad[0] = temp[5];
        RMC.Latitude.minutes_ad[1] = temp[6];
        RMC.Latitude.minutes_ad[2] = temp[7];
        RMC.Latitude.minutes_ad[3] = temp[8];

    }

    // Get North/South
    std::getline( prmcs_ss, temp, ',' ); // get N or S
    RMC.Latitude.vert = temp[0];

    // Get Longitude
    std::getline( prmcs_ss, temp, ',' );
    if ( temp.length() == 10 ) {
        RMC.Longitude.degrees[0] = temp[0];
        RMC.Longitude.degrees[1] = temp[1];
        RMC.Longitude.degrees[2] = temp[2];

        RMC.Longitude.minutes_bd[0] = temp[3];
        RMC.Longitude.minutes_bd[1] = temp[4];

        RMC.Longitude.minutes_ad[0] = temp[6];
        RMC.Longitude.minutes_ad[1] = temp[7];
        RMC.Longitude.minutes_ad[2] = temp[8];
        RMC.Longitude.minutes_ad[3] = temp[9];
    }

    std::getline( prmcs_ss, temp, ',' );
    RMC.Longitude.horiz = temp[0];

    // Keep as float
    std::getline( prmcs_ss, temp, ',' );
    RMC.SOG = std::stof( temp );

    // Keep as float
    std::getline( prmcs_ss, temp, ',' );
    RMC.COG = std::stof( temp );

    std::getline( prmcs_ss, temp, ',' );
    if ( temp.length() == 6 ) {
        RMC.current_date.day[0] = temp[0];
        RMC.current_date.day[1] = temp[1];

        RMC.current_date.month[0] = temp[2];
        RMC.current_date.month[1] = temp[3];

        RMC.current_date.year[0] = temp[4];
        RMC.current_date.year[1] = temp[5];
    }

    // Assume string
    std::getline( prmcs_ss, temp, ',' );
    RMC.mag_var[0] = temp[0];
    RMC.mag_var[1] = temp[1];
    RMC.mag_var[2] = temp[2];
    RMC.mag_var[3] = '.';
    RMC.mag_var[4] = temp[4];


    // Assume char 
    std::getline( prmcs_ss, temp, ',' ); 
    RMC.ew_indicator = temp[0];

    // Assume char
    std::getline( prmcs_ss, temp, ',' ); 
    RMC.mode = temp[0];

    pgs_GPRMC_data.push_back( RMC );
}

// Debugging
void print_GGA( GPGGA& p_GGA ) {
    std::cout << print_time( p_GGA.UTC ) << "," << print_latitude( p_GGA.Latitude ) << "," << print_longitude( p_GGA.Longitude ) << ","
        << p_GGA.fix << "," << p_GGA.num_sat << "," << p_GGA.HDOP <<  "," << p_GGA.altitude << "," 
        << p_GGA.geo_separation << "," << p_GGA.last_DGPS << "," << p_GGA.station_id << std::endl;
}

void print_RMC( GPRMC& p_RMC ) {
    std::cout << print_time( p_RMC.UTC ) << "," << p_RMC.status  << ","
        << print_latitude( p_RMC.Latitude ) <<  "," << print_longitude( p_RMC.Longitude ) << ","
        << p_RMC.SOG << "," << p_RMC.COG << ","
        << print_date( p_RMC.current_date ) << ","
        << p_RMC.mag_var << "," << p_RMC.ew_indicator << "," << p_RMC.mode << std::endl;
}
