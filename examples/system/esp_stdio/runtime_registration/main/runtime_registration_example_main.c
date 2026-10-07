/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Console I/O multiplexer example.
 *
 * This example demonstrates the runtime API of the esp_stdio console
 * multiplexer:
 *
 *   - esp_stdio_register_driver()   -- add a custom VFS backend as an auxiliary
 *   - esp_stdio_set_primary_driver()       -- promote it to the active primary and
 *                                      later restore the previous one
 *   - esp_stdio_unregister_driver() -- remove it again
 *
 * The multiplexer lives on /dev/console (ESP_VFS_DEV_CONSOLE). This example
 * writes to that device directly so the behavior is identical on chip targets
 * and on the Linux host. (On chip targets newlib also wires stdout/printf to
 * /dev/console, so the same fan-out applies to plain printf; on the Linux host
 * printf goes straight to the host stdout and bypasses the multiplexer, which
 * is why the demonstration writes to /dev/console explicitly.)
 *
 * The custom backend used here is a small "capture" sink: every byte written to
 * the console while it is registered is copied into a RAM buffer. At the end the
 * example prints back what the backend recorded, so the effect of the
 * multiplexer's fan-out is visible.
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "esp_log.h"
#include "esp_stdio.h"

static const char *TAG = "mux_example";

#define CAPTURE_BUF_SIZE 512

/* State of the custom "capture" backend, forwarded to every VFS callback as the
 * context pointer. */
typedef struct {
    char buf[CAPTURE_BUF_SIZE];
    size_t len;
} capture_state_t;

static capture_state_t s_capture;

static int capture_open(void *ctx, const char *path, int flags, int mode)
{
    (void)ctx;
    (void)path;
    (void)flags;
    (void)mode;
    return 0; /* a single, always-open pseudo fd is enough */
}

static int capture_close(void *ctx, int fd)
{
    (void)ctx;
    (void)fd;
    return 0;
}

static ssize_t capture_write(void *ctx, int fd, const void *data, size_t size)
{
    (void)fd;
    capture_state_t *s = (capture_state_t *)ctx;

    /* Keep one byte for the terminating NUL so the buffer stays printable.
     * Note: never call any logging / printf here -- that would recursively
     * write to the console and could loop forever. */
    size_t room = CAPTURE_BUF_SIZE - 1 - s->len;
    size_t n = size < room ? size : room;
    memcpy(s->buf + s->len, data, n);
    s->len += n;
    s->buf[s->len] = '\0';

    return (ssize_t)size; /* report the full size as consumed */
}

static ssize_t capture_read(void *ctx, int fd, void *dst, size_t size)
{
    (void)ctx;
    (void)fd;
    (void)dst;
    (void)size;
    return 0; /* nothing to read from the capture backend (EOF) */
}

static int capture_fstat(void *ctx, int fd, struct stat *st)
{
    (void)ctx;
    (void)fd;
    if (st) {
        memset(st, 0, sizeof(*st));
    }
    return 0;
}

static int capture_fsync(void *ctx, int fd)
{
    (void)ctx;
    (void)fd;
    return 0;
}

/* VFS operations table for the capture backend. The `_p` (context-aware)
 * callbacks are used because a context pointer is supplied at registration. */
static const esp_vfs_fs_ops_t s_capture_vfs = {
    .write_p = capture_write,
    .open_p  = capture_open,
    .close_p = capture_close,
    .read_p  = capture_read,
    .fstat_p = capture_fstat,
    .fsync_p = capture_fsync,
};

static void console_puts(int fd, const char *line)
{
    size_t remaining = strlen(line);
    while (remaining > 0) {
        ssize_t written = write(fd, line, remaining);
        if (written <= 0) {
            break;
        }
        line += written;
        remaining -= (size_t)written;
    }
}

void app_main(void)
{
    /* Register the capture backend as a write-only auxiliary. The multiplexer
     * opens each registered backend when /dev/console is opened, so register
     * before opening the console to have the capture backend wired in. */
    const esp_stdio_driver_config_t config = {
        .vfs_ops = &s_capture_vfs,
        .vfs_ctx = &s_capture,
        .path = "/",
    };
    esp_stdio_handle_t capture;
    ESP_ERROR_CHECK(esp_stdio_register_driver(&config, &capture));

    int console = open(ESP_VFS_DEV_CONSOLE, O_RDWR);
    if (console < 0) {
        ESP_LOGE(TAG, "failed to open %s", ESP_VFS_DEV_CONSOLE);
        return;
    }

    /* Every byte written to the console is now fanned out to the capture
     * auxiliary in addition to the primary. */
    console_puts(console, "[1] Written while the capture backend is an auxiliary.\n");

    /* Promote the capture backend to the active read+write primary. The
     * previous primary is returned so it can be restored later; it is not
     * removed, it simply drops back to a write-only auxiliary. */
    esp_stdio_handle_t previous_primary;
    ESP_ERROR_CHECK(esp_stdio_set_primary_driver(capture, &previous_primary));

    console_puts(console, "[2] Written while the capture backend is the primary.\n");

    /* Restore the original primary. The capture backend becomes an auxiliary
     * again, so it keeps receiving the fan-out. */
    ESP_ERROR_CHECK(esp_stdio_set_primary_driver(previous_primary, NULL));

    console_puts(console, "[3] Written after restoring the original primary.\n");

    /* The active primary cannot be unregistered, so close the console and
     * remove the (now auxiliary) capture backend. */
    close(console);
    ESP_ERROR_CHECK(esp_stdio_unregister_driver(capture));

    /* Show what the capture backend recorded while it was registered. On the
     * Linux host the console primary is a bit-bucket and the lines above are
     * not printed, so this RAM dump is what makes the fan-out observable. */
    printf("----- capture backend recorded %u bytes -----\n", (unsigned)s_capture.len);
    fputs(s_capture.buf, stdout);
    printf("----- end of captured data -----\n");
}
