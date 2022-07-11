#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>

#include <WiFi.h>
#include <TFT_eSPI.h>
#include <ESPConnect.h>
#include <time.h>
#include "esp_sntp.h"


TFT_eSPI tft=TFT_eSPI();
AsyncWebServer server(80);

#include "variables.h"
#include "myfunctions.h"
#include "crontab.h" 




void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  

  // check if calibration file exists and size is correct
  if (LittleFS.exists(TFT_CAL_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      LittleFS.remove(TFT_CAL_FILE);
    }
    else
    {
      File f = LittleFS.open(TFT_CAL_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = LittleFS.open(TFT_CAL_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void showTime() {
 time(&now); // read the current time
 localtime_r(&now, &tm); // update the structure tm with the current time
 String aktZeit = "";
 String aktDatum = "";
 aktDatum = wochentage[tm.tm_wday];
 
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
  setupBrightnessControl();
  setBrightness(0);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK,TFT_WHITE);
  // Draw TopBar
  tft.fillRoundRect(0, 0, 320, 16, 1, TFT_LIGHTGREY);
  tft.drawRoundRect(0,0,320,16,1,TFT_BLACK);
  // Draw StatusBar
  tft.fillRoundRect(0, 224, 320, 240, 1, TFT_LIGHTGREY);
  tft.drawRoundRect(0,224,320,240,1,TFT_BLACK);
  
  setBrightness(100);
 
  //tft.setFreeFont(FF2); //select Free, Mono, Oblique, 12pt.
  tft.drawString("Mono 12pt",70,110);//prints string at (70,110)
 
  

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
  tft.setTextColor(TFT_BLACK,TFT_LIGHTGREY);
  tft.drawString("AZ-Touch-IP: ",10,5,1);
  tft.drawString(WiFi.localIP().toString(),100,5,1);
  // check file system exists
  if (!LittleFS.begin()) {
    Serial.println("Formatting file system");
    LittleFS.format();
    LittleFS.begin();
  }
  Serial.println("Informationen zum Dateisystem:");
  Serial.printf("- Bytes total:   %ld\n", LittleFS.totalBytes());
  Serial.printf("- Bytes genutzt: %ld\n\n", LittleFS.usedBytes());
  listDir(LittleFS, "/", 3);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  cronjob();
  
}