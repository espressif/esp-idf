// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
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

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "vfs_tinyusb.h"
#include "sdkconfig.h"

const static char *TAG = "tusb_vfs";
#define VFS_TUSB_MAX_PATH 16
#define VFS_TUSB_PATH_DEFAULT "/dev/tusb_cdc"

// Token signifying that no character is available
#define NONE -1

#define FD_CHECK(fd, ret_val) do {                      \
                                    if ((fd) != 0) {    \
                                    errno = EBADF;      \
                                    return (ret_val);   \
                                    }                   \
                                } while (0)



#if CONFIG_NEWLIB_STDOUT_LINE_ENDING_CRLF
#   define DEFAULT_TX_MODE ESP_LINE_ENDINGS_CRLF
#elif CONFIG_NEWLIB_STDOUT_LINE_ENDING_CR
#   define DEFAULT_TX_MODE ESP_LINE_ENDINGS_CR
#else
#   define DEFAULT_TX_MODE ESP_LINE_ENDINGS_LF
#endif

#if CONFIG_NEWLIB_STDIN_LINE_ENDING_CRLF
#   define DEFAULT_RX_MODE ESP_LINE_ENDINGS_CRLF
#elif CONFIG_NEWLIB_STDIN_LINE_ENDING_CR
#   define DEFAULT_RX_MODE ESP_LINE_ENDINGS_CR
#else
#   define DEFAULT_RX_MODE ESP_LINE_ENDINGS_LF
#endif

typedef struct {
    _lock_t write_lock;
    _lock_t read_lock;
    esp_line_endings_t tx_mode; // Newline conversion mode when transmitting
    esp_line_endings_t rx_mode; // Newline conversion mode when receiving
    uint32_t flags;
    char vfs_path[VFS_TUSB_MAX_PATH];
    int cdc_intf;
} vfs_tinyusb_t;

static vfs_tinyusb_t s_vfstusb;


static esp_err_t apply_path(char const *path)
{
    if (path != NULL) {
        size_t path_len = strlen(path) + 1;
        if (path_len > VFS_TUSB_MAX_PATH) {
            ESP_LOGE(TAG, "The path is too long; maximum is %d characters", VFS_TUSB_MAX_PATH);
            return ESP_ERR_INVALID_ARG;
        }
        strncpy(s_vfstusb.vfs_path, path, (VFS_TUSB_MAX_PATH - 1));
    } else {
        strncpy(s_vfstusb.vfs_path,
                VFS_TUSB_PATH_DEFAULT,
                (VFS_TUSB_MAX_PATH - 1));
    }
    ESP_LOGV(TAG, "Path is set to `%s`", s_vfstusb.vfs_path);
    return ESP_OK;
}

/**
 * @brief Fill s_vfstusb
 *
 * @param cdc_intf - interface of tusb for registration
 * @param path - a path where the CDC will be registered
 * @return esp_err_t ESP_OK or ESP_ERR_INVALID_ARG
 */
static esp_err_t vfstusb_init(int cdc_intf, char const *path)
{
    s_vfstusb.cdc_intf = cdc_intf;
    s_vfstusb.tx_mode = DEFAULT_TX_MODE;
    s_vfstusb.rx_mode = DEFAULT_RX_MODE;

    return apply_path(path);
}

/**
 * @brief Clear s_vfstusb to default values
 */
static void vfstusb_deinit(void)
{
    memset(&s_vfstusb, 0, sizeof(s_vfstusb));
}


static int tusb_open(const char *path, int flags, int mode)
{
    (void) mode;
    (void) path;
    s_vfstusb.flags = flags | O_NONBLOCK; // for now only non-blocking mode is implemented
    return 0;
}

static ssize_t tusb_write(int fd, const void *data, size_t size)
{
    FD_CHECK(fd, -1);
    size_t written_sz = 0;
    const char *data_c = (const char *)data;
    _lock_acquire(&(s_vfstusb.write_lock));
    for (size_t i = 0; i < size; i++) {
        int c = data_c[i];
        /* handling the EOL */
        if (c == '\n' && s_vfstusb.tx_mode != ESP_LINE_ENDINGS_LF) {
            if (tinyusb_cdcacm_write_queue_char(s_vfstusb.cdc_intf, '\r')) {
                written_sz++;
            } else {
                break; // can't write anymore
            }
            if (s_vfstusb.tx_mode == ESP_LINE_ENDINGS_CR) {
                continue;
            }
        }
        /* write a char */
        if (tinyusb_cdcacm_write_queue_char(s_vfstusb.cdc_intf, c)) {
            written_sz++;
        } else {
            break; // can't write anymore
        }

    }
    tinyusb_cdcacm_write_flush(s_vfstusb.cdc_intf, 0);
    _lock_release(&(s_vfstusb.write_lock));
    return written_sz;
}

static int tusb_close(int fd)
{
    FD_CHECK(fd, -1);
    return 0;
}

static ssize_t tusb_read(int fd, void *data, size_t size)
{
    FD_CHECK(fd, -1);
    char *data_c = (char *) data;
    size_t received = 0;
    _lock_acquire(&(s_vfstusb.read_lock));
    int cm1 = NONE;
    int c = NONE;

    while (received < size) {
        cm1 = c; // store the old char
        int c = tud_cdc_n_read_char(0); // get a new one
        if (s_vfstusb.rx_mode == ESP_LINE_ENDINGS_CR) {
            if (c == '\r') {
                c = '\n';
            }
        } else if (s_vfstusb.rx_mode == ESP_LINE_ENDINGS_CR) {
            if ((c == '\n') & (cm1 == '\r')) {
                --received; // step back
                c = '\n';
            }
        }
        if ( c == NONE) { // if data ends
            break;
        }
        data_c[received] = (char) c;
        ++received;
        if (c == '\n') {
            break;
        }
    }
    _lock_release(&(s_vfstusb.read_lock));
    if (received > 0) {
        return received;
    }
    errno = EWOULDBLOCK;
    return -1;
}


static int tusb_fstat(int fd, struct stat *st)
{
    FD_CHECK(fd, -1);
    st->st_mode = S_IFCHR;
    return 0;
}

static int tusb_fcntl(int fd, int cmd, int arg)
{
    FD_CHECK(fd, -1);
    int result = 0;
    switch (cmd) {
    case F_GETFL:
        result = s_vfstusb.flags;
        break;
    case F_SETFL:
        s_vfstusb.flags = arg;
        break;
    default:
        result = -1;
        errno = ENOSYS;
        break;
    }
    return result;
}

esp_err_t esp_vfs_tusb_cdc_unregister(char const *path)
{
    ESP_LOGD(TAG, "Unregistering TinyUSB driver");
    int res;

    if (path == NULL) { // NULL means using the default path for unregistering: VFS_TUSB_PATH_DEFAULT
        res = strcmp(s_vfstusb.vfs_path, VFS_TUSB_PATH_DEFAULT);
    } else {
        res = strcmp(s_vfstusb.vfs_path, path);
    }

    if (res) {
        res = ESP_ERR_INVALID_ARG;
        ESP_LOGE(TAG, "There is no TinyUSB driver registerred to the path '%s' (err: 0x%x)", s_vfstusb.vfs_path, res);
        return res;
    }



    res = esp_vfs_unregister(s_vfstusb.vfs_path);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Can't unregister TinyUSB driver from '%s' (err: 0x%x)", s_vfstusb.vfs_path, res);
    } else {
        ESP_LOGD(TAG, "Unregistered TinyUSB driver");
        vfstusb_deinit();
    }
    return res;
}




esp_err_t esp_vfs_tusb_cdc_register(int cdc_intf, char const *path)
{
    ESP_LOGD(TAG, "Registering TinyUSB CDC driver");
    int res;
    if (!tusb_cdc_acm_initialized(cdc_intf)) {
        ESP_LOGE(TAG, "TinyUSB CDC#%d is not initialized", cdc_intf);
        return ESP_ERR_INVALID_STATE;
    }

    res = vfstusb_init(cdc_intf, path);
    if (res != ESP_OK) {
        return res;
    }

    esp_vfs_t vfs = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .close = &tusb_close,
        .fcntl = &tusb_fcntl,
        .fstat = &tusb_fstat,
        .open = &tusb_open,
        .read = &tusb_read,
        .write = &tusb_write,
    };

    res = esp_vfs_register(s_vfstusb.vfs_path, &vfs, NULL);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Can't register TinyUSB driver (err: %x)", res);
    } else {
        ESP_LOGD(TAG, "TinyUSB CDC registered (%s)", s_vfstusb.vfs_path);
    }
    return res;
}
