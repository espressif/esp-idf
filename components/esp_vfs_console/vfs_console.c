/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <fcntl.h>
#include "esp_err.h"
#include "esp_rom_sys.h"
#include "esp_vfs_cdcacm.h"
#include "esp_private/esp_vfs_cdcacm.h"
#include "driver/esp_private/usb_serial_jtag_vfs.h"
#include "driver/esp_private/uart_vfs.h"
#include "esp_private/usb_console.h"
#include "esp_vfs_console.h"
#include "sdkconfig.h"
#include "esp_private/startup_internal.h"
#include "esp_vfs_null.h"
#include "esp_private/nullfs.h"

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

/**
 * This file is to concentrate all the vfs(UART, USB_SERIAL_JTAG, CDCACM) console into one single file.
 * Get the vfs information from their component (i.e. uart_vfs.c),
 * which can help us to output some string to two different ports(i.e both through uart and usb_serial_jtag).
 * Usually, we set a port as primary and another as secondary. For primary, it is used for all the features supported by each vfs implementation,
 * while the secondary is only used for output.
 */

typedef struct {
    int fd_primary;
    int fd_secondary;
} vfs_console_context_t;

#if CONFIG_VFS_SUPPORT_IO

// Secondary register part.
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
const static esp_vfs_fs_ops_t *secondary_vfs = NULL;
#endif // Secondary part

const static esp_vfs_fs_ops_t *primary_vfs = NULL;

static vfs_console_context_t vfs_console = {0};

int console_open(const char * path, int flags, int mode)
{
// Primary port open
#if CONFIG_ESP_CONSOLE_UART
    vfs_console.fd_primary = open("/dev/uart/"STRINGIFY(CONFIG_ESP_CONSOLE_UART_NUM), flags, mode);
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    vfs_console.fd_primary = open("/dev/usbserjtag", flags, mode);
#elif CONFIG_ESP_CONSOLE_USB_CDC
    vfs_console.fd_primary = open("/dev/cdcacm", flags, mode);
#else
    vfs_console.fd_primary = open("/dev/null", flags, mode);
#endif

// Secondary port open
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    vfs_console.fd_secondary = open("/dev/secondary", flags, mode);
#endif
    return 0;
}

ssize_t console_write(int fd, const void *data, size_t size)
{
    // All function calls are to primary, except from write and close, which will be forwarded to both primary and secondary.
    write(vfs_console.fd_primary, data, size);
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    write(vfs_console.fd_secondary, data, size);
#endif
    return size;
}

int console_fstat(int fd, struct stat * st)
{
    return fstat(vfs_console.fd_primary, st);
}

int console_close(int fd)
{
    // All function calls are to primary, except from write and close, which will be forwarded to both primary and secondary.
    close(vfs_console.fd_primary);
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    close(vfs_console.fd_secondary);
#endif
    return 0;
}

ssize_t console_read(int fd, void * dst, size_t size)
{
    return read(vfs_console.fd_primary, dst, size);
}

int console_fcntl(int fd, int cmd, int arg)
{
    return fcntl(vfs_console.fd_primary, cmd, arg);
}

int console_fsync(int fd)
{
    const int ret_val = fsync(vfs_console.fd_primary);
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    (void)fsync(vfs_console.fd_secondary);
#endif
    return ret_val;
}

#ifdef CONFIG_VFS_SUPPORT_DIR
int console_access(const char *path, int amode)
{
    // currently only UART support DIR.
    return access("/dev/uart/"STRINGIFY(CONFIG_ESP_CONSOLE_UART_NUM), amode);
}
#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_SELECT
static esp_err_t console_start_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                                      esp_vfs_select_sem_t select_sem, void **end_select_args)
{
    // start_select is not guaranteed be implemented even though CONFIG_VFS_SUPPORT_SELECT is enabled in sdkconfig
    if (primary_vfs->select->start_select) {
        return primary_vfs->select->start_select(nfds, readfds, writefds, exceptfds, select_sem, end_select_args);
    }

    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t console_end_select(void *end_select_args)
{
    // end_select is not guaranteed be implemented even though CONFIG_VFS_SUPPORT_SELECT is enabled in sdkconfig
    if (primary_vfs->select->end_select) {
        return primary_vfs->select->end_select(end_select_args);
    }

    return ESP_ERR_NOT_SUPPORTED;
}

#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_TERMIOS

int console_tcsetattr(int fd, int optional_actions, const struct termios *p)
{
    return tcsetattr(vfs_console.fd_primary, optional_actions, p);
}

int console_tcgetattr(int fd, struct termios *p)
{
    return tcgetattr(vfs_console.fd_primary, p);
}

int console_tcdrain(int fd)
{
    return tcdrain(vfs_console.fd_primary);
}

int console_tcflush(int fd, int select)
{
    return tcflush(vfs_console.fd_primary, select);
}
#endif // CONFIG_VFS_SUPPORT_TERMIOS

#ifdef CONFIG_VFS_SUPPORT_DIR
static const esp_vfs_dir_ops_t s_vfs_console_dir = {
    .access = &console_access,
};
#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_SELECT
static const esp_vfs_select_ops_t s_vfs_console_select = {
    .start_select = &console_start_select,
    .end_select = &console_end_select,
};
#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
static const esp_vfs_termios_ops_t s_vfs_console_termios = {
    .tcsetattr = &console_tcsetattr,
    .tcgetattr = &console_tcgetattr,
    .tcdrain = &console_tcdrain,
    .tcflush = &console_tcflush,
};
#endif // CONFIG_VFS_SUPPORT_TERMIOS

static const esp_vfs_fs_ops_t s_vfs_console = {
    .write = &console_write,
    .open = &console_open,
    .fstat = &console_fstat,
    .close = &console_close,
    .read = &console_read,
    .fcntl = &console_fcntl,
    .fsync = &console_fsync,

#ifdef CONFIG_VFS_SUPPORT_DIR
    .dir = &s_vfs_console_dir,
#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_SELECT
    .select = &s_vfs_console_select,
#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    .termios = &s_vfs_console_termios,
#endif // CONFIG_VFS_SUPPORT_TERMIOS
};

static esp_err_t esp_vfs_dev_console_register(void)
{
    return esp_vfs_register_fs(ESP_VFS_DEV_CONSOLE, &s_vfs_console, ESP_VFS_FLAG_STATIC, NULL);
}

esp_err_t esp_vfs_console_register(void)
{
    esp_err_t err = ESP_OK;
// Primary vfs part.
#if CONFIG_ESP_CONSOLE_UART
    primary_vfs = esp_vfs_uart_get_vfs();
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    primary_vfs = esp_vfs_usb_serial_jtag_get_vfs();
#elif CONFIG_ESP_CONSOLE_USB_CDC
    primary_vfs = esp_vfs_cdcacm_get_vfs();
    err = esp_usb_console_init();
    if (err != ESP_OK) {
        return err;
    }
    err = esp_vfs_dev_cdcacm_register();
    if (err != ESP_OK) {
        return err;
    }
#else
    primary_vfs = esp_vfs_null_get_vfs();
#endif

// Secondary vfs part.
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    secondary_vfs = esp_vfs_usb_serial_jtag_get_vfs();
#endif
    err = esp_vfs_dev_console_register();
    return err;
}

ESP_SYSTEM_INIT_FN(init_vfs_console, CORE, BIT(0), 114)
{
    return esp_vfs_console_register();
}

#endif // CONFIG_VFS_SUPPORT_IO

void esp_vfs_include_console_register(void)
{
    // Linker hook function, exists to make the linker examine this file
}
