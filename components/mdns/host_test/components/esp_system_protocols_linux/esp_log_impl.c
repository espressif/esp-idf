// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_err.h"
#include "esp_log.h"
#include <stdlib.h>

void _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    ESP_LOGE("ESP_ERROR_CHECK", "Failed with esp_err_t: 0x%x", rc);
    ESP_LOGE("ESP_ERROR_CHECK", "Expression: %s", expression);
    ESP_LOGE("ESP_ERROR_CHECK", "Functions: %s %s(%d)", function, file, line);
    abort();
}

void esp_log_buffer_hexdump_internal(const char *tag, const void *buffer, uint16_t buff_len, esp_log_level_t log_level)
{
    if ( LOG_LOCAL_LEVEL >= log_level ) {
        ESP_LOG_LEVEL(log_level, tag, "Buffer:%p length:%d", buffer, buff_len);
        for (int i=0; i<buff_len; ++i) {
            printf("%02x ", ((uint8_t*)buffer)[i]);
        }
        printf("\n");
    }
}
