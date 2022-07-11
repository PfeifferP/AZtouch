#include <Arduino.h>
#include <WiFi.h>

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  if (WiFi.status() != WL_CONNECTED){
    return -1;
  } else {
    int32_t dbm = WiFi.RSSI();
    if(dbm <= -100) {
        return 0;
    } else if(dbm >= -50) {
        return 100;
    } else {
        return 2 * (dbm + 100);
    }
  }
}

void drawWifiQuality() {
  int8_t quality = getWifiQuality();
  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  tft.drawRightString("  " + String(quality) + "%",305, 5, 1);
  for (int8_t i = 0; i < 4; i++) {
    tft.drawFastVLine(310 + 2 * i,4,8, TFT_LIGHTGREY);
    for (int8_t j = 0; j < 2 * (i + 1); j++) {
      if (quality > i * 25 || j == 0) {
        tft.drawPixel(310 + 2 * i, 12 - j,TFT_BLACK);
      }
    }
  }
}

void setBrightness(uint32_t newBrightness) {
  ledcWrite(0, map(newBrightness, 0, 100, 255, 0)); // 0-15, 0-255 (with 8 bit resolution);  255=totally dark;0=totally shiny
}

void setupBrightnessControl() {
  //pinMode(5, OUTPUT);
  ledcSetup(0, 5000, 8); // 0-15, 5000, 8
  ledcAttachPin(5, 0); // TFT_BL, 0 - 15
}

void clearStatusBar(){
  tft.fillRoundRect(0, 224, 320, 240, 1, TFT_LIGHTGREY);
  tft.drawRoundRect(0,224,320,240,1,TFT_BLACK);
}

void setStatusBar(String text){
  clearStatusBar;
  tft.drawString(text,10,229,1);
  timerAlarmWrite(timer1,10000000,false);
  timerAlarmEnable(timer1);
}

void checkTouched(){
  if(tft.getTouch(&t_x, &t_y)){
    tft.fillCircle(t_x, t_y, 2, TFT_SKYBLUE);
  }
}