// Copyright (C) 2018-2020 Alibaba Group Holding Limited
// Adaptations to ESP-IDF Copyright (c) 2020 Espressif Systems (Shanghai) Co. Ltd.
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

#ifndef _GENIE_RESET_H_
#define _GENIE_RESET_H_

#ifdef __cplusplus
extern "C" {
#endif /**< __cplusplus */

#define CONFIG_GENIE_RESET_BY_REPEAT      1

#define GENIE_RESET_BY_REPEAT_COUNTER     3
#define GENIE_RESET_BY_REPEAT_TIMEOUT     (10 * 1000)
#define GENIE_RESET_WAIT_DONE_TIMEOUT     (10 * 1000)

#define GENIE_STORE_RESTART_COUNT_KEY     "restart_count"

/**
 * @brief Get reset flag
 *
 * @return uint8_t
 */
uint8_t genie_reset_get_flag(void);

/**
 * @brief Start the timer to handle the program reset operation. Avoid reset indication messages not reaching Genie Speaker.
 *
 */
void genie_reset_done_timer_start(void);

/**
 * @brief Initialize the function that resets whole system by multiple reboot in GENIE_RESET_BY_REPEAT_TIMEOUT.
 *
 */
void genie_reset_by_repeat_init(void);

/**
 * @brief Clean the recored count for reboot.
 *
 */
void genie_reset_clean_count(void);

#ifdef __cplusplus
}
#endif /**< __cplusplus */

#endif /* _GENIE_RESET_H_ */
