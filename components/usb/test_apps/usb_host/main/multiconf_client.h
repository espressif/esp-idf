/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

typedef struct {
    SemaphoreHandle_t dev_open_smp;
    uint8_t bConfigurationValue;
} multiconf_client_test_param_t;

/**
 * @brief Multiconfiguration client task
 */
void multiconf_client_async_task(void *arg);

/**
 * @brief Get configuration descriptor
 */
void multiconf_client_get_conf_desc(void);
