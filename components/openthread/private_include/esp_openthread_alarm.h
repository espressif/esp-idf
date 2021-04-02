// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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
// limitations under the License

#pragma once

#include "esp_openthread_types.h"
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Updates the file descriptor with the OpenThread alarm timeout.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_alarm_update(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief  Performs the alarm process and triggers the fired timers for OpenThread.
 *
 * @param[in]   instance    The OpenThread instance.
 *
 */
void esp_openthread_alarm_process(otInstance *instance);


#ifdef __cplusplus
}
#endif
