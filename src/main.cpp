#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ESPConnect.h>
#include <time.h>




TFT_eSPI tft=TFT_eSPI();
AsyncWebServer server(80);

struct tm tm;

String wochentage[7]={"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};


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

}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(1000);
}