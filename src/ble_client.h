#pragma once
#ifndef EXEC_EVERY

#define EXEC_EVERY(time)      \
  static int32_t lastRun = 0; \
  int32_t now = millis();     \
  if (now < lastRun + time)   \
    return;                   \
  lastRun = millis();
#endif

bool connectToServer();
void setup_ble_client();
void loop_ble_client();
bool isConnected();