// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_vfs.h"
#include "esp_attr.h"
#include "sys/errno.h"
#include "sys/lock.h"
#include "soc/uart_struct.h"
#include "sdkconfig.h"

static uart_dev_t* s_uarts[3] = {&UART0, &UART1, &UART2};
static _lock_t s_uart_locks[3]; // per-UART locks, lazily initialized

static int IRAM_ATTR uart_open(const char * path, int flags, int mode)
{
    // this is fairly primitive, we should check if file is opened read only,
    // and error out if write is requested
    if (strcmp(path, "/0") == 0) {
        return 0;
    } else if (strcmp(path, "/1") == 0) {
        return 1;
    } else if (strcmp(path, "/2") == 0) {
        return 2;
    }
    errno = ENOENT;
    return -1;
}

static void IRAM_ATTR uart_tx_char(uart_dev_t* uart, int c)
{
    while (uart->status.txfifo_cnt >= 127) {
        ;
    }
    uart->fifo.rw_byte = c;
}


static ssize_t IRAM_ATTR uart_write(int fd, const void * data, size_t size)
{
    assert(fd >=0 && fd < 3);
    const char *data_c = (const char *)data;
    uart_dev_t* uart = s_uarts[fd];
    /*
     *  Even though newlib does stream locking on each individual stream, we need
     *  a dedicated UART lock if two streams (stdout and stderr) point to the
     *  same UART.
     */
    _lock_acquire_recursive(&s_uart_locks[fd]);
    for (size_t i = 0; i < size; i++) {
#if CONFIG_NEWLIB_STDOUT_ADDCR
        if (data_c[i]=='\n') {
            uart_tx_char(uart, '\r');
        }
#endif
        uart_tx_char(uart, data_c[i]);
    }
    _lock_release_recursive(&s_uart_locks[fd]);
    return size;
}

static ssize_t IRAM_ATTR uart_read(int fd, void* data, size_t size)
{
    assert(fd >=0 && fd < 3);
    uint8_t *data_c = (uint8_t *) data;
    uart_dev_t* uart = s_uarts[fd];
    size_t received = 0;
    _lock_acquire_recursive(&s_uart_locks[fd]);
    while (uart->status.rxfifo_cnt > 0 && received < size) {
        uint8_t c = uart->fifo.rw_byte;
#if CONFIG_NEWLIB_STDOUT_ADDCR
        /* Convert \r\n sequences to \n.
         * If \r is received, it is put into 'buffered_char' until the next
         * character is received. Then depending on the character, we either
         * drop \r (if the next one is \n) or output \r and then proceed to output
         * the new character.
         */
        const int NONE = -1;
        static int buffered_char = NONE;
        if (buffered_char != NONE) {
            if (buffered_char == '\r' && c == '\n') {
                buffered_char = NONE;
            } else {
                data_c[received] = buffered_char;
                buffered_char = NONE;
                ++received;
                if (received == size) {
                    /* We have placed the buffered character into the output buffer
                     * but there won't be enough space for the newly received one.
                     * Keep the new character in buffered_char until read is called
                     * again.
                     */
                    buffered_char = c;
                    break;
                }
            }
        }
        if (c == '\r') {
            buffered_char = c;
            continue;
        }
#endif //CONFIG_NEWLIB_STDOUT_ADDCR
        data_c[received] = c;
        ++received;
    }
    _lock_release_recursive(&s_uart_locks[fd]);
    if (received > 0) {
        return received;
    }
    errno = EWOULDBLOCK;
    return -1;
}

static int IRAM_ATTR uart_fstat(int fd, struct stat * st)
{
    assert(fd >=0 && fd < 3);
    st->st_mode = S_IFCHR;
    return 0;
}

static int IRAM_ATTR uart_close(int fd)
{
    assert(fd >=0 && fd < 3);
    return 0;
}

void esp_vfs_dev_uart_register()
{
    esp_vfs_t vfs = {
        .fd_offset = 0,
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &uart_write,
        .open = &uart_open,
        .fstat = &uart_fstat,
        .close = &uart_close,
        .read = &uart_read,
        .lseek = NULL,
        .stat = NULL,
        .link = NULL,
        .unlink = NULL,
        .rename = NULL
    };
    ESP_ERROR_CHECK(esp_vfs_register("/dev/uart", &vfs, NULL));
}
