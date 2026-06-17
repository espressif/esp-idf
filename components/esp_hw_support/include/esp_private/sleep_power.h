/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_regdma.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/startup_internal.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_BT_CTRL_SLEEP_ENABLE && SOC_PM_MODEM_LOCK_CLK_WORKAROUND

/**
 * @brief System power retention initialize.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM not enough memory for system power retention
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 */
esp_err_t sleep_power_system_retention_init(void *arg);

#endif

#ifdef __cplusplus
}
#endif
