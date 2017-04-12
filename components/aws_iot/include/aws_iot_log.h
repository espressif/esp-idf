// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

/* (these two headers aren't used here, but AWS IoT SDK code relies on them
   being included from here...) */
#include <stdio.h>
#include <stdlib.h>

#include "esp_log.h"

/* This is a stub replacement for the aws_iot_log.h header in the AWS IoT SDK,
   which redirects their logging framework into the esp-idf logging framework.

   The current (2.1.1) upstream AWS IoT SDK doesn't allow this as some of its
   headers include aws_iot_log.h, but our modified fork does.
*/

// redefine the AWS IoT log functions to call into the IDF log layer
#define IOT_DEBUG(format, ...) ESP_LOGD("aws_iot", format, ##__VA_ARGS__)
#define IOT_INFO(format, ...) ESP_LOGI("aws_iot", format, ##__VA_ARGS__)
#define IOT_WARN(format, ...) ESP_LOGW("aws_iot", format, ##__VA_ARGS__)
#define IOT_ERROR(format, ...) ESP_LOGE("aws_iot", format, ##__VA_ARGS__)

/* Function tracing macros used in AWS IoT SDK,
   mapped to "verbose" level output
*/
#define FUNC_ENTRY ESP_LOGV("aws_iot", "FUNC_ENTRY:   %s L#%d \n", __func__, __LINE__)
#define FUNC_EXIT_RC(x) \
    do {                                                                \
        ESP_LOGV("aws_iot", "FUNC_EXIT:   %s L#%d Return Code : %d \n", __func__, __LINE__, x); \
        return x; \
    } while(0)
