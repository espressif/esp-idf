/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_stdio_cli_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_VFS_DEV_CONSOLE "/dev/console"

/**
 * @brief Register the default console VFS backend(s) selected in Kconfig.
 *
 * Sets up the primary (and any Kconfig-selected secondary) console sink and
 * mounts them under /dev/console. This function is automatically called from
 * startup code to enable serial output; applications normally do not need to
 * call it themselves.
 *
 * @return ESP_OK on success, or an error code from the underlying VFS registration.
 */
esp_err_t esp_stdio_register(void);

#if CONFIG_VFS_SUPPORT_IO

#include "esp_vfs_common.h"
#include "esp_vfs_ops.h"

/**
 * @brief Configuration for registering a console I/O driver in the mux.
 *
 * The caller must keep the ops table, context, and path string valid until
 * esp_stdio_unregister_driver() is called.
 */
typedef struct {
    const esp_vfs_fs_ops_t *vfs_ops;  /*!< VFS operations table (must not be NULL) */
    void *vfs_ctx;                     /*!< Context pointer forwarded to every VFS callback */
    const char *path;                  /*!< Path used for open() inside the driver (e.g. "/0", "/") */
} esp_stdio_driver_config_t;

/** @brief Opaque handle representing a registered console I/O driver. */
typedef struct esp_stdio_entry *esp_stdio_handle_t;

/**
 * @brief Register a VFS driver as a write-only auxiliary sink in the console mux.
 *
 * The driver is added to the multiplexer as an auxiliary: once /dev/console is
 * open, every byte written to stdout/stderr is fanned out to it in addition to
 * the primary.  To use it as the read+write primary instead, promote it with
 * esp_stdio_set_primary_driver().
 *
 * The caller is responsible for initialising the driver before calling this
 * function.  The ops table, context, and path string must remain valid until
 * esp_stdio_unregister_driver() is called.
 *
 * @param config     Pointer to the driver configuration (must not be NULL;
 *                   vfs_ops and path must not be NULL).
 * @param out_handle Receives the opaque handle on success (must not be NULL).
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if any required pointer is NULL
 *         ESP_ERR_NO_MEM if the entry pool is exhausted
 */
esp_err_t esp_stdio_register_driver(const esp_stdio_driver_config_t *config,
                                    esp_stdio_handle_t *out_handle);

/**
 * @brief Unregister a driver from the console mux.
 *
 * The driver's backend fd is closed if it was open and its pool slot is
 * released.  A driver that is currently the active primary cannot be removed:
 * promote a different driver with esp_stdio_set_primary_driver() first.  After
 * a successful call the handle is invalid.
 *
 * @note Unregistering the Kconfig-selected boot console (the handle returned by
 *       the very first esp_stdio_set_primary_driver() call) is a one-way
 *       transition: its pool slot is freed and the saved handle becomes stale,
 *       so it cannot be restored later, and the internal "user owns the
 *       primary" state stays latched for the remaining lifetime.  Prefer
 *       demoting the boot console (promote another driver) over unregistering
 *       it if you may need to restore it.
 *
 * @param handle A valid handle returned by esp_stdio_register_driver().
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if handle is NULL or already unregistered
 *         ESP_ERR_INVALID_STATE if handle is the active primary
 */
esp_err_t esp_stdio_unregister_driver(esp_stdio_handle_t handle);

/**
 * @brief Select the active read+write primary driver.
 *
 * The given driver becomes the source for console reads and the target for
 * console writes.  The previously active primary is not removed — it stays
 * registered as a write-only auxiliary and keeps receiving the fan-out.
 *
 * The previous primary's handle is returned through @p out_old_primary so the
 * caller can restore it later (a simple save/restore that composes across
 * nested overrides).  The very first call returns the handle of the
 * Kconfig-selected default console.
 *
 * @param handle          Handle of the driver to make primary (must be a valid,
 *                        registered handle returned by
 *                        esp_stdio_register_driver()).
 * @param out_old_primary Receives the handle of the previously active primary,
 *                        or NULL if the caller does not need it.
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if handle is NULL or already unregistered
 */
esp_err_t esp_stdio_set_primary_driver(esp_stdio_handle_t handle,
                                       esp_stdio_handle_t *out_old_primary);

#endif // CONFIG_VFS_SUPPORT_IO

/**
 * @brief Install and enable the stdio driver.
 *
 * Initializes the selected console backend and registers it as the active
 * input and output stream source. After calling this function, standard I/O
 * functions such as `printf` and terminal input will use the configured backend.
 *
 * Line endings are automatically configured for typical terminal use:
 * - RX: CR (terminals send CR when Enter is pressed)
 * - TX: CRLF (move cursor to beginning of next line on newline)
 *
 * If a user has taken over the primary console with esp_stdio_set_primary_driver(),
 * this function is a no-op (the user backend is assumed to be already
 * initialised by the caller).
 *
 * @return ESP_OK if the driver is successfully installed, or an appropriate error code otherwise.
 */
esp_err_t esp_stdio_install_io_driver(void);

/**
 * @brief Uninstall the active stdio driver.
 *
 * Restores driver state and detaches the console backend from the standard I/O
 * streams. Call this when shutting down or when replacing the current console
 * driver with a different one.
 *
 * If a user has taken over the primary console with esp_stdio_set_primary_driver(),
 * this function is a no-op.
 */
void esp_stdio_uninstall_io_driver(void);

#ifdef __cplusplus
}
#endif
