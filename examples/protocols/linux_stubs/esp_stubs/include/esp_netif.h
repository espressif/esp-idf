/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <sys/ioctl.h>
#include <net/if.h>
#include <ifaddrs.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t esp_netif_init(void);

#ifdef __cplusplus
}
#endif
