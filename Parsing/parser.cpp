// Takes in textfile as argument, delimiter is a comma
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include "NMEA_Sentences.h"

#define DEBUG 0


void parse_GGA_sentence( std::stringstream& , std::vector< GPGGA >&  );
void parse_RMC_sentence( std::stringstream& , std::vector< GPRMC >&  );

void print_GGA( GPGGA& );
void print_RMC( GPRMC& );

int main( int argc, char* argv[] ) {
    /*if ( argc < 2 ) {
      std::cout << "Incorrect number of arguments." << std::endl;
      exit(0);
      }*/

    std::vector< GPGGA > GPGGA_data;
    std::vector< GPRMC > GPRMC_data;

    std::ifstream file("data.txt");
    std::string line;
    while ( std::getline( file, line ) ) {
        std::stringstream ss( line );
        std::string GGA_RMC;
        std::getline( ss, GGA_RMC, ',');

        #if ( DEBUG )
            std::cout << line << std::endl;
#endif 
        if ( !GGA_RMC.compare("$GPGGA") ) {
            parse_GGA_sentence( ss, GPGGA_data ); 
        }
        else {
            parse_RMC_sentence( ss, GPRMC_data );
        }
    }
    return 0;
}

void parse_GGA_sentence( std::stringstream& pggas_ss, std::vector< GPGGA >& pgs_GPGGA_data ) {
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
    GGA.HDOP = std::stof( temp );

    std::getline( pggas_ss, temp, ',' );
    GGA.altitude = std::stof( temp );

    std::getline( pggas_ss, temp, ',' );

    std::getline( pggas_ss, temp, ',' );
    GGA.geo_separation = std::stof( temp );

    std::getline( pggas_ss, temp, ',' );

    std::getline( pggas_ss, temp, ',' );
    GGA.last_DGPS = std::stof(temp);

    std::getline( pggas_ss, temp, ',' );
    strncpy( GGA.station_id, temp.c_str(), 4 );

    pgs_GPGGA_data.push_back( GGA );
}

void parse_RMC_sentence( std::stringstream& prmcs_ss, std::vector< GPRMC >& pgs_GPRMC_data ) {
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

    std::getline( prmcs_ss, temp, ',' );
    RMC.SOG = std::stof( temp );

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

    std::getline( prmcs_ss, temp, ',' );
    RMC.mag_var[0] = temp[0];
    RMC.mag_var[1] = temp[1];
    RMC.mag_var[2] = temp[2];
    RMC.mag_var[3] = '.';
    RMC.mag_var[4] = temp[4];


    std::getline( prmcs_ss, temp, ',' ); 
    RMC.ew_indicator = temp[0];

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
