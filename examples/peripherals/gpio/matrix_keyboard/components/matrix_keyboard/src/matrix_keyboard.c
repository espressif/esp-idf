/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_compiler.h"
#include "esp_log.h"
#include "driver/dedic_gpio.h"
#include "driver/gpio.h"
#include "matrix_keyboard.h"
#include "esp_rom_sys.h"

static const char *TAG = "mkbd";

#define MKBD_CHECK(a, msg, tag, ret, ...)                                         \
    do {                                                                          \
        if (unlikely(!(a))) {                                                     \
            ESP_LOGE(TAG, "%s(%d): " msg, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret_code = ret;                                                       \
            goto tag;                                                             \
        }                                                                         \
    } while (0)

typedef struct matrix_kbd_t matrix_kbd_t;

struct matrix_kbd_t {
    dedic_gpio_bundle_handle_t row_bundle;
    dedic_gpio_bundle_handle_t col_bundle;
    uint32_t nr_row_gpios;
    uint32_t nr_col_gpios;
    TimerHandle_t debounce_timer;
    matrix_kbd_event_handler event_handler;
    void *event_handler_args;
    uint32_t row_state[0];
};

static IRAM_ATTR bool matrix_kbd_row_isr_callback(dedic_gpio_bundle_handle_t row_bundle, uint32_t row_index, void *args)
{
    BaseType_t high_task_wakeup = pdFALSE;
    matrix_kbd_t *mkbd = (matrix_kbd_t *)args;

    // temporarily disable interrupt
    dedic_gpio_bundle_set_interrupt_and_callback(row_bundle, (1 << mkbd->nr_row_gpios) - 1, DEDIC_GPIO_INTR_NONE, NULL, NULL);
    // get row id, start to check the col id
    dedic_gpio_bundle_write(row_bundle, 1 << row_index, 0);
    dedic_gpio_bundle_write(mkbd->col_bundle, (1 << mkbd->nr_col_gpios) - 1, (1 << mkbd->nr_col_gpios) - 1);
    xTimerStartFromISR(mkbd->debounce_timer, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static void matrix_kbd_debounce_timer_callback(TimerHandle_t xTimer)
{
    matrix_kbd_t *mkbd = (matrix_kbd_t *)pvTimerGetTimerID(xTimer);

    uint32_t row_out = dedic_gpio_bundle_read_out(mkbd->row_bundle);
    uint32_t col_in = dedic_gpio_bundle_read_in(mkbd->col_bundle);
    row_out = (~row_out) & ((1 << mkbd->nr_row_gpios) - 1);
    ESP_LOGD(TAG, "row_out=%"PRIx32", col_in=%"PRIx32, row_out, col_in);
    int row = -1;
    int col = -1;
    uint32_t key_code = 0;
    while (row_out) {
        row = __builtin_ffs(row_out) - 1;
        uint32_t changed_col_bits = mkbd->row_state[row] ^ col_in;
        while (changed_col_bits) {
            col = __builtin_ffs(changed_col_bits) - 1;
            ESP_LOGD(TAG, "row=%d, col=%d", row, col);
            key_code = MAKE_KEY_CODE(row, col);
            if (col_in & (1 << col)) {
                mkbd->event_handler(mkbd, MATRIX_KBD_EVENT_UP, (void *)key_code, mkbd->event_handler_args);
            } else {
                mkbd->event_handler(mkbd, MATRIX_KBD_EVENT_DOWN, (void *)key_code, mkbd->event_handler_args);
            }
            changed_col_bits = changed_col_bits & (changed_col_bits - 1);
        }
        mkbd->row_state[row] = col_in;
        row_out = row_out & (row_out - 1);
    }

    // row lines set to high level
    dedic_gpio_bundle_write(mkbd->row_bundle, (1 << mkbd->nr_row_gpios) - 1, (1 << mkbd->nr_row_gpios) - 1);
    // col lines set to low level
    dedic_gpio_bundle_write(mkbd->col_bundle, (1 << mkbd->nr_col_gpios) - 1, 0);
    dedic_gpio_bundle_set_interrupt_and_callback(mkbd->row_bundle, (1 << mkbd->nr_row_gpios) - 1,
            DEDIC_GPIO_INTR_BOTH_EDGE, matrix_kbd_row_isr_callback, mkbd);
}

esp_err_t matrix_kbd_install(const matrix_kbd_config_t *config, matrix_kbd_handle_t *mkbd_handle)
{
    esp_err_t ret_code = ESP_OK;
    matrix_kbd_t *mkbd = NULL;
    MKBD_CHECK(config, "matrix keyboard configuration can't be null", err, ESP_ERR_INVALID_ARG);
    MKBD_CHECK(mkbd_handle, "matrix keyboard handle can't be null", err, ESP_ERR_INVALID_ARG);

    mkbd = calloc(1, sizeof(matrix_kbd_t) + (config->nr_row_gpios) * sizeof(uint32_t));
    MKBD_CHECK(mkbd, "allocate matrix keyboard context failed", err, ESP_ERR_NO_MEM);

    mkbd->nr_col_gpios = config->nr_col_gpios;
    mkbd->nr_row_gpios = config->nr_row_gpios;

    // GPIO pad configuration
    // Each GPIO used in matrix key board should be able to input and output
    // In case the keyboard doesn't design a resister to pull up row/col line
    // We enable the internal pull up resister, enable Open Drain as well
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,
        .pull_up_en = 1
    };

    for (int i = 0; i < config->nr_row_gpios; i++) {
        io_conf.pin_bit_mask = 1ULL << config->row_gpios[i];
        gpio_config(&io_conf);
    }

    dedic_gpio_bundle_config_t bundle_row_config = {
        .gpio_array = config->row_gpios,
        .array_size = config->nr_row_gpios,
        .flags = {
            .in_en = 1,
            .out_en = 1,
        },
    };
    MKBD_CHECK(dedic_gpio_new_bundle(&bundle_row_config, &mkbd->row_bundle) == ESP_OK,
               "create row bundle failed", err, ESP_FAIL);

    for (int i = 0; i < config->nr_col_gpios; i++) {
        io_conf.pin_bit_mask = 1ULL << config->col_gpios[i];
        gpio_config(&io_conf);
    }

    dedic_gpio_bundle_config_t bundle_col_config = {
        .gpio_array = config->col_gpios,
        .array_size = config->nr_col_gpios,
        .flags = {
            .in_en = 1,
            .out_en = 1,
        },
    };
    MKBD_CHECK(dedic_gpio_new_bundle(&bundle_col_config, &mkbd->col_bundle) == ESP_OK,
               "create col bundle failed", err, ESP_FAIL);

    // Disable interrupt
    dedic_gpio_bundle_set_interrupt_and_callback(mkbd->row_bundle, (1 << config->nr_row_gpios) - 1,
            DEDIC_GPIO_INTR_NONE, NULL, NULL);
    dedic_gpio_bundle_set_interrupt_and_callback(mkbd->col_bundle, (1 << config->nr_col_gpios) - 1,
            DEDIC_GPIO_INTR_NONE, NULL, NULL);

    // Create a ont-shot os timer, used for key debounce
    mkbd->debounce_timer = xTimerCreate("kb_debounce", pdMS_TO_TICKS(config->debounce_ms), pdFALSE, mkbd, matrix_kbd_debounce_timer_callback);
    MKBD_CHECK(mkbd->debounce_timer, "create debounce timer failed", err, ESP_FAIL);

    * mkbd_handle = mkbd;
    return ESP_OK;
err:
    if (mkbd) {
        if (mkbd->debounce_timer) {
            xTimerDelete(mkbd->debounce_timer, 0);
        }
        if (mkbd->col_bundle) {
            dedic_gpio_del_bundle(mkbd->col_bundle);
        }
        if (mkbd->row_bundle) {
            dedic_gpio_del_bundle(mkbd->row_bundle);
        }
        free(mkbd);
    }
    return ret_code;
}

esp_err_t matrix_kbd_uninstall(matrix_kbd_handle_t mkbd_handle)
{
    esp_err_t ret_code = ESP_OK;
    MKBD_CHECK(mkbd_handle, "matrix keyboard handle can't be null", err, ESP_ERR_INVALID_ARG);
    xTimerDelete(mkbd_handle->debounce_timer, 0);
    dedic_gpio_del_bundle(mkbd_handle->col_bundle);
    dedic_gpio_del_bundle(mkbd_handle->row_bundle);
    free(mkbd_handle);
    return ESP_OK;
err:
    return ret_code;
}

esp_err_t matrix_kbd_start(matrix_kbd_handle_t mkbd_handle)
{
    esp_err_t ret_code = ESP_OK;
    MKBD_CHECK(mkbd_handle, "matrix keyboard handle can't be null", err, ESP_ERR_INVALID_ARG);

    // row lines set to high level
    dedic_gpio_bundle_write(mkbd_handle->row_bundle, (1 << mkbd_handle->nr_row_gpios) - 1, (1 << mkbd_handle->nr_row_gpios) - 1);
    // col lines set to low level
    dedic_gpio_bundle_write(mkbd_handle->col_bundle, (1 << mkbd_handle->nr_col_gpios) - 1, 0);

    for (int i = 0; i < mkbd_handle->nr_row_gpios; i++) {
        mkbd_handle->row_state[i] = (1 << mkbd_handle->nr_col_gpios) - 1;
    }

    // only enable row line interrupt
    dedic_gpio_bundle_set_interrupt_and_callback(mkbd_handle->row_bundle, (1 << mkbd_handle->nr_row_gpios) - 1,
            DEDIC_GPIO_INTR_BOTH_EDGE, matrix_kbd_row_isr_callback, mkbd_handle);

    return ESP_OK;
err:
    return ret_code;
}

esp_err_t matrix_kbd_stop(matrix_kbd_handle_t mkbd_handle)
{
    esp_err_t ret_code = ESP_OK;
    MKBD_CHECK(mkbd_handle, "matrix keyboard handle can't be null", err, ESP_ERR_INVALID_ARG);

    xTimerStop(mkbd_handle->debounce_timer, 0);

    // Disable interrupt
    dedic_gpio_bundle_set_interrupt_and_callback(mkbd_handle->row_bundle, (1 << mkbd_handle->nr_row_gpios) - 1,
            DEDIC_GPIO_INTR_NONE, NULL, NULL);
    dedic_gpio_bundle_set_interrupt_and_callback(mkbd_handle->col_bundle, (1 << mkbd_handle->nr_col_gpios) - 1,
            DEDIC_GPIO_INTR_NONE, NULL, NULL);

    return ESP_OK;
err:
    return ret_code;
}

esp_err_t matrix_kbd_register_event_handler(matrix_kbd_handle_t mkbd_handle, matrix_kbd_event_handler handler, void *args)
{
    esp_err_t ret_code = ESP_OK;
    MKBD_CHECK(mkbd_handle, "matrix keyboard handle can't be null", err, ESP_ERR_INVALID_ARG);
    mkbd_handle->event_handler = handler;
    mkbd_handle->event_handler_args = args;
    return ESP_OK;
err:
    return ret_code;
}
