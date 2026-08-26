/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include "esp_openthread.h"
#include "esp_openthread_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes the OpenThread spinel SPI slave.
 *
 */
esp_err_t esp_openthread_host_rcp_spi_init(const esp_openthread_platform_config_t *config);

/**
 * @brief This function deinitializes the OpenThread spinel SPI slave.
 *
 */
void esp_openthread_spi_slave_deinit(void);

#ifdef __cplusplus
} /* extern "C" */
#endif
