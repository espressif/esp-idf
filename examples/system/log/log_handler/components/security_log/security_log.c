/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "security_log.h"

typedef struct {
    FILE *file;
    esp_log_handler_t previous;
} security_context_t;

static security_context_t s_security_context;

static bool handle_security_log(esp_log_msg_t *message, void *ctx)
{
    const security_context_t *security = ctx;
    if (message->tag && strcmp(message->tag, "SECURITY") == 0) {
        char text[256];
        vsnprintf(text, sizeof(text), message->format, message->args);
        fprintf(security->file, "%s%s", text, ESP_LOG_VERSION == 2 ? "\n" : "");
        return false; // Skip the remaining handlers and standard output.
    }
    return security->previous.fn ? security->previous.fn(message, security->previous.ctx) : true;
}

void security_log_start(const char *path)
{
    s_security_context.file = fopen(path, "w");
    assert(s_security_context.file != NULL);
    ESP_ERROR_CHECK(esp_log_set_handler(handle_security_log, &s_security_context, &s_security_context.previous));
}

void security_log_emit_samples(void)
{
    ESP_LOGI("SECURITY", "Audit started");
    ESP_LOGW("SECURITY", "Access attempt requires review");
    ESP_LOGE("SECURITY", "Access denied");
}

void security_log_stop(void)
{
    ESP_ERROR_CHECK(esp_log_set_handler(s_security_context.previous.fn, s_security_context.previous.ctx, NULL));
    fclose(s_security_context.file);
}
