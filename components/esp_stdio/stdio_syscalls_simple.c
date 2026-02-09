/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <reent.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include "sdkconfig.h"
#include "esp_rom_serial_output.h"
#include "esp_system_console.h"

ssize_t _write_r_console(struct _reent *r, int fd, const void * data, size_t size)
{
#if !CONFIG_ESP_CONSOLE_NONE
    const char* cdata = (const char*) data;
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        for (size_t i = 0; i < size; ++i) {
            if (cdata[i] == '\n') {
                esp_system_console_put_char('\r');
            }
            esp_system_console_put_char(cdata[i]);
        }
        return size;
    }
#endif //!CONFIG_ESP_CONSOLE_NONE
    __errno_r(r) = EBADF;
    return -1;
}

ssize_t _read_r_console(struct _reent *r, int fd, void * data, size_t size)
{
#if !CONFIG_ESP_CONSOLE_NONE
    char* cdata = (char*) data;
    if (fd == STDIN_FILENO) {
        size_t received;
        for (received = 0; received < size; ++received) {
            int status = esp_rom_output_rx_one_char((uint8_t*) &cdata[received]);
            if (status != 0) {
                break;
            }
            if (cdata[received] == '\r') {
                cdata[received] = '\n';
            }
        }
        if (received == 0) {
            __errno_r(r) = EWOULDBLOCK;
            return -1;
        }
        return received;
    }
#endif //!CONFIG_ESP_CONSOLE_NONE
    __errno_r(r) = EBADF;
    return -1;
}

static ssize_t _fstat_r_console(struct _reent *r, int fd, struct stat * st)
{
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        memset(st, 0, sizeof(*st));
        /* This needs to be set so that stdout and stderr are line buffered. */
        st->st_mode = S_IFCHR;
        return 0;
    }
    __errno_r(r) = EBADF;
    return -1;
}

static int _fsync_console(int fd)
{
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        esp_rom_output_flush_tx(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);
        return 0;
    }
    errno = EBADF;
    return -1;
}

/* The following weak definitions of syscalls will be used unless
 * another definition is provided. That definition may come from
 * VFS, LWIP, or the application.
 */
ssize_t _read_r(struct _reent *r, int fd, void *dst, size_t size)
__attribute__((weak, alias("_read_r_console")));
ssize_t _write_r(struct _reent *r, int fd, const void *data, size_t size)
__attribute__((weak, alias("_write_r_console")));
int _fstat_r(struct _reent *r, int fd, struct stat *st)
__attribute__((weak, alias("_fstat_r_console")));
int fsync(int fd)
__attribute__((weak, alias("_fsync_console")));
