/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
