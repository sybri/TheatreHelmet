#pragma once
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define BLE_SRV_NAME "TheatreHelmetServer"
#define ANIMATION_AUTO_STOP 15000
struct helmetState{
    uint32_t lastStated;
    bool isStopped;
    bool isUploaded;
    bool isDownloaded;
};

enum class THEATRE_PROTO: uint32_t{
    SERV_READY= 0x00000001,
    START_UPLOAD_HELMET_ORANGE= 0x00000002,
    START_UPLOAD_HELMET_GREEN= 0x00000004,
    START_UPLOAD_HELMET_BLUE= 0x00000008,
    START_DOWNLOAD_HELMET_ORANGE= 0x00000010,
    START_DOWNLOAD_HELMET_GREEN= 0x00000020,
    START_DOWNLOAD_HELMET_BLUE= 0x00000040,
    STOP_HELMET_ORANGE= 0x00000080,
    STOP_HELMET_GREEN= 0x00000100,
    STOP_HELMET_BLUE= 0x00000200,
};
#define HELMET_ORANGE 1
#define HELMET_BLUE 2
#define HELMET_GREEN 3

#ifndef HELMET_NAME
    #ifndef HELMET_MODE_MASTER
        #error "Undefined value of HELMET_NAME."
    #endif
#elif HELMET_NAME == HELMET_ORANGE
    #pragma message  ("Helmet Orange" HELMET_NAME)
    #define LED_COLOR CRGB::OrangeRed
#elif HELMET_NAME == HELMET_GREEN
    #pragma message ("Helmet Green")
    #define LED_COLOR CRGB::Green
#elif HELMET_NAME == HELMET_BLUE
    #pragma message ("Helmet Blue")
    #define LED_COLOR CRGB::Blue
#else
    #error "Unexpected value of HELMET_NAME."
#endif