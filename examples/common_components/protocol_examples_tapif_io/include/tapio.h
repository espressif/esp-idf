/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "esp_err.h"
#include "esp_netif.h"

/**
 * @brief Creates tapio layer as a driver interface to esp-netif
 *
 * @warning Implemented as singleton, can use only one tapio in the system!
 *
 * @return pointer to the tapio driver handle
 */
void *tapio_create(void);

/**
 * @brief esp-netif driver I/O output path
 *
 * @param h Driver's handle
 * @param buffer Data to output
 * @param len Data size
 * @return ESP_OK on success
 */
esp_err_t tapio_output(void *h, void *buffer, size_t len);
