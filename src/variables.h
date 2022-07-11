#include <Arduino.h>

struct tm tm;

String wochentage[7]={"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
#define MY_NTP_SERVER "de.pool.ntp.org"
#define TFT_CAL_FILE "/settings/touchdata"
#define REPEAT_CAL false
#define FORMAT_LITTLEFS_IF_FAILED true

// choose your time zone from this list
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
time_t now;    // this is the epoch

int backlight = 90;