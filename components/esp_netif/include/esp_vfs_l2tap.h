/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#define L2TAP_VFS_DEFAULT_PATH "/dev/net/tap"
#define L2TAP_VFS_CONFIG_DEFAULT()                  \
    {                                               \
        .base_path = L2TAP_VFS_DEFAULT_PATH,        \
    }

#ifdef __cplusplus
extern "C" {
#endif

typedef void *l2tap_iodriver_handle;

/**
 * @brief L2Tap VFS config parameters
 *
 */
typedef struct {
    const char* base_path; /*!< vfs base path */
} l2tap_vfs_config_t;

typedef enum {
    L2TAP_S_RCV_FILTER,
    L2TAP_G_RCV_FILTER,
    L2TAP_S_INTF_DEVICE,
    L2TAP_G_INTF_DEVICE,
    L2TAP_S_DEVICE_DRV_HNDL,
    L2TAP_G_DEVICE_DRV_HNDL
} l2tap_ioctl_opt_t;

/**
 * @brief Add L2 TAP virtual filesystem driver
 *
 * This function must be called prior usage of ESP-NETIF L2 TAP Interface
 *
 * @param config L2 TAP virtual filesystem driver configuration. Default base path /dev/net/tap is used when this paramenter is NULL.
 * @return esp_err_t
 *          - ESP_OK on success
 */
esp_err_t esp_vfs_l2tap_intf_register(l2tap_vfs_config_t *config);

/**
 * @brief Removes L2 TAP virtual filesystem driver
 *
 * @param base_path Base path to the L2 TAP virtual filesystem driver. Default path /dev/net/tap is used when this paramenter is NULL.
 * @return esp_err_t
 *          - ESP_OK on success
 */
esp_err_t esp_vfs_l2tap_intf_unregister(const char *base_path);

/**
 * @brief Filters received Ethernet L2 frames into L2 TAP infrastructure.
 *
 * @param driver_handle handle of driver at which the frame was received
 * @param buff received L2 frame
 * @param size input length of the L2 frame which is set to 0 when frame is filtered into L2 TAP
 * @return esp_err_t
 *                  - ESP_OK is always returned
 */
esp_err_t esp_vfs_l2tap_eth_filter(l2tap_iodriver_handle driver_handle, void *buff, size_t *size);

#ifdef __cplusplus
}
#endif
