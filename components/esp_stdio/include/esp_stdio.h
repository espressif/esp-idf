/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

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
 * @brief Configuration for registering a console I/O backend in the mux.
 *
 * The caller must keep the ops table, context, and path string valid until
 * esp_stdio_unregister_io() is called.
 */
typedef struct {
    const esp_vfs_fs_ops_t *vfs_ops;  /*!< VFS operations table (must not be NULL) */
    void *vfs_ctx;                     /*!< Context pointer forwarded to every VFS callback */
    const char *path;                  /*!< Path used for open() inside the driver (e.g. "/0", "/") */
} esp_stdio_io_config_t;

/** @brief Opaque handle representing a registered console I/O backend. */
typedef struct esp_stdio_entry *esp_stdio_handle_t;

/**
 * @brief Register a VFS backend as a write-only auxiliary sink in the console mux.
 *
 * The backend starts receiving fan-out writes immediately (once /dev/console
 * is open).  To make it the active read+write primary, pass the returned
 * handle to esp_stdio_push_primary().
 *
 * The caller is responsible for initialising the driver before calling this
 * function.  The ops table, context, and path string must remain valid until
 * esp_stdio_unregister_io() is called.
 *
 * @param config     Pointer to the I/O configuration (must not be NULL; vfs_ops
 *                   and path must not be NULL).
 * @param out_handle Receives the opaque handle on success (must not be NULL).
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if any required pointer is NULL
 *         ESP_ERR_NO_MEM if the entry pool is exhausted
 */
esp_err_t esp_stdio_register_io(const esp_stdio_io_config_t *config,
                                esp_stdio_handle_t *out_handle);

/**
 * @brief Push an existing handle onto the primary stack.
 *
 * Removes the entry from the auxiliary list and makes it the active
 * read+write backend.  The previous primary is suspended on a stack and
 * resumes when esp_stdio_pop_primary() is called.
 *
 * @param handle A valid handle returned by esp_stdio_register_io().
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if handle is NULL or not currently registered as auxiliary
 *         ESP_ERR_NO_MEM if the primary stack is full
 */
esp_err_t esp_stdio_push_primary(esp_stdio_handle_t handle);

/**
 * @brief Remove a primary from the stack.
 *
 * The removed entry is returned to the auxiliary list.  When it was the active
 * (top) primary, the next-most-recent primary becomes active; the
 * system-registered primary at the base of the stack always remains as the
 * ultimate fallback and can never be removed.
 *
 * @param handle The primary to remove. Pass NULL to remove the current active
 *               (top) primary. Pass a specific handle to remove it from
 *               wherever it sits in the stack (owner-keyed removal, which lets
 *               independent owners release their own primary without disturbing
 *               a more-recently-pushed one).
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_STATE if there is no user-pushed primary to remove
 *                               (NULL), or if @p handle is not currently on the
 *                               primary stack
 */
esp_err_t esp_stdio_pop_primary(esp_stdio_handle_t handle);

/**
 * @brief Unregister a backend from the console mux entirely.
 *
 * The handle's backend fd is closed if it was open.  If the handle is
 * currently the active primary it is implicitly popped first (the previous
 * primary is restored).  If it is buried in the primary stack it is removed
 * from wherever it sits.  After this call the handle is invalid.
 *
 * @param handle A valid handle returned by esp_stdio_register_io().
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG if handle is NULL
 */
esp_err_t esp_stdio_unregister_io(esp_stdio_handle_t handle);

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
 * If a user has taken over the primary console with esp_stdio_push_primary(),
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
 * If a user has taken over the primary console with esp_stdio_push_primary(),
 * this function is a no-op.
 */
void esp_stdio_uninstall_io_driver(void);

#ifdef __cplusplus
}
#endif
