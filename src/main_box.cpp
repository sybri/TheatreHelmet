
#include <M5Tough.h>
#include "ble_server.h"
//#include "Fonts/GFXFF/FreeSans24pt7b.h"
#include "FastLED.h"
#define EXEC_EVERY(time)      \
  static int32_t lastRun = 0; \
  int32_t now = millis();     \
  if (now < lastRun + time)   \
    return;                   \
  lastRun = millis();

// This program tested for M5Buttons

#define NUM_LEDS_PER_STRIP 5
#define NUM_STRIPS 5
#define NUM_LEDS (NUM_LEDS_PER_STRIP * NUM_STRIPS)
#define COLOR_ORDER GRB
CRGB leds [NUM_LEDS];
uint8_t gBrightness = 128;
#define NUM_COL  4
#define MARGIN_X 2
#define COL_W_OUT  (320/NUM_COL)
#define COL_W_IN  (COL_W_OUT - (2*MARGIN_X))
#define COLX(n) (n*COL_W_OUT)+MARGIN_X


#define NUM_ROW  3
#define MARGIN_Y 2
#define ROW_H_OUT  (240/NUM_ROW)
#define ROW_H_IN  (ROW_H_OUT - (2*MARGIN_Y))
#define ROWY(n) (n*ROW_H_OUT)+MARGIN_Y


ButtonColors on_clrs = {BLUE, WHITE, WHITE};
ButtonColors off_clrs = {BLACK, WHITE, WHITE};
ButtonColors orangeOn = {BLACK, ORANGE, WHITE};
ButtonColors orangeOff = {ORANGE, BLACK, WHITE};
ButtonColors blueOn = {BLACK, BLUE, WHITE};
ButtonColors blueOff = {BLUE, BLACK, WHITE};
ButtonColors greenOn = {BLACK, GREEN, WHITE};
ButtonColors greenOff = {GREEN, BLACK, WHITE};

Button btnDownloadOrange  (COLX(0), ROWY(0) , COL_W_IN, ROW_H_IN, false, "R", orangeOff, orangeOn, MC_DATUM);
Button btnStopOrange      (COLX(0), ROWY(1) , COL_W_IN, ROW_H_IN, false, "X", orangeOff, orangeOn, MC_DATUM);
Button btnUploadOrange    (COLX(0), ROWY(2) , COL_W_IN, ROW_H_IN, false, "E", orangeOff, orangeOn, MC_DATUM);

Button btnDownloadGreen   (COLX(1), ROWY(0) , COL_W_IN, ROW_H_IN, false, "R", greenOff, greenOn, MC_DATUM);
Button btnStopGreen       (COLX(1), ROWY(1) , COL_W_IN, ROW_H_IN, false, "X", greenOff, greenOn, MC_DATUM);
Button btnUploadGreen     (COLX(1), ROWY(2) , COL_W_IN, ROW_H_IN, false, "E", greenOff, greenOn, MC_DATUM);

Button btnDownloadBlue    (COLX(2), ROWY(0) , COL_W_IN, ROW_H_IN, false, "R", blueOff, blueOn, MC_DATUM);
Button btnStopBlue        (COLX(2), ROWY(1) , COL_W_IN, ROW_H_IN, false, "X", blueOff, blueOn, MC_DATUM);
Button btnUploadBlue      (COLX(2), ROWY(2) , COL_W_IN, ROW_H_IN, false, "E", blueOff, blueOn, MC_DATUM);

Button btnDownloadAll    (COLX(3), ROWY(0) , COL_W_IN, ROW_H_IN, false, "R", off_clrs, on_clrs, MC_DATUM);
Button btnStopAll        (COLX(3), ROWY(1) , COL_W_IN, ROW_H_IN, false, "X", off_clrs, on_clrs, MC_DATUM);
Button btnUploadAll      (COLX(3), ROWY(2) , COL_W_IN, ROW_H_IN, false, "E", off_clrs, on_clrs, MC_DATUM);

void eventDisplay(Event& e) {
  //M5.Lcd.fillRect(0, 0, 320, 150, BLACK);
  Serial.printf("%-12s finger%d  %-18s (%3d, %3d) --> (%3d, %3d)   ",
                e.typeName(), e.finger, e.objName(), e.from.x, e.from.y,
                e.to.x, e.to.y);

  Serial.printf("( dir %d deg, dist %d, %d ms )\r\n", e.direction(),
                e.distance(), e.duration);

  char info[50];
    
  if (e.type==E_RELEASE){
     if (e.button == &btnUploadAll){
      startUploadHelmetGreen();
      startUploadHelmetBlue();
      startUploadHelmetOrange();
      Notifiate();
    }
    if (e.button == &btnUploadGreen){
      startUploadHelmetGreen();
       Notifiate();
    }
    if (e.button == &btnDownloadGreen){
      startDownloadHelmetGreen();
       Notifiate();
    }
    if (e.button == &btnUploadBlue){
      startUploadHelmetBlue();
       Notifiate();
    }
     if (e.button == &btnDownloadAll){
      startDownloadHelmetBlue();
      startDownloadHelmetOrange();
      startDownloadHelmetGreen();
       Notifiate();
    }
    if (e.button == &btnDownloadBlue){
      startDownloadHelmetBlue();
       Notifiate();
    }
    if (e.button == &btnUploadOrange){
      startUploadHelmetOrange();
       Notifiate();
    }
    if (e.button == &btnDownloadOrange){
      startDownloadHelmetOrange();
       Notifiate();
    }
    if (e.button == &btnStopAll){
      stopOrange();
       stopBlue();
       stopGreen();
       Notifiate();
    }
    if (e.button == &btnStopOrange){
      stopOrange();
       Notifiate();
    }
    if (e.button == &btnStopBlue){
      stopBlue();
       Notifiate();
    }
    if (e.button == &btnStopGreen){
      stopGreen();
       Notifiate();
    }
  }
   
  //print the button name
  sprintf(info, "Btn %s wasPressed", e.objName());
  //M5.Lcd.drawString(info, 160, 100);
}
#define LED_DATA_PIN GPIO_NUM_19


void drawArrow(int x, int y, int asize, int aangle, int pwidth, int plength, uint16_t color){
  float dx = (asize-10)*cos(aangle-90)*PI/180+x; // calculate X position  
  float dy = (asize-10)*sin(aangle-90)*PI/180+y; // calculate Y position  
  float x1 = 0;         float y1 = plength;
  float x2 = pwidth/2;  float y2 = pwidth/2;
  float x3 = -pwidth/2; float y3 = pwidth/2;
  float angle = aangle*PI/180-135;
  float xx1 = x1*cos(angle)-y1*sin(angle)+dx;
  float yy1 = y1*cos(angle)+x1*sin(angle)+dy;
  float xx2 = x2*cos(angle)-y2*sin(angle)+dx;
  float yy2 = y2*cos(angle)+x2*sin(angle)+dy;
  float xx3 = x3*cos(angle)-y3*sin(angle)+dx;
  float yy3 = y3*cos(angle)+x3*sin(angle)+dy;
  M5.Lcd.fillTriangle(xx1,yy1,xx3,yy3,xx2,yy2, color);
  M5.Lcd.drawLine(x, y, xx1, yy1, color);
  M5.Lcd.drawLine(x+1, y, xx1+1, yy1, color);
  M5.Lcd.drawLine(x, y+1, xx1, yy1+1, color);
  M5.Lcd.drawLine(x-1, y, xx1-1, yy1, color);
  M5.Lcd.drawLine(x, y-1, xx1, yy1-1, color);
  M5.Lcd.drawLine(x+2, y, xx1+2, yy1, color);
  M5.Lcd.drawLine(x, y+2, xx1, yy1+2, color);
  M5.Lcd.drawLine(x-2, y, xx1-2, yy1, color);
  M5.Lcd.drawLine(x, y-2, xx1, yy1-2, color);
}

void setupLeds(){
    FastLED.addLeds<WS2812,LED_DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS,0);
   
}
void setupLog(){
  esp_log_level_set("*", esp_log_level_t::ESP_LOG_ERROR);    // set all components to ERROR level
  esp_log_level_set("wifi", esp_log_level_t::ESP_LOG_WARN);  // enable WARN logs from WiFi stack
  esp_log_level_set("dhcpc", esp_log_level_t::ESP_LOG_INFO); // enable INFO logs from DHCP client
   Serial.begin(115200);
  ESP_LOGI("main","Hello, ESP32!");
}
void setup() {
  setupLog();
  setupLeds();
  setup_ble_server();
  pinMode(LED_DATA_PIN,OUTPUT);
  digitalWrite(LED_DATA_PIN,1);
 
  M5.begin(true,false,false,true,kMBusModeOutput);
  M5.Lcd.setFreeFont(&Orbitron_Light_32 ); 
  M5.Buttons.setFreeFont(&Orbitron_Light_32 );
  M5.Lcd.setTextDatum(MC_DATUM);

  //add handler for the buttons
  btnUploadOrange.addHandler(eventDisplay, E_ALL);
  btnUploadGreen.addHandler(eventDisplay, E_ALL);
  btnUploadBlue.addHandler(eventDisplay, E_ALL);
  btnUploadAll.addHandler(eventDisplay, E_ALL);
  btnDownloadOrange.addHandler(eventDisplay, E_ALL);
  btnDownloadGreen.addHandler(eventDisplay, E_ALL);
  btnDownloadBlue.addHandler(eventDisplay, E_ALL);
  btnDownloadAll.addHandler(eventDisplay, E_ALL);
  btnStopOrange.addHandler(eventDisplay, E_ALL);
  btnStopGreen.addHandler(eventDisplay, E_ALL);
  btnStopBlue.addHandler(eventDisplay, E_ALL);
  btnStopAll.addHandler(eventDisplay, E_ALL);

    M5.Buttons.draw();
  //M5.Buttons.setTextSize(5);
  
}
void loopPin(){
    static int i=0;
    EXEC_EVERY(1000);
    digitalWrite(LED_DATA_PIN,i);
    i=i==0?1:0;
}

void animUpload(){
    static int i=0;
    EXEC_EVERY(50);
    ESP_LOGI("pwr", "Led %d",i);
    /* 
     FastLED[0].clearLedData();
    FastLED[0].clearLeds(NUM_LEDS);
    */
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    for (int index_strip=0;index_strip < NUM_STRIPS; index_strip++){
        leds[i+(index_strip*NUM_LEDS_PER_STRIP)]=CRGB::OrangeRed;
         FastLED[0].showLeds(gBrightness);
    }
    

    i++;
    if (i>=NUM_LEDS_PER_STRIP){
        i=0;
    }
    
    FastLED[0].showLeds(gBrightness);
}
void animDownload(){

}
void loopLeds(){
    //animUpload();
}

void loopPower(){
    EXEC_EVERY(2500);
    ESP_LOGI("pwr", "vbus volt: %0.02fV",M5.Axp.GetVBusVoltage());
}

void loop() 
{
 
  M5.update();
  loopPower();
 
  loopLeds();
  loop_ble_server();
  //loopPin();
}