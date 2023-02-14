/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

typedef struct analog_cmpr_t *analog_cmpr_handle_t;
typedef struct analog_cmpr_ref_voltage_t *analog_cmpr_ref_voltage_handle_t;

typedef struct {
    analog_cmpr_ref_voltage_handle_t ref_volt;
    struct {
        uint32_t detect_pos: 1;
        uint32_t detect_neg: 1;
    } flags;
} analog_cmpr_config_t;

esp_err_t analog_cmpr_new_unit(const analog_cmpr_config_t *config, analog_cmpr_handle_t *ret_cmpr);

typedef struct {
    float ref_voltage;
} analog_cmpr_internal_ref_voltage_config_t;

typedef struct {
    int ref_volt_gpio_num;
} analog_cmpr_gpio_ref_voltage_config_t;

esp_err_t analog_cmpr_new_gpio_ref_voltage(const analog_cmpr_gpio_ref_voltage_config_t *config, analog_cmpr_ref_voltage_handle_t *ret_ref_volt);

esp_err_t analog_cmpr_new_internal_ref_voltage(const analog_cmpr_internal_ref_voltage_config_t *config, analog_cmpr_ref_voltage_handle_t *ret_ref_volt);

typedef struct {

} analog_cmpr_cross_event_data_t;

typedef bool (*analog_cmpr_cross_cb_t) (analog_cmpr_handle_t cmpr, const analog_cmpr_cross_event_data_t *edata, void *user_ctx);

typedef struct {
    analog_cmpr_cross_cb_t on_cross;
} analog_cmpr_event_callbacks_t;

esp_err_t analog_cmpr_register_event_callbacks(analog_cmpr_handle_t cmpr, const analog_cmpr_event_callbacks_t *cbs, void *user_data);

typedef struct {

} analog_cmpr_debounce_filter_config_t;

esp_err_t analog_cmpr_set_debounce_filter(analog_cmpr_handle_t cmpr, const analog_cmpr_debounce_filter_config_t *config);

esp_err_t analog_cmpr_enable(analog_cmpr_handle_t cmpr);
esp_err_t analog_cmpr_disable(analog_cmpr_handle_t cmpr);
