#pragma once
#include <Arduino.h>

void fetch_crypto_data(QueueHandle_t dataQueue);
extern SemaphoreHandle_t fetch_sem;
