/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Wakes the OpenThread main loop on RCP when custom transport RX data is available.
 *
 * Call this from another task or ISR (after RX data has been queued for transport_rx) when using
 * @ref esp_openthread_transport_config_t so select() returns without waiting for the full timeout.
 *
 */
void esp_openthread_transport_notify_rcp_rx(void);

/**
 * @brief Wakes the OpenThread main loop on Host when custom transport RX data is available.
 *
 * Call this from another task or ISR (after RX data has been queued for transport_rx) when using
 * @ref esp_openthread_transport_config_t so select() returns without waiting for the full timeout.
 *
 */
void esp_openthread_transport_notify_host_rx(void);

#ifdef __cplusplus
} // end of extern "C"
#endif
