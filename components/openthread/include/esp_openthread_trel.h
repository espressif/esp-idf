/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_netif.h"
#include "esp_netif_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sets the interface used for trel feature.
 *
 * @note This function must be called after connect to wifi/ethernet
 *
 * @param[in]  trel_netif   The network interface (WiFi or ethernet)
 *
 */
void esp_openthread_set_trel_netif(esp_netif_t *trel_netif);

/**
 * @brief   Gets the trel interface of OpenThread device.
 *
 * @return
 *      The trel interface or NULL if trel not initialized.
 *
 */
esp_netif_t *esp_openthread_get_trel_netif(void);

#ifdef __cplusplus
}
#endif
