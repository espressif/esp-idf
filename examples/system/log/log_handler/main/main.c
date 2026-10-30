/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "security_log.h"

#if ESP_LOG_MODE_BINARY_EN
#error "This example uses printf formatting; select text logging."
#endif

#define WARNINGS_PATH "/logs/warnings.log"
#define SENSOR_PATH   "/logs/sensor.log"
#define SECURITY_PATH "/logs/security.log"

typedef struct {
    FILE *warnings;
    FILE *sensor;
    esp_log_handler_t previous;
} app_log_context_t;

static void write_message(FILE *file, const esp_log_msg_t *message)
{
    va_list args;
    va_copy(args, message->args);
    flockfile(file);
    vfprintf(file, message->format, args);
#if ESP_LOG_VERSION == 2
    fputc('\n', file); // V1 macros already include the newline.
#endif
    funlockfile(file);
    va_end(args);
}

static bool handle_app_log(esp_log_msg_t *message, void *ctx)
{
    const app_log_context_t *files = ctx;
    esp_log_level_t level = message->config.opts.log_level;
    if (level == ESP_LOG_ERROR || level == ESP_LOG_WARN) {
        write_message(files->warnings, message);
    }

    if (message->tag && strcmp(message->tag, "SENSOR") == 0) {
        write_message(files->sensor, message);
    }

    return files->previous.fn ? files->previous.fn(message, files->previous.ctx) : true;
}

static void dump_file(const char *path)
{
    FILE *file = fopen(path, "r");
    assert(file != NULL);
    printf("\n--- %s ---\n", path);
    char line[128];
    while (fgets(line, sizeof(line), file) != NULL) {
        fputs(line, stdout);
    }
    fclose(file);
}

void app_main(void)
{
    const esp_vfs_spiffs_conf_t fs = {
        .base_path = "/logs",
        .partition_label = "storage",
        .max_files = 3,
        .format_if_mount_failed = true,
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&fs));

    app_log_context_t files = {
        .warnings = fopen(WARNINGS_PATH, "w"),
        .sensor = fopen(SENSOR_PATH, "w"),
    };
    assert(files.warnings != NULL && files.sensor != NULL);

    ESP_ERROR_CHECK(esp_log_set_handler(handle_app_log, &files, &files.previous));
    security_log_start(SECURITY_PATH); // Wraps the application handler.

    ESP_LOGI("APP", "Startup complete");
    ESP_LOGI("SENSOR", "Temperature: %d C", 25);
    ESP_LOGW("WIFI", "Connection lost, retry=%d", 1);
    ESP_LOGE("SENSOR", "Read failed: error=%d", -1);
    security_log_emit_samples();

    /* All example log calls have returned. Join any producer tasks before teardown. */
    security_log_stop();
    ESP_ERROR_CHECK(esp_log_set_handler(files.previous.fn, files.previous.ctx, NULL));
    fclose(files.warnings);
    fclose(files.sensor);

    dump_file(WARNINGS_PATH);
    dump_file(SENSOR_PATH);
    dump_file(SECURITY_PATH);

    ESP_ERROR_CHECK(esp_vfs_spiffs_unregister(fs.partition_label));
    printf("Log handler example complete\n");
}
