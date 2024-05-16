
#include "FastLED.h"
#include "ble_client.h"
#include "ble_com_proto.h"
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
#define BLE_LED_PIN 12
CRGB leds [NUM_LEDS];
uint8_t gBrightness = 128;


#define LED_DATA_PIN GPIO_NUM_14

bool startUpload=false;
bool startDownload=false;
bool stop=false;
int bleBlinkSpeed=500;
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
  setup_ble_client();
  pinMode(LED_DATA_PIN,OUTPUT);
  digitalWrite(LED_DATA_PIN,1);
 
}
void loopPin(){
    static int i=0;
    EXEC_EVERY(1000);
    digitalWrite(LED_DATA_PIN,i);
    i=i==0?1:0;
}
void clearHelmet(){
   fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED[0].showLeds(gBrightness);
}
void animUpload(){
    static int i=0;
    EXEC_EVERY(150);
    //ESP_LOGI("pwr", "Led %d",i);
    /* 
     FastLED[0].clearLedData();
    FastLED[0].clearLeds(NUM_LEDS);
    */
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    for (int index_strip=0;index_strip < NUM_STRIPS; index_strip++){
        leds[i+(index_strip*NUM_LEDS_PER_STRIP)]=LED_COLOR;
    }
    i++;
    if (i>=NUM_LEDS_PER_STRIP){
        i=0;
    }
    
    FastLED[0].showLeds(gBrightness);
}
void animDownload(){
   static int i=NUM_LEDS_PER_STRIP-1;
    EXEC_EVERY(150);
    //ESP_LOGI("pwr", "Led %d",i);
    /* 
     FastLED[0].clearLedData();
    FastLED[0].clearLeds(NUM_LEDS);
    */
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    for (int index_strip=0;index_strip < NUM_STRIPS; index_strip++){
        leds[i+(index_strip*NUM_LEDS_PER_STRIP)]=LED_COLOR;
    }
    i--;
    if (i<0){
        i=NUM_LEDS_PER_STRIP-1;
    }
    
    FastLED[0].showLeds(gBrightness);
}
void loopLeds(){
  if (startUpload){
    animUpload();
  } else if (startDownload){
    animDownload();
  } else if (stop){
    clearHelmet();
  } else {
    clearHelmet();
  }
}
void blinkBle(){
  static uint8_t bleLed=0;
  EXEC_EVERY(bleBlinkSpeed);
   digitalWrite(BLE_LED_PIN, bleLed);
   bleLed^=0x1;
    ESP_LOGI("main","ble led");
}
void loop() {
  if (!isConnected()){
    clearHelmet();
    blinkBle();
  }else {
     digitalWrite(BLE_LED_PIN,1);
  }
  loop_ble_client();
  loopLeds();
  //loopPin();
}