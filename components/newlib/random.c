/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/random.h>
#include <sys/param.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include "esp_random.h"
#include "esp_log.h"

static const char *TAG = "RANDOM";

ssize_t getrandom(void *buf, size_t buflen, unsigned int flags)
{
    // Flags are ignored because:
    // - esp_random is non-blocking so it works for both blocking and non-blocking calls,
    // - don't have opportunity so set som other source of entropy.

    ESP_LOGD(TAG, "getrandom(buf=0x%x, buflen=%d, flags=%u)", (int) buf, buflen, flags);

    if (buf == NULL) {
        errno = EFAULT;
        ESP_LOGD(TAG, "getrandom returns -1 (EFAULT)");
        return -1;
    }

    esp_fill_random(buf, buflen);

    ESP_LOGD(TAG, "getrandom returns %d", buflen);
    return buflen;
}
