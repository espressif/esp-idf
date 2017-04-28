/* mumm_malloc-compatible DBGLOG API, which maps to esp_log
   primitives */
#pragma once

#include "esp_log.h"

static const char *TAG = "mumm_malloc";

#define DBGLOG_TRACE(format, ...) ESP_EARLY_LOGV(TAG, format, ## __VA_ARGS__)
#define DBGLOG_DEBUG(format, ...) ESP_EARLY_LOGD(TAG, format, ## __VA_ARGS__)
#define DBGLOG_CRITICAL(format, ...) ESP_EARLY_LOGW(TAG, format, ## __VA_ARGS__)
#define DBGLOG_ERROR(format, ...) ESP_EARLY_LOGE(TAG, format, ## __VA_ARGS__)
#define DBGLOG_WARNING(format, ...) ESP_EARLY_LOGW(TAG, format, ## __VA_ARGS__)

/* DBGLOG_FORCE in mumm_malloc always prints, but in this case we use ESP_LOGI */
#define DBGLOG_FORCE(force, format, ...) do { if(force) { ESP_LOGI(TAG, format, ## __VA_ARGS__); } } while(0)
