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

#ifndef _GENIE_TIMER_H_
#define _GENIE_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif /**< __cplusplus */

// #define GENIE_VENDOR_TIMER_STORE 1 // store timing data/operation
#define GENIE_STORE_VENDOR_TIMER "vendor_timer"

#pragma pack(1)
typedef struct {
    uint16_t type;
    uint8_t para;
} genie_timer_attr_data_t;
#pragma pack()

typedef struct {
    volatile uint16_t year;    // 2019+
    volatile uint8_t month;    // 0-11
    volatile uint8_t day;      // 1-31
    volatile uint8_t seconds;  // 0-59
    volatile uint8_t minutes;  // 0-59
    volatile uint8_t hour;     // 0-23
    volatile uint8_t weekday;  // 0 means sunday
} utc_time_t;

typedef enum {
    GENIE_TIMER_ERR_OK = 0,
    GENIE_TIMER_ERR_INIT,
    GENIE_TIMER_ERR_LOCALTIME_NOTSET,
    GENIE_TIMER_ERR_INDEX,
    GENIE_TIMER_ERR_PARAM,
    GENIE_TIMER_ERR_NORESOURCE,
    GENIE_TIMER_ERR_OTHER,
} genie_timer_error_t;

typedef enum {
    GENIE_TIME_EVT_TIMEOUT     = 0,
    GENIE_TIME_EVT_TIMING_SYNC = 1,
} genie_timer_event_t;

/**
 * @brief
 *
 */
typedef void *genie_timer_handle_t;

/**
 * @brief genie timer event handle callback function.
 *
 */
typedef int (*genie_timer_event_func_t)(uint8_t event, uint8_t index, genie_timer_attr_data_t *data);

/**
 * @brief Genie time init.
 *
 * @param cb genie timer event handle callback function.
 *
 * @return int
 */
int genie_timer_init(genie_timer_event_func_t cb);

/**
 * @brief Local time update.
 *
 * @param unix_time
 *
 * @return int
 */
int genie_timer_local_time_update(uint32_t unix_time);

/**
 * @brief Get local unix time.
 *
 * @return uint32_t
 */
uint32_t genie_timer_local_unixtime_get(void);

/**
 * @brief Update local timezone.
 *
 * @param timezone
 *
 * @return int
 */
int genie_timer_timezone_update(int8_t timezone);

/**
 * @brief Get local timezone.
 *
 * @return int8_t
 */
int8_t genie_timer_timezone_get(void);

/**
 * @brief
 *
 * @param period_time
 * @param retry_delay
 * @param retry_times
 *
 * @return int
 */
int genie_timer_time_sync_get(uint16_t *period_time, uint8_t *retry_delay, uint8_t *retry_times);

/**
 * @brief
 *
 * @param period_time
 * @param retry_delay
 * @param retry_times
 *
 * @return int
 */
int genie_timer_time_sync_set(uint16_t period_time, uint8_t retry_delay, uint8_t retry_times);

/**
 * @brief Start timer specified by index.
 *
 * @param index
 * @param unix_time
 * @param attr_data
 *
 * @return int
 */
int genie_timer_start(uint8_t index, uint32_t unix_time, genie_timer_attr_data_t *attr_data);

/**
 * @brief Start periodic timer specified by index.
 *
 * @param index
 * @param periodic_time
 * @param schedule
 * @param attr_data
 *
 * @return int
 */
int genie_timer_periodic_start(uint8_t index, uint16_t periodic_time, uint8_t schedule, genie_timer_attr_data_t *attr_data);

/**
 * @brief Remove genie timer.
 *
 * @param index
 *
 * @return int
 */
int genie_timer_remove(uint8_t index);

/**
 * @brief show local time.
 *
 */
void genie_timer_local_time_show(void);

#ifdef __cplusplus
}
#endif /**< __cplusplus */

#endif /* _GENIE_TIMER_H_ */
