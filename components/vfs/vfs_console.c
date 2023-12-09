/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_vfs_console.h"
#include "esp_rom_sys.h"
#include "esp_vfs_cdcacm.h"
#include "esp_vfs_private.h"
#include "esp_vfs_usb_serial_jtag.h"
#include "esp_vfs_dev.h"
#include "esp_private/usb_console.h"
#include "sdkconfig.h"

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

/**
 * This file is to concentrate all the vfs(UART, USB_SERIAL_JTAG, CDCACM) console into one single file.
 * Get the vfs information from their component (i.e. vfs_uart.c) through `esp_vfs_usb_xxx_get_console()`,
 * which can help us to output some string to two different ports(i.e both through uart and usb_serial_jtag).
 * Usually, we set a port as primary and another as secondary. For primary, it is used for all the features supported by each vfs implementation,
 * while the secondary is only used for output.
 */

typedef struct {
    int fd_primary;
    int fd_secondary;
} vfs_console_context_t;

#if CONFIG_VFS_SUPPORT_IO
// Primary register part.
#ifdef CONFIG_ESP_CONSOLE_UART
const static char *primary_path = "/dev/uart";
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
const static char *primary_path = "/dev/usbserjtag";
#elif CONFIG_ESP_CONSOLE_USB_CDC
const static char *primary_path = "/dev/cdcacm";
#endif

// Secondary register part.
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
const static char *secondary_path = "/dev/secondary";
static int secondary_vfs_index;
#endif // Secondary part

static int primary_vfs_index;

static vfs_console_context_t vfs_console= {0};

int console_open(const char * path, int flags, int mode)
{
// Primary port open
#if CONFIG_ESP_CONSOLE_UART
    vfs_console.fd_primary = get_vfs_for_path(primary_path)->vfs.open("/"STRINGIFY(CONFIG_ESP_CONSOLE_UART_NUM), flags, mode);
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    vfs_console.fd_primary = esp_vfs_usb_serial_jtag_get_vfs()->open("/", flags, mode);
#elif CONFIG_ESP_CONSOLE_USB_CDC
    vfs_console.fd_primary = esp_vfs_cdcacm_get_vfs()->open("/", flags, mode);
#endif

// Secondary port open
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    vfs_console.fd_secondary = get_vfs_for_path(secondary_path)->vfs.open("/", flags, mode);
#endif
    return 0;
}

ssize_t console_write(int fd, const void *data, size_t size)
{
    // All function calls are to primary, except from write and close, which will be forwarded to both primary and secondary.
    get_vfs_for_index(primary_vfs_index)->vfs.write(vfs_console.fd_primary, data, size);
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    get_vfs_for_index(secondary_vfs_index)->vfs.write(vfs_console.fd_secondary, data, size);
#endif
    return size;
}

int console_fstat(int fd, struct stat * st)
{
    return get_vfs_for_index(primary_vfs_index)->vfs.fstat(fd, st);
}

int console_close(int fd)
{
    // All function calls are to primary, except from write and close, which will be forwarded to both primary and secondary.
    get_vfs_for_index(primary_vfs_index)->vfs.close(vfs_console.fd_primary);
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    get_vfs_for_index(secondary_vfs_index)->vfs.close(vfs_console.fd_secondary);
#endif
    return 0;
}

ssize_t console_read(int fd, void * dst, size_t size)
{
    return get_vfs_for_index(primary_vfs_index)->vfs.read(vfs_console.fd_primary, dst, size);
}

int console_fcntl(int fd, int cmd, int arg)
{
    return get_vfs_for_index(primary_vfs_index)->vfs.fcntl(vfs_console.fd_primary, cmd, arg);
}

int console_fsync(int fd)
{
    return get_vfs_for_index(primary_vfs_index)->vfs.fsync(vfs_console.fd_primary);
}

#ifdef CONFIG_VFS_SUPPORT_DIR
int console_access(const char *path, int amode)
{
    // currently only UART support DIR.
    return get_vfs_for_index(primary_vfs_index)->vfs.access("/"STRINGIFY(CONFIG_ESP_CONSOLE_UART_NUM), amode);
}
#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_SELECT
static esp_err_t console_start_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
        esp_vfs_select_sem_t select_sem, void **end_select_args)
{
    const vfs_entry_t* vfs_entry = get_vfs_for_index(primary_vfs_index);
    // start_select is not guaranteed be implemented even though CONFIG_VFS_SUPPORT_SELECT is enabled in sdkconfig
    if (vfs_entry && vfs_entry->vfs.start_select) {
        return vfs_entry->vfs.start_select(nfds, readfds, writefds, exceptfds, select_sem, end_select_args);
    }
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t console_end_select(void *end_select_args)
{
    const vfs_entry_t* vfs_entry = get_vfs_for_index(primary_vfs_index);
    // end_select is not guaranteed be implemented even though CONFIG_VFS_SUPPORT_SELECT is enabled in sdkconfig
    if (vfs_entry && vfs_entry->vfs.end_select) {
        return vfs_entry->vfs.end_select(end_select_args);
    }
    return ESP_ERR_NOT_SUPPORTED;
}

#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_TERMIOS

int console_tcsetattr(int fd, int optional_actions, const struct termios *p)
{
    return get_vfs_for_index(primary_vfs_index)->vfs.tcsetattr(vfs_console.fd_primary, optional_actions, p);
}

int console_tcgetattr(int fd, struct termios *p)
{
    return get_vfs_for_index(primary_vfs_index)->vfs.tcgetattr(vfs_console.fd_primary, p);
}

int console_tcdrain(int fd)
{
    return get_vfs_for_index(primary_vfs_index)->vfs.tcdrain(vfs_console.fd_primary);
}

int console_tcflush(int fd, int select)
{
    return get_vfs_for_index(primary_vfs_index)->vfs.tcflush(vfs_console.fd_primary, select);
}
#endif // CONFIG_VFS_SUPPORT_TERMIOS

static const esp_vfs_t vfs = {
    .flags = ESP_VFS_FLAG_DEFAULT,
    .write = &console_write,
    .open = &console_open,
    .fstat = &console_fstat,
    .close = &console_close,
    .read = &console_read,
    .fcntl = &console_fcntl,
    .fsync = &console_fsync,
#ifdef CONFIG_VFS_SUPPORT_DIR
    .access = &console_access,
#endif // CONFIG_VFS_SUPPORT_DIR
#ifdef CONFIG_VFS_SUPPORT_SELECT
    .start_select = &console_start_select,
    .end_select = &console_end_select,
#endif // CONFIG_VFS_SUPPORT_SELECT
#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    .tcsetattr = &console_tcsetattr,
    .tcgetattr = &console_tcgetattr,
    .tcdrain = &console_tcdrain,
    .tcflush = &console_tcflush,
#endif // CONFIG_VFS_SUPPORT_TERMIOS
};

esp_err_t esp_vfs_dev_console_register(void)
{
    return esp_vfs_register(ESP_VFS_DEV_CONSOLE, &vfs, NULL);
}

esp_err_t esp_vfs_console_register(void)
{
    esp_err_t err = ESP_OK;
// Primary register part.
#ifdef CONFIG_ESP_CONSOLE_UART
    const esp_vfs_t *uart_vfs = esp_vfs_uart_get_vfs();
    err = esp_vfs_register_common(primary_path, strlen(primary_path), uart_vfs, NULL, &primary_vfs_index);
#elif CONFIG_ESP_CONSOLE_USB_CDC
    const esp_vfs_t *cdcacm_vfs = esp_vfs_cdcacm_get_vfs();
    err = esp_usb_console_init();
    if (err != ESP_OK) {
        return err;
    }
    err = esp_vfs_register_common(primary_path, strlen(primary_path), cdcacm_vfs, NULL, &primary_vfs_index);
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    const esp_vfs_t *usb_serial_jtag_vfs = esp_vfs_usb_serial_jtag_get_vfs();
    err = esp_vfs_register_common(primary_path, strlen(primary_path), usb_serial_jtag_vfs, NULL, &primary_vfs_index);
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    if (err != ESP_OK) {
        return err;
    }

// Secondary register part.
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    const esp_vfs_t *usb_serial_jtag_vfs = esp_vfs_usb_serial_jtag_get_vfs();
    err = esp_vfs_register_common(secondary_path, strlen(secondary_path), usb_serial_jtag_vfs, NULL, &secondary_vfs_index);
    if(err != ESP_OK) {
        return err;
    }
#endif
    err = esp_vfs_dev_console_register();
    return err;
}

#endif // CONFIG_VFS_SUPPORT_IO
