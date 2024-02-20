/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/rmt_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t test_rmt_new_led_strip_encoder(rmt_encoder_handle_t *ret_encoder);

esp_err_t test_rmt_new_nec_protocol_encoder(rmt_encoder_handle_t *ret_encoder);

#ifdef __cplusplus
}
#endif
