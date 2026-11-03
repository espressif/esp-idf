/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sleep_sys_periph.h
 *
 * This file contains declarations of digital peripheral retention related functions in light sleep mode.
 */

/**
 * @brief Whether to allow the TOP power domain to be powered off.
 *
 * In light sleep mode, only when the system can provide enough memory
 * for digital peripheral retention, the TOP power domain can be powered off.
 *
 * @return True to allow power off
 */
bool peripheral_domain_pd_allowed(void);

#if SOC_PM_SUPPORT_CNNT_PD
/**
 * @brief Whether to allow the CNNT power domain to be powered off.
 *
 * CNNT (USB / SDMMC / Ethernet) can stay powered while TOP is down. For example,
 * EMAC retention is required only when CNNT itself is powered off.
 *
 * @return True to allow power off
 */
bool cnnt_domain_pd_allowed(void);
#endif

#ifdef __cplusplus
}
#endif
