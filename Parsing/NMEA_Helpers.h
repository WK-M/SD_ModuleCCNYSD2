#ifndef _NMEA_HELPERS 
#define _NMEA_HELPERS 

#include <string>

struct Date {
    char day[2];
    char month[2];
    char year[2];
};

struct Time {
    char hour[2];
    char minute[2];
    char seconds[2];
    char dec_seconds[3]; // After decimal
};

struct Lat {
    char degrees[2];
    char minutes_bd[2];
    char minutes_ad[4];
    char vert;
};

struct Long {
    char degrees[3];
    char minutes_bd[2];
    char minutes_ad[4];
    char horiz;
};

std::string print_date( const struct Date& d ) {
    std::string r_string;

    r_string += d.day[0]; 
    r_string += d.day[1];
    r_string += d.month[0];
    r_string += d.month[1];
    r_string += d.year[0];
    r_string += d.year[1];
    return r_string;
}

std::string format_date( const struct Date& d ) {
    std::string r_string;

    r_string += d.month[0];
    r_string += d.month[1];
    r_string += "/";
    r_string += d.day[0]; 
    r_string += d.day[1];
    r_string += "/";
    r_string += d.year[0];
    r_string += d.year[1];
    return r_string;
}

std::string format_UTC( const struct Time& t ) {

    std::string r_string;

    r_string += t.hour[0];
    r_string += t.hour[1];
    r_string += ":";
    r_string += t.minute[0];
    r_string += t.minute[1];
    r_string += ":";
    r_string += t.seconds[0];
    r_string += t.seconds[1];
    r_string += ".";
    r_string += t.dec_seconds[0];
    r_string += t.dec_seconds[1];
    r_string += t.dec_seconds[2];
    return r_string;

}

std::string print_time( const struct Time& t ) {
    std::string r_string;

    r_string += t.hour[0];
    r_string += t.hour[1];
    r_string += t.minute[0];
    r_string += t.minute[1];
    r_string += t.seconds[0];
    r_string += t.seconds[1];
    r_string += ".";
    r_string += t.dec_seconds[0];
    r_string += t.dec_seconds[1];
    r_string += t.dec_seconds[2];
    return r_string;
}

std::string format_LAT( const struct Lat& L ) {
    std::string r_string;

    if ( L.vert == 'N' ) {
        r_string = "+";
    }
    else if ( L.vert  == 'S') {
        r_string = "-";
    }
    else {
        r_string = "";
    }
    
    if ( L.degrees[0] != '\0' ) {
        r_string += L.degrees[0];
        r_string += L.degrees[1];
        r_string += " ";
        r_string += L.minutes_bd[0]; 
        r_string += L.minutes_bd[1];
        r_string += ".";
        r_string += L.minutes_ad[0];
        r_string += L.minutes_ad[1];
        r_string += L.minutes_ad[2];
        r_string += L.minutes_ad[3];
        r_string += "\'";
    }
    else {
        r_string += " ";
    }

    return r_string;
}

std::string format_LONG( const struct Long& L ) {
    std::string r_string;

    if ( L.horiz == 'E' ) {
        r_string = "+";
    }
    else if ( L.horiz  == 'W') {
        r_string = "-";
    }
    else {
        r_string = "";
    }

    if ( L.degrees[0] != '\0' ) {

        r_string += L.degrees[0];
        r_string += L.degrees[1];
        r_string += L.degrees[2];
        r_string += " ";
        r_string += L.minutes_bd[0]; 
        r_string += L.minutes_bd[1];
        r_string += ".";
        r_string += L.minutes_ad[0];
        r_string += L.minutes_ad[1];
        r_string += L.minutes_ad[2];
        r_string += L.minutes_ad[3];
        r_string += "\'";

    }
    else {
        r_string += "";
    }

    return r_string;
}
std::string print_latitude( const struct Lat& L ) {
    std::string r_string;

    r_string += L.degrees[0];
    r_string += L.degrees[1];
    r_string += L.minutes_bd[0];
    r_string += L.minutes_bd[1];
    r_string += ".";
    r_string += L.minutes_ad[0];
    r_string += L.minutes_ad[1];
    r_string += L.minutes_ad[2];
    r_string += L.minutes_ad[3];
    r_string += ",";
    r_string += L.vert;
    return r_string;
}

std::string print_longitude( const struct Long& L ) {
    std::string r_string;

    r_string += L.degrees[0];
    r_string += L.degrees[1];
    r_string += L.minutes_bd[0];
    r_string += L.minutes_bd[1];
    r_string += ".";
    r_string += L.minutes_ad[0];
    r_string += L.minutes_ad[1];
    r_string += L.minutes_ad[2];
    r_string += L.minutes_ad[3];
    r_string += ",";
    r_string += L.horiz;
    return r_string;
}

#endif
