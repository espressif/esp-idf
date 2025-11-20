/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/stream_buffer.h"
#include "openthread/platform/logging.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_ncp.h"

#define OTLOG_LINE_MAX 256
#define ROM_TAP_SB_SIZE  1024
#define ROM_TAP_TRIGLVL  1

typedef struct {
    char line[OTLOG_LINE_MAX];
    size_t len;
} line_buf_t;

static SemaphoreHandle_t s_mutex;
static line_buf_t s_stdout_buf;
static line_buf_t s_stderr_buf;
static StreamBufferHandle_t s_rom_tap_sb;
static line_buf_t s_rom_line;

static inline line_buf_t *get_buf_for_fd(int fd)
{
    if (fd == STDERR_FILENO) return &s_stderr_buf;
    // default: treat as stdout
    return &s_stdout_buf;
}

// Flush one assembled line to otPlatLog (strip trailing CR/LF)
static void flush_line_to_otlog(line_buf_t *b, otLogLevel level, bool allow_empty)
{
    while (b->len > 0 && (b->line[b->len - 1] == '\n' || b->line[b->len - 1] == '\r')) {
        b->len--;
    }
    b->line[b->len] = '\0';

    if (b->len == 0 && allow_empty) {
        otPlatLog(level, OT_LOG_REGION_NCP, "%s", " ");
        return;
    }

    otPlatLog(level, OT_LOG_REGION_NCP, "%s", b->line);
    b->len = 0;
}

// Accumulate bytes; emit at newline or buffer full.
static void buffer_and_maybe_flush(int fd, const char *data, size_t size)
{
    line_buf_t *b = get_buf_for_fd(fd);
    otLogLevel lvl = (fd == STDERR_FILENO) ? OT_LOG_LEVEL_WARN : OT_LOG_LEVEL_NOTE;

    for (size_t i = 0; i < size; i++) {
        char c = data[i];
        if (b->len < OTLOG_LINE_MAX - 1) {
            b->line[b->len++] = c;
        }

        if (c == '\n') {
            flush_line_to_otlog(b, lvl, /*allow_empty=*/true);
        } else if (b->len == OTLOG_LINE_MAX - 1) {
            flush_line_to_otlog(b, lvl, /*allow_empty=*/false);
        }
    }
}

static ssize_t otlog_write(int fd, const void *data, size_t size)
{
    if (!data || size == 0) return 0;

    if (s_mutex) xSemaphoreTake(s_mutex, portMAX_DELAY);
    buffer_and_maybe_flush(fd, (const char *)data, size);
    if (s_mutex) xSemaphoreGive(s_mutex);

    return (ssize_t)size;
}

static ssize_t otlog_read(int fd, void *dst, size_t size)
{
    (void)fd; (void)dst; (void)size;
    return 0;
}

static int otlog_open(const char *path, int flags, int mode)
{
    (void)path; (void)flags; (void)mode;
    return 0;
}

static int otlog_close(int fd)
{
    (void)fd;
    return 0;
}

static IRAM_ATTR void rom_putc_tap(char c)
{
    BaseType_t woken = pdFALSE;

    if (!xPortCanYield()) {
        xStreamBufferSendFromISR(s_rom_tap_sb, &c, 1, &woken);
        if (woken) portYIELD_FROM_ISR();
    } else {
        xStreamBufferSend(s_rom_tap_sb, &c, 1, 0);
    }
}

static esp_err_t esp_rom_printf_mirror_install(void)
{
    s_rom_tap_sb = xStreamBufferCreate(ROM_TAP_SB_SIZE, ROM_TAP_TRIGLVL);
    ESP_RETURN_ON_FALSE(s_rom_tap_sb, ESP_ERR_NO_MEM, OT_PLAT_LOG_TAG, "Failed to create s_rom_tap_sb buffer");

    esp_rom_install_channel_putc(1, rom_putc_tap);
    return ESP_OK;
}

static void rom_tap_consumer_task(void *arg)
{
    const otLogLevel lvl = OT_LOG_LEVEL_NOTE;
    uint8_t ch;

    for (;;) {
        if (xStreamBufferReceive(s_rom_tap_sb, &ch, 1, portMAX_DELAY) != 1) continue;

        if (s_rom_line.len < OTLOG_LINE_MAX - 1) {
            s_rom_line.line[s_rom_line.len++] = (char)ch;
        }
        if (ch == '\n' || s_rom_line.len == OTLOG_LINE_MAX - 1) {
            flush_line_to_otlog(&s_rom_line, lvl, /*allow_empty=*/true);
            s_rom_line.len = 0;
        }
    }
}

esp_err_t esp_console_redirect_to_otlog(void)
{
    if (!s_mutex) {
        s_mutex = xSemaphoreCreateMutex();
        ESP_RETURN_ON_FALSE(s_mutex, ESP_ERR_NO_MEM, OT_PLAT_LOG_TAG, "Failed to create s_mutex");
    }

    esp_vfs_t vfs = {};
    vfs.flags = ESP_VFS_FLAG_DEFAULT;
    vfs.write = &otlog_write;
    vfs.read  = &otlog_read;
    vfs.open  = &otlog_open;
    vfs.close = &otlog_close;

    ESP_RETURN_ON_ERROR(esp_vfs_register("/dev/otlog", &vfs, NULL), OT_PLAT_LOG_TAG, "vfs register failed");

    FILE *fout = freopen("/dev/otlog", "w", stdout);
    FILE *ferr = freopen("/dev/otlog", "w", stderr);
    ESP_RETURN_ON_FALSE(fout && ferr, ESP_FAIL, OT_PLAT_LOG_TAG, "freopen failed (stdout=%p, stderr=%p)", fout, ferr);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    ESP_RETURN_ON_ERROR(esp_rom_printf_mirror_install(), OT_PLAT_LOG_TAG, "esp_rom_printf mirror install failed");
    xTaskCreatePinnedToCore(rom_tap_consumer_task, "ot_rom_tap", 2048, NULL, 4, NULL, tskNO_AFFINITY);

    return ESP_OK;
}
