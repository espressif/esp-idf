/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp_heap_caps.h"

#include "esp_private/esp_vfs_cdcacm_select.h"
#include "esp_private/usb_console.h"

#define USB_CDC_LOCAL_FD 0

// Newline conversion mode when transmitting
static esp_line_endings_t s_tx_mode =
#if CONFIG_LIBC_STDOUT_LINE_ENDING_CRLF
    ESP_LINE_ENDINGS_CRLF;
#elif CONFIG_LIBC_STDOUT_LINE_ENDING_CR
    ESP_LINE_ENDINGS_CR;
#else
    ESP_LINE_ENDINGS_LF;
#endif

// Newline conversion mode when receiving
static esp_line_endings_t s_rx_mode =
#if CONFIG_LIBC_STDIN_LINE_ENDING_CRLF
    ESP_LINE_ENDINGS_CRLF;
#elif CONFIG_LIBC_STDIN_LINE_ENDING_CR
    ESP_LINE_ENDINGS_CR;
#else
    ESP_LINE_ENDINGS_LF;
#endif

#if CONFIG_VFS_SELECT_IN_RAM
#define CDCACM_VFS_MALLOC_FLAGS (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define CDCACM_VFS_MALLOC_FLAGS MALLOC_CAP_DEFAULT
#endif

#define NONE -1

//Read and write lock, lazily initialized
static _lock_t s_write_lock;
static _lock_t s_read_lock;

static bool s_blocking;
static SemaphoreHandle_t s_rx_semaphore;
static SemaphoreHandle_t s_tx_semaphore;

#ifdef CONFIG_VFS_SUPPORT_SELECT

typedef struct {
    esp_vfs_select_sem_t select_sem;
    fd_set *readfds;
    fd_set *writefds;
    fd_set *errorfds;
    fd_set readfds_orig;
    fd_set writefds_orig;
    fd_set errorfds_orig;
} cdcacm_select_args_t;

static cdcacm_select_args_t **s_registered_selects = NULL;
static int s_registered_select_num = 0;
static portMUX_TYPE s_registered_select_lock = portMUX_INITIALIZER_UNLOCKED;

static esp_err_t cdcacm_end_select(void *end_select_args);

#endif // CONFIG_VFS_SUPPORT_SELECT

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
    return USB_CDC_LOCAL_FD; // fd 0
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

    if (s_blocking) {
        while (cdcacm_data_length_in_buffer() < size) {
            xSemaphoreTake(s_rx_semaphore, portMAX_DELAY);
        }
    } else {
        /* process pending interrupts before requesting available data */
        esp_usb_console_poll_interrupts();
        size = MIN(size, cdcacm_data_length_in_buffer());
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

#ifdef CONFIG_VFS_SUPPORT_SELECT

static void select_notif_callback_isr(cdcacm_select_notif_t cdcacm_select_notif, BaseType_t *task_woken)
{
    portENTER_CRITICAL_ISR(&s_registered_select_lock);
    for (int i = 0; i < s_registered_select_num; ++i) {
        cdcacm_select_args_t *args = s_registered_selects[i];
        if (args) {
            switch (cdcacm_select_notif) {
            case CDCACM_SELECT_READ_NOTIF:
                if (FD_ISSET(USB_CDC_LOCAL_FD, &args->readfds_orig)) {
                    FD_SET(USB_CDC_LOCAL_FD, args->readfds);
                    esp_vfs_select_triggered_isr(args->select_sem, task_woken);
                }
                break;
            case CDCACM_SELECT_WRITE_NOTIF:
                if (FD_ISSET(USB_CDC_LOCAL_FD, &args->writefds_orig)) {
                    FD_SET(USB_CDC_LOCAL_FD, args->writefds);
                    esp_vfs_select_triggered_isr(args->select_sem, task_woken);
                }
                break;
            case CDCACM_SELECT_ERROR_NOTIF:
                if (FD_ISSET(USB_CDC_LOCAL_FD, &args->errorfds_orig)) {
                    FD_SET(USB_CDC_LOCAL_FD, args->errorfds);
                    esp_vfs_select_triggered_isr(args->select_sem, task_woken);
                }
                break;
            }
        }
    }
    portEXIT_CRITICAL_ISR(&s_registered_select_lock);
}

static esp_err_t register_select(cdcacm_select_args_t *args)
{
    esp_err_t ret = ESP_ERR_INVALID_ARG;

    if (args) {
        portENTER_CRITICAL(&s_registered_select_lock);
        const int new_size = s_registered_select_num + 1;
        cdcacm_select_args_t **new_selects;
        if ((new_selects = heap_caps_realloc(s_registered_selects, new_size * sizeof(cdcacm_select_args_t *), CDCACM_VFS_MALLOC_FLAGS)) == NULL) {
            ret = ESP_ERR_NO_MEM;
        } else {
            /* on first select registration register the callback  */
            if (s_registered_select_num == 0) {
                cdcacm_set_select_notif_callback(select_notif_callback_isr);
            }

            s_registered_selects = new_selects;
            s_registered_selects[s_registered_select_num] = args;
            s_registered_select_num = new_size;
            ret = ESP_OK;
        }
        portEXIT_CRITICAL(&s_registered_select_lock);
    }

    return ret;
}

static esp_err_t unregister_select(cdcacm_select_args_t *args)
{
    esp_err_t ret = ESP_OK;
    if (args) {
        ret = ESP_ERR_INVALID_STATE;
        portENTER_CRITICAL(&s_registered_select_lock);
        for (int i = 0; i < s_registered_select_num; ++i) {
            if (s_registered_selects[i] == args) {
                const int new_size = s_registered_select_num - 1;
                // The item is removed by overwriting it with the last item. The subsequent rellocation will drop the
                // last item.
                s_registered_selects[i] = s_registered_selects[new_size];
                s_registered_selects = heap_caps_realloc(s_registered_selects, new_size * sizeof(cdcacm_select_args_t *), CDCACM_VFS_MALLOC_FLAGS);
                // Shrinking a buffer with realloc is guaranteed to succeed.
                s_registered_select_num = new_size;

                /* when the last select is unregistered, also unregister the callback  */
                if (s_registered_select_num == 0) {
                    cdcacm_set_select_notif_callback(NULL);
                }

                ret = ESP_OK;
                break;
            }
        }
        portEXIT_CRITICAL(&s_registered_select_lock);
    }
    return ret;
}

static esp_err_t cdcacm_start_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                                     esp_vfs_select_sem_t select_sem, void **end_select_args)
{
    (void)nfds; /* Since there is only 1 USB OTG, this parameter is useless */
    *end_select_args = NULL;

    if (!esp_usb_console_is_installed()) {
        return ESP_ERR_INVALID_STATE;
    }

    cdcacm_select_args_t *args = heap_caps_malloc(sizeof(cdcacm_select_args_t), CDCACM_VFS_MALLOC_FLAGS);

    if (args == NULL) {
        return ESP_ERR_NO_MEM;
    }

    args->select_sem = select_sem;
    args->readfds = readfds;
    args->writefds = writefds;
    args->errorfds = exceptfds;
    args->readfds_orig = *readfds; // store the original values because they will be set to zero
    args->writefds_orig = *writefds;
    args->errorfds_orig = *exceptfds;
    FD_ZERO(readfds);
    FD_ZERO(writefds);
    FD_ZERO(exceptfds);

    esp_err_t ret = register_select(args);
    if (ret != ESP_OK) {
        free(args);
        return ret;
    }

    /* make sure the driver processes any pending interrupts before checking read or write
     * readiness */
    esp_usb_console_poll_interrupts();
    bool trigger_select = false;
    if (FD_ISSET(USB_CDC_LOCAL_FD, &args->readfds_orig) &&
            esp_usb_console_available_for_read() > 0) {

        // signalize immediately when read is ready
        FD_SET(USB_CDC_LOCAL_FD, readfds);
        trigger_select = true;
    }

    if (FD_ISSET(USB_CDC_LOCAL_FD, &args->writefds_orig) &&
            esp_usb_console_write_available()) {

        // signalize immediately when write is ready
        FD_SET(USB_CDC_LOCAL_FD, writefds);
        trigger_select = true;
    }

    if (trigger_select) {
        esp_vfs_select_triggered(args->select_sem);
    }

    *end_select_args = args;
    return ESP_OK;
}

static esp_err_t cdcacm_end_select(void *end_select_args)
{
    cdcacm_select_args_t *args = end_select_args;
    esp_err_t ret = unregister_select(args);
    if (args) {
        free(args);
    }

    return ret;
}

#endif // CONFIG_VFS_SUPPORT_SELECT

void esp_vfs_dev_cdcacm_set_tx_line_endings(esp_line_endings_t mode)
{
    s_tx_mode = mode;
}

void esp_vfs_dev_cdcacm_set_rx_line_endings(esp_line_endings_t mode)
{
    s_rx_mode = mode;
}

#ifdef CONFIG_VFS_SUPPORT_SELECT
static const esp_vfs_select_ops_t s_cdcacm_vfs_select = {
    .start_select = &cdcacm_start_select,
    .end_select = &cdcacm_end_select,
};
#endif // CONFIG_VFS_SUPPORT_SELECT

static const esp_vfs_fs_ops_t s_cdcacm_vfs = {
    .write = &cdcacm_write,
    .open = &cdcacm_open,
    .fstat = &cdcacm_fstat,
    .close = &cdcacm_close,
    .read = &cdcacm_read,
    .fcntl = &cdcacm_fcntl,
    .fsync = &cdcacm_fsync,
#ifdef CONFIG_VFS_SUPPORT_SELECT
    .select = &s_cdcacm_vfs_select,
#endif // CONFIG_VFS_SUPPORT_SELECT
};

const esp_vfs_fs_ops_t *esp_vfs_cdcacm_get_vfs(void)
{
    return &s_cdcacm_vfs;
}

esp_err_t esp_vfs_dev_cdcacm_register(void)
{
    return esp_vfs_register_fs("/dev/cdcacm", &s_cdcacm_vfs, ESP_VFS_FLAG_STATIC, NULL);
}
