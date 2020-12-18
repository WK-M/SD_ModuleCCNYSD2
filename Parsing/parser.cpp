// This is not guaranteed to work on a Windows Workstation!
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include "NMEA_Sentences.h"

#define DEBUG 0

//Todo: 
// - Maybe try to create a GUI

void add_GGA( std::stringstream& , std::vector< GPGGA >&  );
void add_RMC( std::stringstream& , std::vector< GPRMC >&  );
void add_BME( std::stringstream& , std::vector< BME >&  );
void parse_NMEA_GGA( const std::string&, const std::vector< GPGGA >&, const std::vector< BME >& );
void parse_NMEA_RMC( const std::string&, const std::vector< GPRMC >&, const std::vector< BME >& );
void parse_GEO( const std::string&, std::vector< GPRMC >& );
void print_GGA( const GPGGA& );
void print_RMC( const GPRMC& );

int main( int argc, char* argv[] ) {
    bool f_RMC = false;
    bool f_GGA = false;
    bool f_LATLONG = false;

    if ( argc < 3 ) {
      std::cout << "./parser_exec [File name here] -o [OPTION]" << std::endl;
      exit(0);
    }

    if ( std::string( argv[2] ) == "-o" ) {
        if ( argc < 4 ) {
            std::cout << "Missing arguments!" << std::endl;
            exit(0);
        }
        if ( argc >= 4 && argc <= 7 ) {
            for ( int i = 3; i < argc; i++ ) {
                if ( std::string(argv[i]) == "RMC" ) f_RMC = true;
                else if ( std::string(argv[i]) == "GGA" ) f_GGA = true;
                else if ( std::string(argv[i]) == "GEO" ) f_LATLONG = true;
                else {
                    std::cout << "Option not recognized. Currently the following flags are working:\n"
                            << "RMC - CSV File with RMC NMEA sentence only\n"
                            << "GGA - CSV File with GGA NMEA sentence only\n"
                            << "GEO - CSV File with Latitude and Longitude data only\n"
                            << "Script does support multiple arguments, so outputting both RMC and GGA data will work\n"
                            << "Example: ./parser_exec data.txt -o RMC GGA" << std::endl;
                }
            }
        }
    }
    else {
        std::cout << "The only valid argument is -o. " << std::endl;
        exit(0);
    }

    std::vector< GPGGA > GPGGA_data;
    std::vector< GPRMC > GPRMC_data;
    std::vector< BME > BME_data;

    std::ifstream file(argv[1]);
    std::string line;
    while ( std::getline( file, line ) ) {
        if ( line.empty() == false && line.compare("------") != 0 ) {
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

    // Get time to setup for file
    time_t t = time(0);
    struct tm* now = localtime( &t );

    char buffer[80];
    std::string save_file;

    // Create new csv for GPGGA specifically
    if ( f_GGA == true ) {
        strftime( buffer, 80, "%Y-%m-%d-%S_GPGGA_", now );
        std::string inputted_file = std::string(argv[1]);
        save_file += buffer;
        save_file += inputted_file.substr(0, inputted_file.length() - 4 );
        save_file += ".csv";

        parse_NMEA_GGA( save_file, GPGGA_data , BME_data );
#if DEBUG
        for ( auto data : GPGGA_data ) {
                print_GGA( data );
        }
#endif
    }

    // Create new csv for GPRMC specifically
    if ( f_RMC == true ) {
        strftime( buffer, 80, "%Y-%m-%d-%S_GPRMC_", now );
        std::string inputted_file = std::string(argv[1]);
        save_file = buffer;
        save_file += inputted_file.substr(0, inputted_file.length() - 4 );
        save_file += ".csv";

        parse_NMEA_RMC( save_file, GPRMC_data , BME_data );
#if DEBUG
        for ( auto data : GPRMC_data ) {
                print_RMC( data );
        }
#endif
    }

    if ( f_LATLONG == true ) {
        strftime( buffer, 80, "%Y-%m-%d-%S_GEO_", now );
        std::string inputted_file = std::string(argv[1]);
        save_file = buffer;
        save_file += inputted_file.substr(0, inputted_file.length() - 4 );
        save_file += ".csv";
        parse_GEO( save_file, GPRMC_data );
    }
    return 0;
}

void parse_GEO( const std::string& filename, std::vector< GPRMC >& NMEA_RMC ) {
    std::ofstream file( filename );

    if ( file.is_open() ) {
        for ( auto& NMEA: NMEA_RMC ) {
            std::string r_string = format_LAT( NMEA.Latitude ) + "," + format_LONG( NMEA.Longitude ) + "\n";
        }
        file.close();
    }
    else {
        std::cout << "Error" << std::endl;
    }
    
}


void parse_NMEA_GGA( const std::string& filename, const std::vector< GPGGA >& NMEA_GGA, const std::vector< BME >& BME_GGA ) {
    int i = 0;
    std::ofstream file( filename );
    // Iterate through each vector
    if ( file.is_open() ) {
        for ( auto& NMEA : NMEA_GGA ) {
            std::string r_string = format_UTC( NMEA.UTC ) + "," + format_LAT( NMEA.Latitude ) + "," + format_LONG( NMEA.Longitude ) 
                + "," + std::to_string( NMEA.fix ) + "," + std::to_string( NMEA.num_sat ) + "," 
                +  NMEA.HDOP + "," + NMEA.altitude + "," +  NMEA.geo_separation + "," +  NMEA.last_DGPS 
                + "," + NMEA.station_id + "," + std::to_string(BME_GGA[i].temperature) + ","
                + std::to_string(BME_GGA[i].pressure) + "," 
                + std::to_string(BME_GGA[i].altitude) + ","
                + std::to_string(BME_GGA[i].humidity);
            r_string += '\n';
            ++i;
            file << r_string;
        }

        file.close();
    }
    else {
        std::cout << "Error" << std::endl;
    }
}

void parse_NMEA_RMC( const std::string& filename, const std::vector< GPRMC >& NMEA_RMC, const std::vector< BME >& BME_RMC ) {
    // Iterate through each vector
    int i = 0;
    std::ofstream file( filename );
    if ( file.is_open() ) {
        for ( auto& NMEA : NMEA_RMC ) {
            std::string r_string = format_UTC( NMEA.UTC ) + "," + NMEA.status + "," + format_LAT( NMEA.Latitude ) + "," 
                + format_LONG( NMEA.Longitude ) + "," + std::to_string( NMEA.SOG ) + "," + std::to_string( NMEA.COG ) + "," 
                + format_date( NMEA.current_date ) + ",";
            r_string += NMEA.mag_var[0];
            r_string += NMEA.mag_var[1];
            r_string += NMEA.mag_var[2];
            r_string += NMEA.mag_var[3];
            r_string += NMEA.mag_var[4];
            r_string += ","; 
            r_string += NMEA.ew_indicator; 
            r_string += ","; 
            r_string += NMEA.mode;
            r_string += "," + std::to_string(BME_RMC[i].temperature) + "," 
                + std::to_string(BME_RMC[i].pressure) + "," 
                + std::to_string(BME_RMC[i].altitude) + "," 
                + std::to_string(BME_RMC[i].humidity);
            r_string += '\n';
            ++i;
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
    if ( temp.length() == 9 ) { 
        GGA.Latitude.degrees[0] = temp[0];
        GGA.Latitude.degrees[1] = temp[1];

        GGA.Latitude.minutes_bd[0] = temp[2];
        GGA.Latitude.minutes_bd[1] = temp[3];

        GGA.Latitude.minutes_ad[0] = temp[5];
        GGA.Latitude.minutes_ad[1] = temp[6];
        GGA.Latitude.minutes_ad[2] = temp[7];
        GGA.Latitude.minutes_ad[3] = temp[8];
    }
    else {
        GGA.Latitude.degrees[0] = '\0';
        GGA.Latitude.degrees[1] = '\0';

        GGA.Latitude.minutes_bd[0] = '\0';
        GGA.Latitude.minutes_bd[1] = '\0';

        GGA.Latitude.minutes_ad[0] = '\0';
        GGA.Latitude.minutes_ad[1] = '\0';
        GGA.Latitude.minutes_ad[2] = '\0';
        GGA.Latitude.minutes_ad[3] = '\0';
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
    else {
        GGA.Longitude.degrees[0] = '\0';
        GGA.Longitude.degrees[1] = '\0';
        GGA.Longitude.degrees[2] = '\0';

        GGA.Longitude.minutes_bd[0] = '\0';
        GGA.Longitude.minutes_bd[1] = '\0';

        GGA.Longitude.minutes_ad[0] = '\0';
        GGA.Longitude.minutes_ad[1] = '\0';
        GGA.Longitude.minutes_ad[2] = '\0';
        GGA.Longitude.minutes_ad[3] = '\0';
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

void add_BME( std::stringstream& ss_BME, std::vector< BME >& BME_data) {
    BME data;
    std::string temp; 

    // Get UTC data
    std::getline( ss_BME, temp, ',' );
    data.pressure = std::stof( temp );

    std::getline( ss_BME, temp,  ',' );
    data.temperature = std::stof( temp );

    std::getline( ss_BME, temp,  ',' );
    data.altitude = std::stof( temp );

    std::getline( ss_BME, temp,  ',' );
    data.humidity = std::stof( temp );

    BME_data.push_back( data );
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
    else {
        memset(RMC.Latitude.degrees, '\0' , 2);
        memset(RMC.Latitude.minutes_bd, '\0', 2);
        memset(RMC.Latitude.minutes_ad, '\0', 4);
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
    else {
        RMC.Longitude.degrees[0] = '\0';
        RMC.Longitude.degrees[1] = '\0';
        RMC.Longitude.degrees[2] = '\0';

        RMC.Longitude.minutes_bd[0] = '\0';
        RMC.Longitude.minutes_bd[1] = '\0';

        RMC.Longitude.minutes_ad[0] = '\0';
        RMC.Longitude.minutes_ad[1] = '\0';
        RMC.Longitude.minutes_ad[2] = '\0';
        RMC.Longitude.minutes_ad[3] = '\0';
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

    if ( !temp.empty() ) {
        RMC.mag_var[0] = temp[0];
        RMC.mag_var[1] = temp[1];
        RMC.mag_var[2] = temp[2];
        RMC.mag_var[3] = '.';
        RMC.mag_var[4] = temp[4];
    }
    else {
        memset(RMC.mag_var, 0, 5 );
    }


    // Assume char 
    std::getline( prmcs_ss, temp, ',' ); 
    RMC.ew_indicator = ( temp.length() == 1 ) ? temp[0] : '\0';

    // Assume char
    std::getline( prmcs_ss, temp, ',' ); 
    RMC.mode = temp[0];

    pgs_GPRMC_data.push_back( RMC );
}

// Debugging
void print_GGA( const GPGGA& p_GGA ) {
    std::cout << print_time( p_GGA.UTC ) << "," << print_latitude( p_GGA.Latitude ) << "," << print_longitude( p_GGA.Longitude ) << ","
        << p_GGA.fix << "," << p_GGA.num_sat << "," << p_GGA.HDOP <<  "," << p_GGA.altitude << "," 
        << p_GGA.geo_separation << "," << p_GGA.last_DGPS << "," << p_GGA.station_id << std::endl;
}

void print_RMC( const GPRMC& p_RMC ) {
    std::cout << print_time( p_RMC.UTC ) << "," << p_RMC.status  << ","
        << print_latitude( p_RMC.Latitude ) << "," << print_longitude( p_RMC.Longitude ) << ","  
        << p_RMC.SOG << "," << p_RMC.COG << ","
        << print_date( p_RMC.current_date ) << ","
        << p_RMC.mag_var << "," << p_RMC.ew_indicator << "," << p_RMC.mode << std::endl;
}
