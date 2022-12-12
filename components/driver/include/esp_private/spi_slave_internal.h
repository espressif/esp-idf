/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 * This file contains SPI Slave private/internal APIs. Private/Internal APIs are:
 * - Visible to other IDF components
 * - Suggest NOT to use these APIs in your applications
 * - We don't provide backward compatibility on these APIs either
 */

#pragma once
#include "sdkconfig.h"
#include "esp_err.h"
#include "hal/spi_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @note
 * This API is used to reset SPI Slave transaction queue. After calling this function:
 * - The SPI Slave transaction queue will be reset.
 *
 * @note This API shouldn't be called when the corresponding SPI Master is doing an SPI transaction.
 * If this gets called when its corresponding SPI Master is doing an SPI transaction, the SPI Slave behaviour is undefined
 *
 * @param host SPI peripheral that is acting as a slave
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t spi_slave_queue_reset(spi_host_device_t host);
