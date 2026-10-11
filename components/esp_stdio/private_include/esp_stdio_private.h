/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <fcntl.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_VFS_SUPPORT_IO

#define STDIO_MAX_ENTRIES CONFIG_ESP_STDIO_MAX_VFS_ENTRIES

typedef struct esp_stdio_entry {
    const esp_vfs_fs_ops_t *ops;
    void *vfs_ctx;
    const char *path;
    int fd;        /* backend fd; -1 when closed */
    bool in_use;   /* false = slot available */
} esp_stdio_entry_t;

/*
 * Shared mux state — defined in stdio_vfs.c, used by stdio_mux_api.c.
 *
 * All registered drivers live in the single pool s_entry_pool. "Primary" is
 * just a tag: s_active_primary points at the entry currently used for reads and
 * writes; every other in-use entry is a write-only auxiliary. s_default_primary
 * marks the Kconfig-selected boot console; it is the reference used to tell
 * whether a user has taken over the primary (see esp_stdio_has_user_primary()),
 * and is set to NULL if that console is ever unregistered. It is write-once at
 * boot and never re-established: once NULL, esp_stdio_has_user_primary() stays
 * latched true because s_active_primary is never NULL.
 */
extern esp_stdio_entry_t s_entry_pool[STDIO_MAX_ENTRIES];
extern esp_stdio_entry_t *s_active_primary;
extern esp_stdio_entry_t *s_default_primary;
extern int s_open_count;
extern _lock_t s_lock;

/* Low-level helpers — defined in stdio_vfs.c, called from stdio_mux_api.c. */
void entry_open(esp_stdio_entry_t *e, int flags);
void entry_close(esp_stdio_entry_t *e);

/**
 * @brief Return true if the active primary is NOT the Kconfig default backend.
 *
 * Used by stdio_port.c to decide whether to skip hardware init/deinit
 * (the caller that promoted its own primary is responsible for its own driver
 * lifecycle).
 *
 * Latches true once the boot console is unregistered (s_default_primary == NULL).
 */
bool esp_stdio_has_user_primary(void);

#endif // CONFIG_VFS_SUPPORT_IO

#ifdef __cplusplus
}
#endif
