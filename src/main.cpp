#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ESPConnect.h>
#include <time.h>
#include "esp_sntp.h"



TFT_eSPI tft=TFT_eSPI();
AsyncWebServer server(80);

struct tm tm;

String wochentage[7]={"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
#define MY_NTP_SERVER "at.pool.ntp.org"

// choose your time zone from this list
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
time_t now;    // this is the epoch



void showTime() {
 time(&now); // read the current time
 localtime_r(&now, &tm); // update the structure tm with the current time
 String aktZeit = "";
 String aktDatum = "";
 aktDatum = wochentage[tm.tm_wday];
 aktDatum += ", " + (tm.tm_mday);
 aktDatum += "." + tm.tm_mon+1 + "." + tm.tm_year;
 Serial.print("year:");
 Serial.print(tm.tm_year + 1900); // years since 1900
 Serial.print("\tmonth:");
 Serial.print(tm.tm_mon + 1); // January = 0 (!)
 Serial.print("\tday:");
 Serial.print(tm.tm_mday); // day of month
 Serial.print("\thour:");
 Serial.print(tm.tm_hour); // hours since midnight 0-23
 Serial.print("\tmin:");
 Serial.print(tm.tm_min); // minutes after the hour 0-59
 Serial.print("\tsec:");
 Serial.print(tm.tm_sec); // seconds after the minute 0-61*
 Serial.print("\twday");
 Serial.print(wochentage[tm.tm_wday]); // days since Sunday 0-6
 if (tm.tm_isdst == 1) // Daylight Saving Time flag
  Serial.print("\tDST");
 else
  Serial.print("\tstandard");
 Serial.println();

}

void cbSyncTime(struct timeval *tv)  // callback function to show when NTP was synchronized
{
  Serial.println(F("NTP time synched"));
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK,TFT_WHITE);
  
  tft.fillRoundRect(0, 0, 320, 16, 1, TFT_LIGHTGREY);
  tft.drawRoundRect(0,0,320,16,1,TFT_BLACK);
  tft.fillRoundRect(0, 224, 320, 240, 1, TFT_LIGHTGREY);
  tft.drawRoundRect(0,224,320,240,1,TFT_BLACK);

  /* AutoConnect AP Configure SSID and password for Captive Portal */
  ESPConnect.autoConnect("ESPConfig");
  /* Begin connecting to previous WiFi or start autoConnect AP if unable to connect */
  if(ESPConnect.begin(&server)){
    Serial.println("Connected to WiFi");
    Serial.println("IPAddress: "+WiFi.localIP().toString());
  }else{
    Serial.println("Failed to connect to WiFi");
  }
  Serial.println(WiFi.localIP());
  sntp_set_time_sync_notification_cb(cbSyncTime);  // set a Callback function for time synchronization notification
  sntp_set_sync_interval(12 * 60 * 60 * 1000UL); // 12 hours
  configTime(0, 0, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line
  setenv("TZ", MY_TZ, 1); // Set environment variable with your time zone
  tzset();
}

void loop() {
  // put your main code here, to run repeatedly:
  showTime();
  delay(1000);
}