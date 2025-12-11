/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <sys/types.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pipe vfs initialization settings
 */
typedef struct {
    size_t max_fds;     /*!< The maximum number of pipes supported */
} esp_vfs_pipe_config_t;

#define ESP_VFS_PIPE_CONFIG_DEFAULT() (esp_vfs_pipe_config_t) { \
      .max_fds = 20, \
};

/**
 * @brief  Registers the pipe vfs.
 *
 * @return  ESP_OK if successful, ESP_ERR_NO_MEM if too many VFSes are
 *          registered.
 */
esp_err_t esp_vfs_pipe_register(const esp_vfs_pipe_config_t *config);

/**
 * @brief  Unregisters the pipe vfs.
 *
 * @return ESP_OK if successful, ESP_ERR_INVALID_STATE if VFS for given prefix
 *         hasn't been registered
 */
esp_err_t esp_vfs_pipe_unregister(void);

/*
 * @brief Creates a pair of pipe file descriptor.
 *
 * A pipe is a unidirectional data channel that can be used for interprocess
 * It uses blocking read and write operations, so it can't be used in interrupt context.
 *
 * @return The file descriptor if successful, -1 if error happens.
 */
int pipe(int fildes[2]);

#ifdef __cplusplus
}
#endif
