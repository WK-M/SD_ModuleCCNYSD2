#ifndef _NMEA_HELPERS 
#define _NMEA_HELPERS 

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

#endif

