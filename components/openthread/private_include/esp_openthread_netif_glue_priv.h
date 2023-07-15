/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_openthread.h"
#include "openthread/ip6.h"
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function updates the netif fds and timeouts to the main loop.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_netif_glue_update(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function performs the netif process.
 *
 * @param[in]    instance   The OpenThread instance.
 * @param[in]    mainloop   The main loop context.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on OpenThread failure
 *      - ESP_ERR_NO_MEM on memory allocation failure
 *
 */
esp_err_t esp_openthread_netif_glue_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function judges the target address is openthread mesh local or not.
 *
 * @param[in]    address   The address.
 *
 * @return
 *      - True if the address is openthread mesh local, otherwise false
 *
 */
bool is_openthread_internal_mesh_local_addr(const otIp6Address *address);

#ifdef __cplusplus
}
#endif
