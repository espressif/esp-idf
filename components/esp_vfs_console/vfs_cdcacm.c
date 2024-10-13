/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/errno.h>
#include <sys/lock.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include "esp_vfs.h"
#include "esp_vfs_cdcacm.h"
#include "esp_attr.h"
#include "sdkconfig.h"

#include "esp_private/usb_console.h"

// Newline conversion mode when transmitting
static esp_line_endings_t s_tx_mode =
#if CONFIG_NEWLIB_STDOUT_LINE_ENDING_CRLF
    ESP_LINE_ENDINGS_CRLF;
#elif CONFIG_NEWLIB_STDOUT_LINE_ENDING_CR
    ESP_LINE_ENDINGS_CR;
#else
    ESP_LINE_ENDINGS_LF;
#endif

// Newline conversion mode when receiving
static esp_line_endings_t s_rx_mode =
#if CONFIG_NEWLIB_STDIN_LINE_ENDING_CRLF
    ESP_LINE_ENDINGS_CRLF;
#elif CONFIG_NEWLIB_STDIN_LINE_ENDING_CR
    ESP_LINE_ENDINGS_CR;
#else
    ESP_LINE_ENDINGS_LF;
#endif

#define NONE -1

//Read and write lock, lazily initialized
static _lock_t s_write_lock;
static _lock_t s_read_lock;

static bool s_blocking;
static SemaphoreHandle_t s_rx_semaphore;
static SemaphoreHandle_t s_tx_semaphore;

static ssize_t cdcacm_write(int fd, const void *data, size_t size)
{
    assert(fd == 0);
    const char *cdata = (const char *)data;
    _lock_acquire_recursive(&s_write_lock);
    for (size_t i = 0; i < size; i++) {
        if (cdata[i] != '\n') {
            esp_usb_console_write_buf(&cdata[i], 1);
        } else {
            if (s_tx_mode == ESP_LINE_ENDINGS_CRLF || s_tx_mode == ESP_LINE_ENDINGS_CR) {
                char cr = '\r';
                esp_usb_console_write_buf(&cr, 1);
            }
            if (s_tx_mode == ESP_LINE_ENDINGS_CRLF || s_tx_mode == ESP_LINE_ENDINGS_LF) {
                char lf = '\n';
                esp_usb_console_write_buf(&lf, 1);
            }
        }
    }
    _lock_release_recursive(&s_write_lock);
    return size;
}

static int cdcacm_fsync(int fd)
{
    assert(fd == 0);
    _lock_acquire_recursive(&s_write_lock);
    ssize_t written = esp_usb_console_flush();
    _lock_release_recursive(&s_write_lock);
    return (written < 0) ? -1 : 0;
}

static int cdcacm_open(const char *path, int flags, int mode)
{
    return 0; // fd 0
}

static int cdcacm_fstat(int fd, struct stat *st)
{
    assert(fd == 0);
    memset(st, 0, sizeof(*st));
    st->st_mode = S_IFCHR;
    return 0;
}

static int cdcacm_close(int fd)
{
    assert(fd == 0);
    return 0;
}

static int s_peek_char = NONE;

/* Helper function which returns a previous character or reads a new one from
 * CDC-ACM driver. Previous character can be returned ("pushed back") using
 * cdcacm_return_char function. Returns NONE if no character is available. Note
 * the cdcacm driver maintains its own RX buffer and a receive call does not
 * invoke an USB operation, so there's no penalty to reading data char-by-char.
 */
static int cdcacm_read_char(void)
{
    /* return character from peek buffer, if it is there */
    if (s_peek_char != NONE) {
        int c = s_peek_char;
        s_peek_char = NONE;
        return c;
    }
    /* Peek buffer is empty; try to read from cdcacm driver. */
    uint8_t c;
    ssize_t read = esp_usb_console_read_buf((char *) &c, 1);
    if (read <= 0) {
        return NONE;
    } else {
        return c;
    }
}

static ssize_t cdcacm_data_length_in_buffer(void)
{
    ssize_t len = esp_usb_console_available_for_read();
    if (len < 0) {
        len = 0;
    }
    if (s_peek_char != NONE) {
        len += 1;
    }
    return len;
}

/* Push back a character; it will be returned by next call to cdcacm_read_char */
static void cdcacm_return_char(int c)
{
    assert(s_peek_char == NONE);
    s_peek_char = c;
}

static ssize_t cdcacm_read(int fd, void *data, size_t size)
{
    assert(fd == 0);
    char *data_c = (char *) data;
    ssize_t received = 0;
    _lock_acquire_recursive(&s_read_lock);

    while (cdcacm_data_length_in_buffer() < size) {
        if (!s_blocking) {
            errno = EWOULDBLOCK;
            _lock_release_recursive(&s_read_lock);
            return -1;
        }
        xSemaphoreTake(s_rx_semaphore, portMAX_DELAY);
    }

    if (s_rx_mode == ESP_LINE_ENDINGS_CR || s_rx_mode == ESP_LINE_ENDINGS_LF) {
        /* This is easy. Just receive, and if needed replace \r by \n. */
        received = esp_usb_console_read_buf(data_c, size);
        if (s_rx_mode == ESP_LINE_ENDINGS_CR) {
            /* Change CRs to newlines */
            for (ssize_t i = 0; i < received; i++) {
                if (data_c[i] == '\r') {
                    data_c[i] = '\n';
                }
            }
        }
    } else {
        while (received < size) {
            int c = cdcacm_read_char();
            if (c == '\r') {
                /* look ahead */
                int c2 = cdcacm_read_char();
                if (c2 == NONE) {
                    /* could not look ahead, put the current character back */
                    cdcacm_return_char(c);
                    break;
                }
                if (c2 == '\n') {
                    /* this was \r\n sequence. discard \r, return \n */
                    c = '\n';
                } else {
                    /* \r followed by something else. put the second char back,
                     * it will be processed on next iteration. return \r now.
                     */
                    cdcacm_return_char(c2);
                }
            } else if (c == NONE) {
                break;
            }
            data_c[received++] = (char) c;
            if (c == '\n') {
                break;
            }
        }
    }
    _lock_release_recursive(&s_read_lock);
    if (received > 0) {
        return received;
    }
    errno = EWOULDBLOCK;
    return -1;
}

/* Non-static, to be able to place into IRAM by ldgen */
void cdcacm_rx_cb(void* arg)
{
    assert(s_blocking);
    xSemaphoreGive(s_rx_semaphore);
}

/* Non-static, to be able to place into IRAM by ldgen */
void cdcacm_tx_cb(void* arg)
{
    assert(s_blocking);
    xSemaphoreGive(s_tx_semaphore);
}

static int cdcacm_enable_blocking(void)
{
    s_rx_semaphore = xSemaphoreCreateBinary();
    if (!s_rx_semaphore) {
        errno = ENOMEM;
        goto fail;
    }
    s_tx_semaphore = xSemaphoreCreateBinary();
    if (!s_tx_semaphore) {
        errno = ENOMEM;
        goto fail;
    }
    esp_err_t err = esp_usb_console_set_cb(&cdcacm_rx_cb, &cdcacm_tx_cb, NULL);
    if (err != ESP_OK) {
        errno = ENODEV;
        goto fail;
    }
    s_blocking = true;
    return 0;

fail:
    if (s_rx_semaphore) {
        vSemaphoreDelete(s_rx_semaphore);
        s_rx_semaphore = NULL;
    }
    if (s_tx_semaphore) {
        vSemaphoreDelete(s_tx_semaphore);
        s_tx_semaphore = NULL;
    }
    return -1;
}

static int cdcacm_disable_blocking(void)
{
    esp_usb_console_set_cb(NULL, NULL, NULL); /* ignore any errors */
    vSemaphoreDelete(s_rx_semaphore);
    s_rx_semaphore = NULL;
    vSemaphoreDelete(s_tx_semaphore);
    s_tx_semaphore = NULL;
    s_blocking = false;
    return 0;
}

static int cdcacm_fcntl(int fd, int cmd, int arg)
{
    assert(fd == 0);
    int result;
    if (cmd == F_GETFL) {
        result = O_RDWR;
        if (!s_blocking) {
            result |= O_NONBLOCK;
        }
    } else if (cmd == F_SETFL) {
        bool blocking = (arg & O_NONBLOCK) == 0;
        result = 0;
        if (blocking && !s_blocking) {
            result = cdcacm_enable_blocking();
        } else if (!blocking && s_blocking) {
            result = cdcacm_disable_blocking();
        }
    } else {
        /* unsupported operation */
        result = -1;
        errno = ENOSYS;
    }
    return result;
}

void esp_vfs_dev_cdcacm_set_tx_line_endings(esp_line_endings_t mode)
{
    s_tx_mode = mode;
}

void esp_vfs_dev_cdcacm_set_rx_line_endings(esp_line_endings_t mode)
{
    s_rx_mode = mode;
}

static const esp_vfs_t vfs = {
    .flags = ESP_VFS_FLAG_DEFAULT,
    .write = &cdcacm_write,
    .open = &cdcacm_open,
    .fstat = &cdcacm_fstat,
    .close = &cdcacm_close,
    .read = &cdcacm_read,
    .fcntl = &cdcacm_fcntl,
    .fsync = &cdcacm_fsync
};

const esp_vfs_t *esp_vfs_cdcacm_get_vfs(void)
{
    return &vfs;
}

esp_err_t esp_vfs_dev_cdcacm_register(void)
{
    return esp_vfs_register("/dev/cdcacm", &vfs, NULL);
}
