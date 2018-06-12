// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/errno.h>
#include <sys/lock.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "esp_attr.h"
#include "soc/uart_struct.h"
#include "driver/uart.h"
#include "sdkconfig.h"
#include "driver/uart_select.h"

// TODO: make the number of UARTs chip dependent
#define UART_NUM 3

// Token signifying that no character is available
#define NONE -1

// UART write bytes function type
typedef void (*tx_func_t)(int, int);
// UART read bytes function type
typedef int (*rx_func_t)(int);

// Basic functions for sending and receiving bytes over UART
static void uart_tx_char(int fd, int c);
static int uart_rx_char(int fd);

// Functions for sending and receiving bytes which use UART driver
static void uart_tx_char_via_driver(int fd, int c);
static int uart_rx_char_via_driver(int fd);

// Pointers to UART peripherals
static uart_dev_t* s_uarts[UART_NUM] = {&UART0, &UART1, &UART2};
// per-UART locks, lazily initialized
static _lock_t s_uart_read_locks[UART_NUM];
static _lock_t s_uart_write_locks[UART_NUM];
// One-character buffer used for newline conversion code, per UART
static int s_peek_char[UART_NUM] = { NONE, NONE, NONE };
// Per-UART non-blocking flag. Note: default implementation does not honor this
// flag, all reads are non-blocking. This option becomes effective if UART
// driver is used.
static bool s_non_blocking[UART_NUM];

/* Lock ensuring that uart_select is used from only one task at the time */
static _lock_t s_one_select_lock;

static SemaphoreHandle_t *_signal_sem = NULL;
static fd_set *_readfds = NULL;
static fd_set *_writefds = NULL;
static fd_set *_errorfds = NULL;
static fd_set *_readfds_orig = NULL;
static fd_set *_writefds_orig = NULL;
static fd_set *_errorfds_orig = NULL;

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

static void uart_end_select();

// Functions used to write bytes to UART. Default to "basic" functions.
static tx_func_t s_uart_tx_func[UART_NUM] = {
        &uart_tx_char, &uart_tx_char, &uart_tx_char
};

// Functions used to read bytes from UART. Default to "basic" functions.
static rx_func_t s_uart_rx_func[UART_NUM] = {
        &uart_rx_char, &uart_rx_char, &uart_rx_char
};


static int uart_open(const char * path, int flags, int mode)
{
    // this is fairly primitive, we should check if file is opened read only,
    // and error out if write is requested
    int fd = -1;

    if (strcmp(path, "/0") == 0) {
        fd = 0;
    } else if (strcmp(path, "/1") == 0) {
        fd = 1;
    } else if (strcmp(path, "/2") == 0) {
        fd = 2;
    } else {
        errno = ENOENT;
        return fd;
    }

    s_non_blocking[fd] = ((flags & O_NONBLOCK) == O_NONBLOCK);

    return fd;
}

static void uart_tx_char(int fd, int c)
{
    uart_dev_t* uart = s_uarts[fd];
    while (uart->status.txfifo_cnt >= 127) {
        ;
    }
    uart->fifo.rw_byte = c;
}

static void uart_tx_char_via_driver(int fd, int c)
{
    char ch = (char) c;
    uart_write_bytes(fd, &ch, 1);
}

static int uart_rx_char(int fd)
{
    uart_dev_t* uart = s_uarts[fd];
    if (uart->status.rxfifo_cnt == 0) {
        return NONE;
    }
    return uart->fifo.rw_byte;
}

static int uart_rx_char_via_driver(int fd)
{
    uint8_t c;
    int timeout = s_non_blocking[fd] ? 0 : portMAX_DELAY;
    int n = uart_read_bytes(fd, &c, 1, timeout);
    if (n <= 0) {
        return NONE;
    }
    return c;
}

static ssize_t uart_write(int fd, const void * data, size_t size)
{
    assert(fd >=0 && fd < 3);
    const char *data_c = (const char *)data;
    /*  Even though newlib does stream locking on each individual stream, we need
     *  a dedicated UART lock if two streams (stdout and stderr) point to the
     *  same UART.
     */
    _lock_acquire_recursive(&s_uart_write_locks[fd]);
    for (size_t i = 0; i < size; i++) {
        int c = data_c[i];
        if (c == '\n' && s_tx_mode != ESP_LINE_ENDINGS_LF) {
            s_uart_tx_func[fd](fd, '\r');
            if (s_tx_mode == ESP_LINE_ENDINGS_CR) {
                continue;
            }
        }
        s_uart_tx_func[fd](fd, c);
    }
    _lock_release_recursive(&s_uart_write_locks[fd]);
    return size;
}

/* Helper function which returns a previous character or reads a new one from
 * UART. Previous character can be returned ("pushed back") using
 * uart_return_char function.
 */
static int uart_read_char(int fd)
{
    /* return character from peek buffer, if it is there */
    if (s_peek_char[fd] != NONE) {
        int c = s_peek_char[fd];
        s_peek_char[fd] = NONE;
        return c;
    }
    return s_uart_rx_func[fd](fd);
}

/* Push back a character; it will be returned by next call to uart_read_char */
static void uart_return_char(int fd, int c)
{
    assert(s_peek_char[fd] == NONE);
    s_peek_char[fd] = c;
}

static ssize_t uart_read(int fd, void* data, size_t size)
{
    assert(fd >=0 && fd < 3);
    char *data_c = (char *) data;
    size_t received = 0;
    _lock_acquire_recursive(&s_uart_read_locks[fd]);
    while (received < size) {
        int c = uart_read_char(fd);
        if (c == '\r') {
            if (s_rx_mode == ESP_LINE_ENDINGS_CR) {
                c = '\n';
            } else if (s_rx_mode == ESP_LINE_ENDINGS_CRLF) {
                /* look ahead */
                int c2 = uart_read_char(fd);
                if (c2 == NONE) {
                    /* could not look ahead, put the current character back */
                    uart_return_char(fd, c);
                    break;
                }
                if (c2 == '\n') {
                    /* this was \r\n sequence. discard \r, return \n */
                    c = '\n';
                } else {
                    /* \r followed by something else. put the second char back,
                     * it will be processed on next iteration. return \r now.
                     */
                    uart_return_char(fd, c2);
                }
            }
        } else if (c == NONE) {
            break;
        }
        data_c[received] = (char) c;
        ++received;
        if (c == '\n') {
            break;
        }
    }
    _lock_release_recursive(&s_uart_read_locks[fd]);
    if (received > 0) {
        return received;
    }
    errno = EWOULDBLOCK;
    return -1;
}

static int uart_fstat(int fd, struct stat * st)
{
    assert(fd >=0 && fd < 3);
    st->st_mode = S_IFCHR;
    return 0;
}

static int uart_close(int fd)
{
    assert(fd >=0 && fd < 3);
    return 0;
}

static int uart_fcntl(int fd, int cmd, va_list args)
{
    assert(fd >=0 && fd < 3);
    int result = 0;
    if (cmd == F_GETFL) {
        if (s_non_blocking[fd]) {
            result |= O_NONBLOCK;
        }
    } else if (cmd == F_SETFL) {
        int arg = va_arg(args, int);
        s_non_blocking[fd] = (arg & O_NONBLOCK) != 0;
    } else {
        // unsupported operation
        result = -1;
        errno = ENOSYS;
    }
    return result;
}

static int uart_access(const char *path, int amode)
{
    int ret = -1;

    if (strcmp(path, "/0") == 0 || strcmp(path, "/1") == 0 || strcmp(path, "/2") == 0) {
        if (F_OK == amode) {
            ret = 0; //path exists
        } else {
            if ((((amode & R_OK) == R_OK) || ((amode & W_OK) == W_OK)) && ((amode & X_OK) != X_OK)) {
                ret = 0; //path is readable and/or writable but not executable
            } else {
                errno = EACCES;
            }
        }
    } else {
        errno = ENOENT;
    }

    return ret;
}

static void select_notif_callback(uart_port_t uart_num, uart_select_notif_t uart_select_notif, BaseType_t *task_woken)
{
    switch (uart_select_notif) {
        case UART_SELECT_READ_NOTIF:
            if (FD_ISSET(uart_num, _readfds_orig)) {
                FD_SET(uart_num, _readfds);
                esp_vfs_select_triggered_isr(_signal_sem, task_woken);
            }
            break;
        case UART_SELECT_WRITE_NOTIF:
            if (FD_ISSET(uart_num, _writefds_orig)) {
                FD_SET(uart_num, _writefds);
                esp_vfs_select_triggered_isr(_signal_sem, task_woken);
            }
            break;
        case UART_SELECT_ERROR_NOTIF:
            if (FD_ISSET(uart_num, _errorfds_orig)) {
                FD_SET(uart_num, _errorfds);
                esp_vfs_select_triggered_isr(_signal_sem, task_woken);
            }
            break;
    }
}

static esp_err_t uart_start_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, SemaphoreHandle_t *signal_sem)
{
    if (_lock_try_acquire(&s_one_select_lock)) {
        return ESP_ERR_INVALID_STATE;
    }

    const int max_fds = MIN(nfds, UART_NUM);

    portENTER_CRITICAL(uart_get_selectlock());

    if (_readfds || _writefds || _errorfds || _readfds_orig || _writefds_orig || _errorfds_orig || _signal_sem) {
        portEXIT_CRITICAL(uart_get_selectlock());
        uart_end_select();
        return ESP_ERR_INVALID_STATE;
    }

    if ((_readfds_orig = malloc(sizeof(fd_set))) == NULL) {
        portEXIT_CRITICAL(uart_get_selectlock());
        uart_end_select();
        return ESP_ERR_NO_MEM;
    }

    if ((_writefds_orig = malloc(sizeof(fd_set))) == NULL) {
        portEXIT_CRITICAL(uart_get_selectlock());
        uart_end_select();
        return ESP_ERR_NO_MEM;
    }

    if ((_errorfds_orig = malloc(sizeof(fd_set))) == NULL) {
        portEXIT_CRITICAL(uart_get_selectlock());
        uart_end_select();
        return ESP_ERR_NO_MEM;
    }

    //uart_set_select_notif_callback set the callbacks in UART ISR
    for (int i = 0; i < max_fds; ++i) {
        if (FD_ISSET(i, readfds) || FD_ISSET(i, writefds) || FD_ISSET(i, exceptfds)) {
            uart_set_select_notif_callback(i, select_notif_callback);
        }
    }

    _signal_sem = signal_sem;

    _readfds = readfds;
    _writefds = writefds;
    _errorfds = exceptfds;

    *_readfds_orig = *readfds;
    *_writefds_orig = *writefds;
    *_errorfds_orig = *exceptfds;

    FD_ZERO(readfds);
    FD_ZERO(writefds);
    FD_ZERO(exceptfds);

    portEXIT_CRITICAL(uart_get_selectlock());
    // s_one_select_lock is not released on successfull exit - will be
    // released in uart_end_select()

    return ESP_OK;
}

static void uart_end_select()
{
    portENTER_CRITICAL(uart_get_selectlock());
    for (int i = 0; i < UART_NUM; ++i) {
        uart_set_select_notif_callback(i, NULL);
    }

    _signal_sem = NULL;

    _readfds = NULL;
    _writefds = NULL;
    _errorfds = NULL;

    if (_readfds_orig) {
        free(_readfds_orig);
        _readfds_orig = NULL;
    }

    if (_writefds_orig) {
        free(_writefds_orig);
        _writefds_orig = NULL;
    }

    if (_errorfds_orig) {
        free(_errorfds_orig);
        _errorfds_orig = NULL;
    }
    portEXIT_CRITICAL(uart_get_selectlock());
    _lock_release(&s_one_select_lock);
}

void esp_vfs_dev_uart_register()
{
    esp_vfs_t vfs = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &uart_write,
        .open = &uart_open,
        .fstat = &uart_fstat,
        .close = &uart_close,
        .read = &uart_read,
        .fcntl = &uart_fcntl,
        .access = &uart_access,
        .start_select = &uart_start_select,
        .end_select = &uart_end_select,
    };
    ESP_ERROR_CHECK(esp_vfs_register("/dev/uart", &vfs, NULL));
}

void esp_vfs_dev_uart_set_rx_line_endings(esp_line_endings_t mode)
{
    s_rx_mode = mode;
}

void esp_vfs_dev_uart_set_tx_line_endings(esp_line_endings_t mode)
{
    s_tx_mode = mode;
}

void esp_vfs_dev_uart_use_nonblocking(int uart_num)
{
    _lock_acquire_recursive(&s_uart_read_locks[uart_num]);
    _lock_acquire_recursive(&s_uart_write_locks[uart_num]);
    s_uart_tx_func[uart_num] = uart_tx_char;
    s_uart_rx_func[uart_num] = uart_rx_char;
    _lock_release_recursive(&s_uart_write_locks[uart_num]);
    _lock_release_recursive(&s_uart_read_locks[uart_num]);
}

void esp_vfs_dev_uart_use_driver(int uart_num)
{
    _lock_acquire_recursive(&s_uart_read_locks[uart_num]);
    _lock_acquire_recursive(&s_uart_write_locks[uart_num]);
    s_uart_tx_func[uart_num] = uart_tx_char_via_driver;
    s_uart_rx_func[uart_num] = uart_rx_char_via_driver;
    _lock_release_recursive(&s_uart_write_locks[uart_num]);
    _lock_release_recursive(&s_uart_read_locks[uart_num]);
}
