// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdio.h>
#include <stdarg.h>
#include <sdkconfig.h>
#include "SEGGER_SYSVIEW_Int.h"
#include "freertos/FreeRTOS.h"

static portMUX_TYPE s_log_mutex = portMUX_INITIALIZER_UNLOCKED;

int esp_sysview_vprintf(const char * format, va_list args)
{
    static char log_buffer[SEGGER_SYSVIEW_MAX_STRING_LEN];

    portENTER_CRITICAL(&s_log_mutex);
    size_t len = vsnprintf(log_buffer, sizeof(log_buffer), format, args);
    if (len > sizeof(log_buffer) - 1) {
        log_buffer[sizeof(log_buffer - 1)] = 0;
    }
    SEGGER_SYSVIEW_Print(log_buffer);
    portEXIT_CRITICAL(&s_log_mutex);
    return len;
}
