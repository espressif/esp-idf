// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <sys/random.h>
#include <sys/param.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include "esp_system.h"
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
