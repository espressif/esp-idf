// Copyright 2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

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
    size_t max_fds;     /*!< The maxinum number of eventfds supported */
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
 * @brief Creates an event file descirptor.
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
