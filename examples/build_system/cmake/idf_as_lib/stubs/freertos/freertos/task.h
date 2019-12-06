#pragma once

#include <stdint.h>

#define portTICK_PERIOD_MS  1000

void vTaskDelay( const uint32_t xTicksToDelay );