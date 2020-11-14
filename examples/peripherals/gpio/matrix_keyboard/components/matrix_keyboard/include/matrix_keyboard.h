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

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAKE_KEY_CODE(row, col) ((row << 8) | (col))
#define GET_KEY_CODE_ROW(code)  ((code >> 8) & 0xFF)
#define GET_KEY_CODE_COL(code)  (code & 0xFF)

/**
 * @brief Type defined for matrix keyboard handle
 *
 */
typedef struct matrix_kbd_t *matrix_kbd_handle_t;

/**
 * @brief Matrix keyboard event ID
 *
 */
typedef enum {
    MATRIX_KBD_EVENT_DOWN, /*!< Key is pressed down */
    MATRIX_KBD_EVENT_UP    /*!< Key is released */
} matrix_kbd_event_id_t;

/**
 * @brief Type defined for matrix keyboard event handler
 *
 * @note The event handler runs in a OS timer context
 *
 * @param[in] mkbd_handle Handle of matrix keyboard that return from `matrix_kbd_install`
 * @param[in] event Event ID, refer to `matrix_kbd_event_id_t` to see all supported events
 * @param[in] event_data Data for corresponding event
 * @param[in] handler_args Arguments that user passed in from `matrix_kbd_register_event_handler`
 * @return Currently always return ESP_OK
 */
typedef esp_err_t (*matrix_kbd_event_handler)(matrix_kbd_handle_t mkbd_handle, matrix_kbd_event_id_t event, void *event_data, void *handler_args);

/**
 * @brief Configuration structure defined for matrix keyboard
 *
 */
typedef struct {
    const int *row_gpios;  /*!< Array, contains GPIO numbers used by row line */
    const int *col_gpios;  /*!< Array, contains GPIO numbers used by column line */
    uint32_t nr_row_gpios; /*!< row_gpios array size */
    uint32_t nr_col_gpios; /*!< col_gpios array size */
    uint32_t debounce_ms;  /*!< Debounce time */
} matrix_kbd_config_t;

/**
 * @brief Default configuration for matrix keyboard driver
 *
 */
#define MATRIX_KEYBOARD_DEFAULT_CONFIG() \
{                                        \
    .row_gpios = NULL,                   \
    .col_gpios = NULL,                   \
    .nr_row_gpios = 0,                   \
    .nr_col_gpios = 0,                   \
    .debounce_ms = 20,                   \
}

/**
 * @brief Install matrix keyboard driver
 *
 * @param[in] config Configuration of matrix keyboard driver
 * @param[out] mkbd_handle Returned matrix keyboard handle if installation succeed
 * @return
 *      - ESP_OK: Install matrix keyboard driver successfully
 *      - ESP_ERR_INVALID_ARG: Install matrix keyboard driver failed because of some invalid argument
 *      - ESP_ERR_NO_MEM: Install matrix keyboard driver failed because there's no enough capable memory
 *      - ESP_FAIL: Install matrix keyboard driver failed because of other error
 */
esp_err_t matrix_kbd_install(const matrix_kbd_config_t *config, matrix_kbd_handle_t *mkbd_handle);

/**
 * @brief Uninstall matrix keyboard driver
 *
 * @param[in] mkbd_handle Handle of matrix keyboard that return from `matrix_kbd_install`
 * @return
 *      - ESP_OK: Uninstall matrix keyboard driver successfully
 *      - ESP_ERR_INVALID_ARG: Uninstall matrix keyboard driver failed because of some invalid argument
 *      - ESP_FAIL: Uninstall matrix keyboard driver failed because of other error
 */
esp_err_t matrix_kbd_uninstall(matrix_kbd_handle_t mkbd_handle);

/**
 * @brief Start matrix keyboard driver
 *
 * @param[in] mkbd_handle Handle of matrix keyboard that return from `matrix_kbd_install`
 * @return
 *      - ESP_OK: Start matrix keyboard driver successfully
 *      - ESP_ERR_INVALID_ARG: Start matrix keyboard driver failed because of some invalid argument
 *      - ESP_FAIL: Start matrix keyboard driver failed because of other error
 */
esp_err_t matrix_kbd_start(matrix_kbd_handle_t mkbd_handle);

/**
 * @brief Stop matrix kayboard driver
 *
 * @param[in] mkbd_handle Handle of matrix keyboard that return from `matrix_kbd_install`
 * @return
 *      - ESP_OK: Stop matrix keyboard driver successfully
 *      - ESP_ERR_INVALID_ARG: Stop matrix keyboard driver failed because of some invalid argument
 *      - ESP_FAIL: Stop matrix keyboard driver failed because of other error
 */
esp_err_t matrix_kbd_stop(matrix_kbd_handle_t mkbd_handle);

/**
 * @brief Register matrix keyboard event handler
 *
 * @param[in] mkbd_handle Handle of matrix keyboard that return from `matrix_kbd_install`
 * @param[in] handler Event handler
 * @param[in] args Arguments that will be passed to the handler
 * @return
 *      - ESP_OK: Register event handler successfully
 *      - ESP_ERR_INVALID_ARG: Register event handler failed because of some invalid argument
 *      - ESP_FAIL: Register event handler failed because of other error
 */
esp_err_t matrix_kbd_register_event_handler(matrix_kbd_handle_t mkbd_handle, matrix_kbd_event_handler handler, void *args);

#ifdef __cplusplus
}
#endif
