#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ESPConnect.h>
#include <time.h>


/* https://fipsok.de/Esp32-Webserver/ntp-zeit-Esp32.tab */

TFT_eSPI tft=TFT_eSPI();
AsyncWebServer server(80);


String wochentage[7]={"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

void initTime(){
  struct tm timeinfo;

  Serial.println("Setting up time");
  configTime(0, 0, "pool.ntp.org");    // First connect to NTP server, with 0 TZ offset
  if(!getLocalTime(&timeinfo)){
    Serial.println("  Failed to obtain time");
    return;
  }
  Serial.println("  Got the time from NTP");
  // Now we can set the real timezone
  setenv("TZ","CET-1CEST,M3.5.0,M10.5.0/3",1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}
void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time 1");
    return;
  }
  Serial.println(&timeinfo, "%w, %B %d %Y %H:%M:%S zone %Z %z ");
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
  //initialisieren des NTPClients um die Zeit auszulesen
  initTime();

  printLocalTime();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(1000);
}