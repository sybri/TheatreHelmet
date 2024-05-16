/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */
#include <Arduino.h>
#include "BLEDevice.h"
#include "ble_client.h"
#include "ble_com_proto.h"
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "ble_com_proto.h"
// #include "BLEScan.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID(SERVICE_UUID);
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID(CHARACTERISTIC_UUID);

static bool doConnect = false;
static bool connected = false;
static bool doScan = false;
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;
extern bool startUpload;
extern bool startDownload;
extern bool stop;
extern int bleBlinkSpeed;

static void notifyCallback(
    BLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
    uint32_t *value=(uint32_t*)pData;
    ESP_LOGI("ble_client", "Notify callback for characteristic %s  of data length  %d data: %x ", pBLERemoteCharacteristic->getUUID().toString().c_str(), length, *pData);
    if (*value & (uint32_t) THEATRE_PROTO::SERV_READY){
        #if HELMET_NAME == HELMET_ORANGE
            startDownload=*value & (uint32_t) THEATRE_PROTO::START_DOWNLOAD_HELMET_ORANGE;
            startUpload=*value & (uint32_t) THEATRE_PROTO::START_UPLOAD_HELMET_ORANGE;
            stop=*value & (uint32_t) THEATRE_PROTO::STOP_HELMET_ORANGE;
        #endif
         #if HELMET_NAME==HELMET_GREEN
            startDownload=*value & (uint32_t) THEATRE_PROTO::START_DOWNLOAD_HELMET_GREEN;
            startUpload=*value & (uint32_t) THEATRE_PROTO::START_UPLOAD_HELMET_GREEN;
            stop=*value & (uint32_t) THEATRE_PROTO::STOP_HELMET_GREEN;
        #endif
         #if HELMET_NAME==HELMET_BLUE
            startDownload=*value & (uint32_t) THEATRE_PROTO::START_DOWNLOAD_HELMET_BLUE;
            startUpload=*value & (uint32_t) THEATRE_PROTO::START_UPLOAD_HELMET_BLUE;
            stop=*value & (uint32_t) THEATRE_PROTO::STOP_HELMET_BLUE;
        #endif

    } else {
        startDownload=false;
        startUpload=false;
        stop=true;
    }
}

class MyClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pclient)
    {
    }

    void onDisconnect(BLEClient *pclient)
    {
        connected = false;
        ESP_LOGI("ble_client", "onDisconnect");
    }
};
bool isConnected(){
    return connected;
}
bool connectToServer()
{
    bleBlinkSpeed=500;

    ESP_LOGI("ble_client", "Forming a connection to %s  - Created client", myDevice->getAddress().toString().c_str());

    BLEClient *pClient = BLEDevice::createClient();

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice); // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    ESP_LOGI("ble_client", " - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr)
    {
        ESP_LOGI("ble_client", "Failed to find our service UUID: %s", serviceUUID.toString().c_str());

        pClient->disconnect();
        return false;
    }
    ESP_LOGI("ble_client", " - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr)
    {
        ESP_LOGI("ble_client", "Failed to find our characteristic UUID: %s", charUUID.toString().c_str());

        pClient->disconnect();
        return false;
    }
    ESP_LOGI("ble_client", " - Found our characteristic");

    // Read the value of the characteristic.
    if (pRemoteCharacteristic->canRead())
    {
        uint32_t value = pRemoteCharacteristic->readUInt32();
        ESP_LOGI("ble_client", "The characteristic value was: %x", value);
    }

    if (pRemoteCharacteristic->canNotify())
        pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    ESP_LOGI("ble_client", "connected done");
    return connected;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    /**
     * Called for each advertising BLE server.
     */
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        ESP_LOGI("ble_client", "BLE Advertised Device found: %s %s", advertisedDevice.getName().c_str(),advertisedDevice.getAddress().toString().c_str());
        for(int i =0 ; i < advertisedDevice.getServiceUUIDCount();i++){

         ESP_LOGI("ble_client", "   - Service UUID %d => %s",i,advertisedDevice.getServiceUUID(i).toString().c_str());
        }
        // We have found a device, let us now see if it contains the service we are looking for.
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID))
        {
            ESP_LOGI("ble_client", "Ye!!!!!!!!!!!!!");
            BLEDevice::getScan()->stop();
            bleBlinkSpeed=1000;
            myDevice = new BLEAdvertisedDevice(advertisedDevice);
            doConnect = true;
            doScan = true;

        } // Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks

void setup_ble_client()
{

    ESP_LOGI("ble_client", "Starting Arduino BLE Client application...");
    BLEDevice::init("");

    // Retrieve a Scanner and set the callback we want to use to be informed when we
    // have detected a new device.  Specify that we want active scanning and start the
    // scan to run for 5 seconds.
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    bleBlinkSpeed=250;
    pBLEScan->start(0, true);
    
} // End of setup.

// This is the Arduino main loop function.
void loop_ble_client()
{
    EXEC_EVERY(1000);
    // If the flag "doConnect" is true then we have scanned for and found the desired
    // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
    // connected we set the connected flag to be true.
    if (doConnect == true)
    {
        if (connectToServer())
        {
            ESP_LOGI("ble_client", "We are now connected to the BLE Server.");
        }
        else
        {
            ESP_LOGI("ble_client", "We have failed to connect to the server; there is nothin more we will do.");
        }
        doConnect = false;
    }

    // If we are connected to a peer BLE Server, update the characteristic each time we are reached
    // with the current time since boot.
    if (connected)
    {
        //String newValue = "Time since boot: " + String(millis() / 1000);
        //ESP_LOGI("ble_client", "Setting new characteristic value to \"%s\"", newValue.c_str());

        // Set the characteristic's value to be the array of bytes that is actually a string.
        //pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
    }
    else if (doScan)
    {
        bleBlinkSpeed=250;
        BLEDevice::getScan()->start(0,true); // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
    }
    ESP_LOGI("ble_client",".");

    
} // End of loop