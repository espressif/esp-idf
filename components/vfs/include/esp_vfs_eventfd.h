/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <sys/types.h>

#include "esp_err.h"

#define EFD_SUPPORT_ISR (1 << 4)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Eventfd vfs initialization settings
 */
typedef struct {
    size_t max_fds;     /*!< The maximum number of eventfds supported */
} esp_vfs_eventfd_config_t;

#define ESP_VFS_EVENTD_CONFIG_DEFAULT() (esp_vfs_eventfd_config_t) { \
      .max_fds = 5, \
};

/**
 * @brief  Registers the event vfs.
 *
 * @return  ESP_OK if successful, ESP_ERR_NO_MEM if too many VFSes are
 *          registered.
 */
esp_err_t esp_vfs_eventfd_register(const esp_vfs_eventfd_config_t *config);

/**
 * @brief  Unregisters the event vfs.
 *
 * @return ESP_OK if successful, ESP_ERR_INVALID_STATE if VFS for given prefix
 *         hasn't been registered
 */
esp_err_t esp_vfs_eventfd_unregister(void);

/*
 * @brief Creates an event file descriptor.
 *
 * The behavior of read, write and select is the same as man(2) eventfd with
 * EFD_SEMAPHORE **NOT** specified. A new flag EFD_SUPPORT_ISR has been added.
 * This flag is required to write to event fds in interrupt handlers. Accessing
 * the control blocks of event fds with EFD_SUPPORT_ISR will cause interrupts to
 * be temporarily blocked (e.g. during read, write and beginning and ending of
 * the * select).
 *
 * @return The file descriptor if successful, -1 if error happens.
 */
int eventfd(unsigned int initval, int flags);

#ifdef __cplusplus
}
#endif
