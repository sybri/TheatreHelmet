#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Arduino.h>
#include "ble_com_proto.h"

void setup_ble_server();
void loop_ble_server();
void Notifiate(THEATRE_PROTO flags);
void Notifiate(uint32_t flags);
void startUploadHelmetGreen();
void startUploadHelmetBlue();
void startUploadHelmetOrange();
void startDownloadHelmetGreen();
void startDownloadHelmetBlue();
void startDownloadHelmetOrange();
void stopBlue();
void stopGreen();
void stopOrange();