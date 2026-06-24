/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdalign.h>
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
    L2TAP_S_RCV_FILTER,         /*!< Set Ethertype filter, frames with this type to be passed to the file descriptor. */
    L2TAP_G_RCV_FILTER,         /*!< Get current Ethertype filter. */
    L2TAP_S_INTF_DEVICE,        /*!< Bound the file descriptor to a specific Network Interface is identified by its ``if_key``. */
    L2TAP_G_INTF_DEVICE,        /*!< Get the Network Interface ``if_key`` the file descriptor is bound to. */
    L2TAP_S_DEVICE_DRV_HNDL,    /*!< Bound the file descriptor to a specific Network Interface identified by IO Driver handle. */
    L2TAP_G_DEVICE_DRV_HNDL,    /*!< Get the Network Interface IO Driver handle the file descriptor is bound to. */
    L2TAP_S_TIMESTAMP_EN,       /*!< Enables the hardware Time Stamping (TS) processing by the file descriptor. TS needs to be supported by hardware and enabled in the IO driver. */
} l2tap_ioctl_opt_t;

/**
 * @brief Information Record (IREC) Header Type indicates expected type of Header Data
 *
 */
typedef enum {
    L2TAP_IREC_INVALID      = -1,   /*!< Indicate invalid IREC (data is not valid)*/
    L2TAP_IREC_TIME_STAMP   = 1     /*!< To retrieve time stamp in `struct timespec` format */
} l2tap_irec_type_t;


/**
 * @brief Information Record (IREC)
 *
 */
typedef struct
{
    size_t len;                         /*!< Length of the record including header and data*/
    l2tap_irec_type_t type;             /*!< Type of the record */
    alignas(long long) uint8_t data[];  /*!< Records Data aligned to double word */
} l2tap_irec_hdr_t;

/**
 * @brief Extended Buffer
 *
 * @attention Use macros when allocating buffer for Information Records and when manipulating with data in the records
 *            to ensure proper memory alignment
 *
 */
typedef struct {
    size_t info_recs_len;       /*!< Length of Information Records buffer */
    void *info_recs_buff;       /*!< Buffer holding extended information (IRECs) related to IO frames */
    size_t buff_len;            /*!< Length of the actual IO Frame buffer */
    void *buff;                 /*!< Pointer to the IO Frame buffer */
} l2tap_extended_buff_t;


/**
 * @brief Macros for operations with Information Records
 *
 * Align to double word size to each info record starts aligned in memory even if not aligned
 * info data size is used by previous record. Double word alignment (at 32-bit arch) is needed when accessing
 * double word variables or structs containing double word variables.
 *
 */
#define L2TAP_ALIGN(size) (((size) + sizeof(long long) - 1U) & ~(sizeof(long long) - 1U))

#define L2TAP_IREC_LEN(size)                    (sizeof(l2tap_irec_hdr_t) + size)
#define L2TAP_IREC_SPACE(size)                  (L2TAP_ALIGN(L2TAP_IREC_LEN(size)))
#define L2TAP_IREC_FIRST(ext_buff)              (ext_buff)->info_recs_len >= sizeof(l2tap_irec_hdr_t) ? (l2tap_irec_hdr_t *)(ext_buff)->info_recs_buff : NULL
#define L2TAP_IREC_NEXT(ext_buff, curr_rec)     (((curr_rec) == NULL) ? L2TAP_IREC_FIRST(ext_buff) : \
                                                (uint8_t *)(curr_rec) + L2TAP_ALIGN((curr_rec)->len) + L2TAP_ALIGN(sizeof(l2tap_irec_hdr_t)) > \
                                                    (uint8_t *)(ext_buff)->info_recs_buff + (ext_buff)->info_recs_len ? \
                                                        NULL : \
                                                        (l2tap_irec_hdr_t *)(void *)(((uint8_t *)(curr_rec) + L2TAP_ALIGN((curr_rec)->len))))

/**
 * @brief Add L2 TAP virtual filesystem driver
 *
 * This function must be called prior usage of ESP-NETIF L2 TAP Interface
 *
 * @param config L2 TAP virtual filesystem driver configuration. Default base path /dev/net/tap is used when this parameter is NULL.
 * @return esp_err_t
 *          - ESP_OK on success
 */
esp_err_t esp_vfs_l2tap_intf_register(l2tap_vfs_config_t *config);

/**
 * @brief Removes L2 TAP virtual filesystem driver
 *
 * @param base_path Base path to the L2 TAP virtual filesystem driver. Default path /dev/net/tap is used when this parameter is NULL.
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
 * @param info extra information about received Ethernet frame
 * @return esp_err_t
 *                  - ESP_OK is always returned
 */
esp_err_t esp_vfs_l2tap_eth_filter_frame(l2tap_iodriver_handle driver_handle, void *buff, size_t *size, void *info);

/**
 * @brief Wrapper over L2 TAP filter function to ensure backward compatibility.
 *
 * This macro is provided for backward compatibility with the original `esp_vfs_l2tap_eth_filter` function.
 * It calls `esp_vfs_l2tap_eth_filter_frame()` with the `info` parameter set to `NULL`, which means
 * L2 TAP features that depend on extra information about the received Ethernet frame (e.g., timestamps)
 * will not work as expected.
 *
 * @note For new implementations, it is recommended to use `esp_vfs_l2tap_eth_filter_frame()` directly to
 *       take advantage of the extended functionality.
 */
#define esp_vfs_l2tap_eth_filter(drv_hndl, buf, size) esp_vfs_l2tap_eth_filter_frame(drv_hndl, buf, size, NULL)


#ifdef __cplusplus
}
#endif
