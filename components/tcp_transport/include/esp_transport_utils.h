// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ESP_TRANSPORT_UTILS_H_
#define _ESP_TRANSPORT_UTILS_H_
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Convert milliseconds to timeval struct
 *
 * @param[in]  timeout_ms  The timeout milliseconds
 * @param[out] tv          Timeval struct
 */
void esp_transport_utils_ms_to_timeval(int timeout_ms, struct timeval *tv);


#define ESP_TRANSPORT_MEM_CHECK(TAG, a, action) if (!(a)) {                                                      \
        ESP_LOGE(TAG,"%s:%d (%s): %s", __FILE__, __LINE__, __FUNCTION__, "Memory exhausted");       \
        action;                                                                                         \
        }

#ifdef __cplusplus
}
#endif
#endif /* _ESP_TRANSPORT_UTILS_H_ */