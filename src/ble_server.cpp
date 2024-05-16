#include "ble_server.h"
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "ble_com_proto.h"
BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;
uint32_t data = 0x0001;



struct helmetState helmetOrangeState = { .lastStated=0,.isStopped=false,.isUploaded=false,.isDownloaded=false};
struct helmetState helmetGreenState = { .lastStated=0,.isStopped=false,.isUploaded=false,.isDownloaded=false};
struct helmetState helmetBlueState = { .lastStated=0,.isStopped=false,.isUploaded=false,.isDownloaded=false};


bool deviceConnected = false;
bool oldDeviceConnected = false;
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup_ble_server()
{
    BLEDevice::init(BLE_SRV_NAME); // set the device name
    pServer = BLEDevice::createServer();
     pServer->setCallbacks(new MyServerCallbacks());
    pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE
    );
     pCharacteristic->addDescriptor(new BLE2902());

    pCharacteristic->setValue(data);
    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x00); // functions that help with iPhone connections issue
    //pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    ESP_LOGI("ble_server", "Characteristic defined! Now you can read it in your phone!");
}
void loop_ble_server() {
    // notify changed value
    if (deviceConnected) {
       // pCharacteristic->setValue(data);
        //pCharacteristic->notify();
       
        delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        ESP_LOGI("ble_server", "start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
         pCharacteristic->setValue(data);
        pCharacteristic->notify();
    }
    if (!helmetGreenState.isStopped && ( millis()-  helmetGreenState.lastStated) > ANIMATION_AUTO_STOP){
        stopGreen();
    }
     if (!helmetOrangeState.isStopped && ( millis()-  helmetOrangeState.lastStated) > ANIMATION_AUTO_STOP){
        stopOrange();
    }
     if (!helmetBlueState.isStopped && ( millis()-  helmetBlueState.lastStated) > ANIMATION_AUTO_STOP){
        stopBlue();
    }
}
void Notifiate(THEATRE_PROTO flags)
{
    Notifiate((uint32_t)flags);
}
void Notifiate(uint32_t flags)
{
    data = flags | (uint32_t)THEATRE_PROTO::SERV_READY;

    pCharacteristic->setValue(data);
    pCharacteristic->notify();
}
void startUploadHelmetGreen()
{
    helmetGreenState.isUploaded=true;
    helmetGreenState.isStopped=false;
    helmetGreenState.isDownloaded=false;
    helmetGreenState.lastStated=millis();
    Notifiate(THEATRE_PROTO::START_UPLOAD_HELMET_GREEN);
}
void startUploadHelmetBlue()
{
    helmetBlueState.isUploaded=true;
    helmetBlueState.isStopped=false;
    helmetBlueState.isDownloaded=false;
    helmetBlueState.lastStated=millis();
    Notifiate(THEATRE_PROTO::START_UPLOAD_HELMET_BLUE);
}
void startUploadHelmetOrange()
{
    helmetOrangeState.isUploaded=true;
    helmetOrangeState.isStopped=false;
    helmetOrangeState.isDownloaded=false;
    helmetOrangeState.lastStated=millis();
    Notifiate(THEATRE_PROTO::START_UPLOAD_HELMET_ORANGE);
}

void startDownloadHelmetGreen()
{
    
    helmetGreenState.isUploaded=false;
    helmetGreenState.isStopped=false;
    helmetGreenState.isDownloaded=true;
    helmetGreenState.lastStated=millis();
    Notifiate(THEATRE_PROTO::START_DOWNLOAD_HELMET_GREEN);
}
void startDownloadHelmetBlue()
{
    helmetBlueState.isUploaded=false;
    helmetBlueState.isStopped=false;
    helmetBlueState.isDownloaded=true;
    helmetBlueState.lastStated=millis();
    Notifiate(THEATRE_PROTO::START_DOWNLOAD_HELMET_BLUE);
}
void startDownloadHelmetOrange()
{
    helmetOrangeState.isUploaded=false;
    helmetOrangeState.isStopped=false;
    helmetOrangeState.isDownloaded=true;
    helmetOrangeState.lastStated=millis();
    Notifiate(THEATRE_PROTO::START_DOWNLOAD_HELMET_ORANGE);
}
void stopBlue()
{
    helmetBlueState.isUploaded=false;
    helmetBlueState.isStopped=true;
    helmetBlueState.isDownloaded=false;
    Notifiate(THEATRE_PROTO::STOP_HELMET_BLUE);
}

void stopGreen()
{
    helmetGreenState.isUploaded=false;
    helmetGreenState.isStopped=true;
    helmetGreenState.isDownloaded=false;
    Notifiate(THEATRE_PROTO::STOP_HELMET_GREEN);
}
void stopOrange()
{
    helmetOrangeState.isUploaded=false;
    helmetOrangeState.isStopped=true;
    helmetOrangeState.isDownloaded=false;
    Notifiate(THEATRE_PROTO::STOP_HELMET_ORANGE);
}