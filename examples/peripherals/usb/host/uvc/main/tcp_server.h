/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t tcp_server_wait_for_connection(void);

esp_err_t tcp_server_send(uint8_t *payload, size_t size);

void tcp_server_close_when_done(void);

#ifdef __cplusplus
}
#endif
