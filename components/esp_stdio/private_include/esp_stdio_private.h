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
 * s_primary_stack[0] is the immovable system-registered sentinel.
 * s_primary_stack[s_primary_index] is the active primary.
 * Auxiliaries are any in-use pool entries not on the stack.
 */
extern esp_stdio_entry_t s_entry_pool[STDIO_MAX_ENTRIES];
extern esp_stdio_entry_t *s_primary_stack[STDIO_MAX_ENTRIES];
extern int s_primary_index;
extern int s_open_count;
extern _lock_t s_lock;

/* Low-level helpers — defined in stdio_vfs.c, called from stdio_mux_api.c. */
void entry_open(esp_stdio_entry_t *e, int flags);
void entry_close(esp_stdio_entry_t *e);

/**
 * @brief Return true if the active primary is NOT the system-registered backend.
 *
 * Used by stdio_port.c to decide whether to skip hardware init/deinit
 * (the caller of esp_stdio_push_primary() is responsible for their own driver
 * lifecycle).
 */
bool esp_stdio_has_user_primary(void);

#endif // CONFIG_VFS_SUPPORT_IO

#ifdef __cplusplus
}
#endif
