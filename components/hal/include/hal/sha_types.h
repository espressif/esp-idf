// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "sdkconfig.h"

/* Use enum from rom for backwards compatibility */
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/sha.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Use enum from rom for backwards compatibility */
#if CONFIG_IDF_TARGET_ESP32
typedef enum SHA_TYPE esp_sha_type;
#else
typedef SHA_TYPE esp_sha_type;
#endif

#ifdef __cplusplus
}
#endif
