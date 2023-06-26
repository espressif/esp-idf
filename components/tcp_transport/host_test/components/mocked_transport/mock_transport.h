/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_transport.h"

 int mock_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms);

 int mock_close(esp_transport_handle_t t);

 int mock_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms);

 int mock_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms);

 int mock_poll_read(esp_transport_handle_t t, int timeout_ms);

 int mock_poll_write(esp_transport_handle_t t, int timeout_ms);

 esp_err_t mock_destroy(esp_transport_handle_t t);
