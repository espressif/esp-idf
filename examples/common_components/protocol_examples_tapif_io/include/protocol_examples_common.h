/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Common functions for protocol examples, to establish tap interface connection
 * For linux target

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */
#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configure and connect, wait for IP
 *
 * @return ESP_OK on successful connection
 */
esp_err_t example_connect(void);


#ifdef __cplusplus
}
#endif
