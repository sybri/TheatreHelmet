
#include <M5Tough.h>
#include "ble_server.h"

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


ButtonColors on_clrs = {BLUE, WHITE, WHITE};
ButtonColors off_clrs = {BLACK, WHITE, WHITE};
ButtonColors orangeOn = {BLACK, ORANGE, WHITE};
ButtonColors orangeOff = {ORANGE, BLACK, WHITE};
ButtonColors blueOn = {BLACK, BLUE, WHITE};
ButtonColors blueOff = {BLUE, BLACK, WHITE};
ButtonColors greenOn = {BLACK, GREEN, WHITE};
ButtonColors greenOff = {GREEN, BLACK, WHITE};
Button btnDownloadOrange  (20, 0    , 80, 80, false, "Rec", orangeOff, orangeOn, MC_DATUM);
Button btnUploadOrange    (20, 160  , 80, 80, false, "Env", orangeOff, orangeOn, MC_DATUM);
Button btnDownloadGreen   (120, 0   , 80, 80, false, "Rec", greenOff, greenOn, MC_DATUM);
Button btnUploadGreen     (120, 160 , 80, 80, false, "Env", greenOff, greenOn, MC_DATUM);
Button btnDownloadBlue    (220, 0   , 80, 80, false, "Rec", blueOff, blueOn, MC_DATUM);
Button btnUploadBlue      (220, 160 , 80, 80, false, "Env", blueOff, blueOn, MC_DATUM);
Button btnStopOrange      (20 , 100 , 80, 40, false, "Stop", off_clrs, on_clrs, MC_DATUM);
Button btnStopBlue        (120 , 100 , 80, 40, false, "Stop", off_clrs, on_clrs, MC_DATUM);
Button btnStopGreen       (220 , 100 , 80, 40, false, "Stop", off_clrs, on_clrs, MC_DATUM);

void eventDisplay(Event& e) {
  //M5.Lcd.fillRect(0, 0, 320, 150, BLACK);
  Serial.printf("%-12s finger%d  %-18s (%3d, %3d) --> (%3d, %3d)   ",
                e.typeName(), e.finger, e.objName(), e.from.x, e.from.y,
                e.to.x, e.to.y);

  Serial.printf("( dir %d deg, dist %d, %d ms )\r\n", e.direction(),
                e.distance(), e.duration);

  char info[50];
    
  if (e.type==E_RELEASE){
    if (e.button == &btnUploadGreen){
      startUploadHelmetGreen();
    }
    if (e.button == &btnDownloadGreen){
      startDownloadHelmetGreen();
    }
    if (e.button == &btnUploadBlue){
      startUploadHelmetBlue();
    }
    if (e.button == &btnDownloadBlue){
      startDownloadHelmetBlue();
    }
    if (e.button == &btnUploadOrange){
      startUploadHelmetOrange();
    }
    if (e.button == &btnDownloadOrange){
      startDownloadHelmetOrange();
    }
    if (e.button == &btnStopOrange){
      stopOrange();
    }
    if (e.button == &btnStopBlue){
      stopBlue();
    }
    if (e.button == &btnStopGreen){
      stopGreen();
    }
  }
   
  //print the button name
  sprintf(info, "Btn %s wasPressed", e.objName());
  //M5.Lcd.drawString(info, 160, 100);
}
#define LED_DATA_PIN GPIO_NUM_19


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
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Buttons.draw();
  //add handler for the buttons
  btnUploadOrange.addHandler(eventDisplay, E_ALL);
  btnUploadGreen.addHandler(eventDisplay, E_ALL);
  btnUploadBlue.addHandler(eventDisplay, E_ALL);
  btnDownloadOrange.addHandler(eventDisplay, E_ALL);
  btnDownloadGreen.addHandler(eventDisplay, E_ALL);
  btnDownloadBlue.addHandler(eventDisplay, E_ALL);
  btnStopOrange.addHandler(eventDisplay, E_ALL);
  btnStopGreen.addHandler(eventDisplay, E_ALL);
  btnStopBlue.addHandler(eventDisplay, E_ALL);
  
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

void loop() {
 
  M5.update();
  loopPower();
 
  loopLeds();
  loop_ble_server();
  //loopPin();
}