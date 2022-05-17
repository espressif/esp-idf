// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef ESP_APP_TRACE_PORT_H_
#define ESP_APP_TRACE_PORT_H_

#include "esp_app_trace_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Apptrace HW interface. */
typedef struct {
    esp_err_t (*init)(void *hw_data);
    uint8_t *(*get_up_buffer)(void *hw_data, uint32_t, esp_apptrace_tmo_t *);
    esp_err_t (*put_up_buffer)(void *hw_data, uint8_t *, esp_apptrace_tmo_t *);
    esp_err_t (*flush_up_buffer_nolock)(void *hw_data, uint32_t, esp_apptrace_tmo_t *);
    esp_err_t (*flush_up_buffer)(void *hw_data, esp_apptrace_tmo_t *);
    void (*down_buffer_config)(void *hw_data, uint8_t *buf, uint32_t size);
    uint8_t *(*get_down_buffer)(void *hw_data, uint32_t *, esp_apptrace_tmo_t *);
    esp_err_t (*put_down_buffer)(void *hw_data, uint8_t *, esp_apptrace_tmo_t *);
    bool (*host_is_connected)(void *hw_data);
} esp_apptrace_hw_t;

esp_apptrace_hw_t *esp_apptrace_jtag_hw_get(void **data);
esp_apptrace_hw_t *esp_apptrace_uart_hw_get(int num, void **data);

#ifdef __cplusplus
}
#endif

#endif
