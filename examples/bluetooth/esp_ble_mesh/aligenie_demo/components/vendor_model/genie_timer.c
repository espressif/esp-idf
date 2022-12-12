/*
 * SPDX-FileCopyrightText: 2018-2020 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2020-2022 Espressif Systems (Shanghai) CO LTD
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "genie_mesh.h"
#include "genie_event.h"
#include "genie_util.h"
#include "genie_timer.h"
#include "genie_model_srv.h"
#include "ble_mesh_example_nvs.h"

#define DAY     86400
#define HOUR    3600
#define MINU    60

#define VT_NUM (40)
#define VT_LOCK    util_semaphore_take(&g_genie_timer.lock, -1)
#define VT_UNLOCK  util_semaphore_give(&g_genie_timer.lock)

extern nvs_handle_t NVS_HANDLE;

static const char *TAG = "genie_timer";

static utc_time_t local_time = {0};

typedef enum {
    TIMER_OFF     = 0,
    TIMER_ON      = 1,
    TIMER_INVAILD = 0xf,
} vt_state;

struct genie_timer_t {
    genie_snode_t next;
    uint8_t index;
    uint8_t state: 4;
    uint8_t periodic: 1;
    uint16_t periodic_time;
    uint8_t schedule;
    uint32_t unixtime_match;
    genie_timer_attr_data_t attr_data;
};

struct unixtime_sync_para_t {
    uint16_t period_time; // Synchronization period: Time synchronization request cycle.
    uint8_t retry_delay;  // Retry delay: How long to retry after a failed time synchronization request
    uint8_t retry_times;  // Retry times of time synchronization request
};

struct {
    uint16_t magic;
    int8_t timezone;
    struct unixtime_sync_para_t timing_sync_config;  // Time synchronization parameters
    struct genie_timer_t timer_data[VT_NUM];    // Store Timing operation
} g_timing_data;

struct {
    uint32_t init: 1;
    uint32_t update: 1;
    esp_timer_handle_t timer;
    struct k_work work;
    util_semaphore_t lock;
    genie_slist_t timer_list_active;
    genie_slist_t timer_list_idle;
    uint32_t unix_time;
    uint32_t unix_time_sync_match;
    uint8_t  unix_time_sync_retry_times;
    genie_timer_event_func_t cb;
} g_genie_timer;

static inline uint8_t is_leap_year(uint16_t year);

utc_time_t genie_timer_local_time_get(void)
{
    return local_time;
}

uint32_t genie_timer_local_unixtime_get(void)
{
    return g_genie_timer.unix_time;
}

static inline utc_time_t convert_unix_to_utc(uint32_t unix_time)
{
    uint16_t g_noleap_daysbeforemonth[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    utc_time_t utc     = {0};
    uint32_t   epoch   = 0;
    uint32_t   jdn     = 0;
    int        year    = 0;
    int        month   = 0;
    int        day     = 0;
    int        hour    = 0;
    int        minutes = 0;
    int        sec     = 0;
    int        weekday = 0;

    epoch = unix_time;

    jdn    = epoch / DAY;
    epoch -= DAY * jdn;

    weekday = (jdn + 4) % 7; // 1970/1/1 is thursday

    hour   = epoch / HOUR;
    epoch -= HOUR * hour;

    minutes = epoch / MINU;
    epoch  -= MINU * minutes;

    sec    = epoch;

    year   = jdn   / (4 * 365 + 1); /* Number of 4-years periods since the epoch */
    jdn   -= year * (4 * 365 + 1);  /* Remaining days */
    year <<= 2;                  /* Years since the epoch */

    /* Then we will brute force the next 0-3 years */
    bool leapyear;
    int  tmp;

    for (; ;) {
        /* Is this year a leap year (we'll need this later too) */
        leapyear = is_leap_year(year + 1970);

        /* Get the number of days in the year */
        tmp = (leapyear ? 366 : 365);

        /* Do we have that many days? */
        if (jdn >= tmp) {
            /* Yes.. bump up the year */
            year++;
            jdn -= tmp;
        } else {
            /* Nope... then go handle months */
            break;
        }
    }

    /* At this point, value has the year and days has number days into this year */
    year += 1970;

    /* Handle the month (zero based) */
    int min   = 0;
    int max   = 11;
    int value = 0;

    do {
        /* Get the midpoint */
        value = (min + max) >> 1;

        /* Get the number of days that occurred before the beginning of the month
         * following the midpoint.
         */
        tmp = g_noleap_daysbeforemonth[value + 1];
        if (value + 1 >= 2 && is_leap_year(leapyear)) {
            tmp++;
        }

        /* Does the number of days before this month that equal or exceed the
         * number of days we have remaining?
         */
        if (tmp > jdn) {
            /* Yes.. then the month we want is somewhere from 'min' and to the
             * midpoint, 'value'.  Could it be the midpoint?
             */
            tmp = g_noleap_daysbeforemonth[value];
            if (value >= 2 && is_leap_year(leapyear)) {
                tmp++;
            }

            if (tmp > jdn) {
                /* No... The one we want is somewhere between min and value-1 */
                max = value - 1;
            } else {
                /* Yes.. 'value' contains the month that we want */
                break;
            }
        } else {
            /* No... The one we want is somwhere between value+1 and max */
            min = value + 1;
        }

        /* If we break out of the loop because min == max, then we want value
         * to be equal to min == max.
         */
        value = min;
    } while (min < max);

    /* The selected month number is in value. Subtract the number of days in the
     * selected month
     */
    tmp = g_noleap_daysbeforemonth[value];

    if (value >= 2 && is_leap_year(leapyear)) {
        tmp++;
    }

    jdn -= tmp;

    /* At this point, value has the month into this year (zero based) and days has
     * number of days into this month (zero based)
     */
    month = value;   // zero based
    day   = jdn + 1; // one based

    utc.year    = year;
    utc.month   = month;
    utc.day     = day;
    utc.weekday = weekday;
    utc.hour    = hour;
    utc.minutes = minutes;
    utc.seconds = sec;

    return utc;
}

static inline uint32_t convert_utc_to_unix(utc_time_t *utc_time)
{
    uint32_t days;
    uint16_t g_noleap_daysbeforemonth[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    days = (utc_time->year - 1970) * 365;
    days += (utc_time->year - 1969) >> 2;

    days += g_noleap_daysbeforemonth[utc_time->month];

    if (utc_time->month >= 2 && is_leap_year(utc_time->year)) {
        days++;
    }

    days += utc_time->day - 1;

    return ((days * 24 + utc_time->hour) * 60 + utc_time->minutes) * 60 + utc_time->seconds;
}

static inline uint8_t is_leap_year(uint16_t year)
{
    if (((year % 4) == 0) && ((year % 100) != 0)) {
        return 1;
    } else if ((year % 400) == 0) {
        return 1;
    } else {
        return 0;
    }
}

static inline void month_update(void)
{
    local_time.month++;

    if (local_time.month >= 12) {
        local_time.month = 0;
        local_time.year++;
    }
}

static inline void days_update(void)
{
    uint8_t month_days_list[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
    };

    local_time.day++;

    if (is_leap_year(local_time.year)) {
        month_days_list[1] = 29;
    }

    uint8_t month_day = month_days_list[local_time.month];

    if (local_time.day > month_day) {
        local_time.day = 0;
        month_update();
    }

    local_time.weekday = (local_time.weekday + 1) % 7;
}

static inline void hours_update(void)
{
    local_time.hour++;

    if (local_time.hour == 24) {
        local_time.hour = 0;
        days_update();
    }
}

static inline void minutes_update(void)
{
    local_time.minutes++;

    if (local_time.minutes == 60) {
        local_time.minutes = 0;
        hours_update();
    }
}

static inline void seconds_update(void)
{
    local_time.seconds++;

    if (local_time.seconds == 60) {
        local_time.seconds = 0;
        minutes_update();
    }
}

static void genie_timer_update(void *args)
{
    if (!g_genie_timer.update) {
        ESP_LOGD(TAG, "g_genie_timer.update %d", g_genie_timer.update);
        return;
    }

    g_genie_timer.unix_time += 1;
    seconds_update();

    k_work_submit(&g_genie_timer.work);

    if (g_genie_timer.unix_time % 60 == 0) {
        ESP_LOGI(TAG, "genie_timer_update %" PRIu32, g_genie_timer.unix_time);
    }
}

static inline uint8_t next_weekday_diff_get(uint8_t weekday_now, uint8_t schedule)
{
    uint16_t schedule_tmp = 0;

    if (weekday_now == 0) {
        weekday_now = 7;
    }

    schedule_tmp = ((schedule | ((uint16_t) schedule << 7)) >> (weekday_now - 1)) & 0x7f;

    uint8_t day_diff = 0;

    while (day_diff < 7) {
        if ((schedule_tmp >> day_diff)  & 0x0001) {
            break;
        }
        day_diff++;
    }

    return day_diff;
}

static inline uint8_t is_weekday_match(uint8_t weekday_now, uint8_t schedule)
{
    uint8_t weekday_mask = weekday_now ? (uint8_t)(1 << (weekday_now - 1)) : (uint8_t)(1 << 6);
    return (weekday_mask == (schedule & weekday_mask));
}

static inline uint8_t next_weekday(uint8_t weekday_now)
{
    return (weekday_now + 1) % 7;
}

static int genie_timer_save(void)
{
#ifdef GENIE_VENDOR_TIMER_STORE
    int ret = 0;

    ret = ble_mesh_nvs_store(NVS_HANDLE, GENIE_STORE_VENDOR_TIMER, &g_timing_data, sizeof(g_timing_data));
    if (ret) {
        ESP_LOGI(TAG, "vendor timers save fail %d", ret);
    }

    return ret;
#else
    return -1;
#endif
}

static int genie_timer_restore(void)
{
#ifdef GENIE_VENDOR_TIMER_STORE
    int  ret   = 0;
    bool exist = false;

    memset(&g_timing_data, 0, sizeof(g_timing_data));

    ret = ble_mesh_nvs_restore(NVS_HANDLE, GENIE_STORE_VENDOR_TIMER, &g_timing_data, sizeof(g_timing_data), &exist);
    if (ret) {
        ESP_LOGI(TAG, "vendor timers restore fail %d", ret);
        return ret;
    }

    if (g_timing_data.magic != 0xABCD
            || g_timing_data.timezone < -12
            || g_timing_data.timezone > 12) {
        ESP_LOGI(TAG, "vendor timers restore missmatch");
        return -1;
    }

    for (int i = 0; i < VT_NUM; i++) {
        if (g_timing_data.timer_data[i].state != TIMER_INVAILD) {
            genie_slist_append(&g_genie_timer.timer_list_active, &g_timing_data.timer_data[i].next);
        } else {
            genie_slist_append(&g_genie_timer.timer_list_idle, &g_timing_data.timer_data[i].next);
        }

        ESP_LOGI(TAG, "restore vendor timer index %d state %d periodic %d periodic_time %d schedule %d unixtime_match %d",
                 g_timing_data.timer_data[i].index, g_timing_data.timer_data[i].state,
                 g_timing_data.timer_data[i].periodic, g_timing_data.timer_data[i].periodic_time,
                 g_timing_data.timer_data[i].schedule, g_timing_data.timer_data[i].unixtime_match);
    }

    return 0;
#else
    return -1;
#endif
}

static uint8_t is_genie_timer_timeout(struct genie_timer_t *vendor_timer)
{
    if (vendor_timer->state == TIMER_INVAILD) {
        return 0;
    }

    if (vendor_timer->periodic) {
        if (is_weekday_match(local_time.weekday, vendor_timer->schedule)
                && vendor_timer->unixtime_match < g_genie_timer.unix_time) {
            vendor_timer->unixtime_match += (1 + next_weekday_diff_get(next_weekday(local_time.weekday), vendor_timer->schedule)) * DAY;
        }
    }

    return vendor_timer->unixtime_match == g_genie_timer.unix_time;
}

static void genie_timer_check(void)
{
    struct genie_timer_t *tmp, *node;
    GENIE_SLIST_FOR_EACH_CONTAINER_SAFE(&g_genie_timer.timer_list_active, node,  tmp, next) {
        if (is_genie_timer_timeout(node)) {
            if (g_genie_timer.cb) {
                g_genie_timer.cb(GENIE_TIME_EVT_TIMEOUT, node->index, &node->attr_data);
            }

            VT_LOCK;

            if (!node->periodic) {
                node->unixtime_match = 0xffffffff;
                node->state          = TIMER_INVAILD;
                genie_slist_find_and_remove(&g_genie_timer.timer_list_active, &node->next);
                genie_slist_append(&g_genie_timer.timer_list_idle, &node->next);
            } else {
                node->unixtime_match += 24 * HOUR;
            }

            VT_UNLOCK;
            genie_timer_save();
        }
    }

    if (g_genie_timer.unix_time_sync_match
            && g_genie_timer.unix_time_sync_match <= g_genie_timer.unix_time) {
        if (g_genie_timer.cb) {
            int ret = g_genie_timer.cb(GENIE_TIME_EVT_TIMING_SYNC, 0, NULL);

            if (ret) {
                if (g_genie_timer.unix_time_sync_retry_times > 0) {
                    g_genie_timer.unix_time_sync_match += g_timing_data.timing_sync_config.retry_delay * MINU;
                    g_genie_timer.unix_time_sync_retry_times--;
                    return;
                }
            }
        }

        g_genie_timer.unix_time_sync_match       = g_genie_timer.unix_time + g_timing_data.timing_sync_config.period_time * MINU;
        g_genie_timer.unix_time_sync_retry_times = g_timing_data.timing_sync_config.retry_times;
    }
}

static void genie_timer_check_work(struct k_work *work)
{
    genie_timer_check();
}

static genie_timer_handle_t genie_timer_find(uint8_t index)
{
    ENTER_FUNC();
    if (index >= VT_NUM) {
        return NULL;
    }

    VT_LOCK;
    struct genie_timer_t *tmp, *node = NULL;
    genie_slist_t *list = NULL;
    list = &g_genie_timer.timer_list_active;

    GENIE_SLIST_FOR_EACH_CONTAINER_SAFE(list, node,  tmp, next) {
        if (node->index == index) {
            VT_UNLOCK;
            return node;
        }
    }
    VT_UNLOCK;

    return NULL;
}

static struct genie_timer_t *genie_timer_new(void)
{
    ENTER_FUNC();
    struct genie_timer_t *free_timer = NULL;
    VT_LOCK;
    free_timer = (struct genie_timer_t *)genie_slist_get(&g_genie_timer.timer_list_idle);
    VT_UNLOCK;
    ESP_LOGI(TAG, "timer new %p", free_timer);
    return free_timer;
}

int genie_timer_start(uint8_t index, uint32_t unix_time, genie_timer_attr_data_t *attr_data)
{
    ENTER_FUNC();
    struct genie_timer_t *vendor_timer = NULL;

    if (!attr_data) {
        return -GENIE_TIMER_ERR_PARAM;
    }

    ESP_LOGI(TAG, "timer start index %d expect unix_time %" PRIu32 " attr_type %d",
             index,  unix_time, attr_data->type);

    if (!g_genie_timer.init) {
        return -GENIE_TIMER_ERR_INIT;
    }

    if (!g_genie_timer.update) {
        return -GENIE_TIMER_ERR_LOCALTIME_NOTSET;
    }

    if (index >= VT_NUM) {
        //return -GENIE_TIMER_ERR_INDEX;
    }

    if (unix_time <= g_genie_timer.unix_time) {
        return -GENIE_TIMER_ERR_PARAM;
    }

    vendor_timer = genie_timer_find(index);

    if (vendor_timer == NULL) {
        vendor_timer = genie_timer_new();

        if (vendor_timer == NULL) {
            return -GENIE_TIMER_ERR_NORESOURCE;
        }
    } else {
        VT_LOCK;
        genie_slist_find_and_remove(&g_genie_timer.timer_list_active, &vendor_timer->next);
        VT_UNLOCK;
    }

    vendor_timer->index          = index;
    vendor_timer->unixtime_match = unix_time;        // + g_genie_timer.timezone * HOUR;
    vendor_timer->state          = TIMER_ON;
    vendor_timer->attr_data.type = attr_data->type;
    vendor_timer->attr_data.para = attr_data->para;

    VT_LOCK;
    genie_slist_append(&g_genie_timer.timer_list_active, &vendor_timer->next);
    VT_UNLOCK;

    genie_timer_save();

    return 0;
}

int genie_timer_periodic_start(uint8_t index, uint16_t periodic_time, uint8_t schedule, genie_timer_attr_data_t *attr_data)
{
    ENTER_FUNC();
    struct genie_timer_t *vendor_timer = NULL;

    ESP_LOGI(TAG, "periodic timer start index %d periodic_time %d schedule %d attr_para %d",
             index,  periodic_time,  schedule, attr_data->para);

    if (!g_genie_timer.init) {
        return -GENIE_TIMER_ERR_INIT;
    }

    if (!g_genie_timer.update) {
        return -GENIE_TIMER_ERR_LOCALTIME_NOTSET;
    }

    if (index >= VT_NUM) {
        //return -GENIE_TIMER_ERR_INDEX;
    }

    if (schedule == 0) {
        return -GENIE_TIMER_ERR_PARAM;
    }

    vendor_timer = genie_timer_find(index);

    if (vendor_timer == NULL) {
        vendor_timer = genie_timer_new();

        if (vendor_timer == NULL) {
            return -GENIE_TIMER_ERR_NORESOURCE;
        }
    } else {
        VT_LOCK;
        genie_slist_find_and_remove(&g_genie_timer.timer_list_active, &vendor_timer->next);
        VT_UNLOCK;
    }

    vendor_timer->index         = index;
    vendor_timer->periodic      = 1;
    vendor_timer->periodic_time = periodic_time;
    vendor_timer->schedule      = schedule;
    vendor_timer->state         = TIMER_ON;

    utc_time_t utc         = local_time;
    utc.hour    = 0;
    utc.minutes = 0;
    utc.seconds = 0;
    utc.day     = utc.day + next_weekday_diff_get(local_time.weekday, schedule);

    vendor_timer->unixtime_match = convert_utc_to_unix(&utc) + periodic_time - g_timing_data.timezone * HOUR;

    ESP_LOGI(TAG, "periodic timer unixtime_match %" PRIu32, vendor_timer->unixtime_match);

    VT_LOCK;
    genie_slist_append(&g_genie_timer.timer_list_active, &vendor_timer->next);
    VT_UNLOCK;
    genie_timer_save();
    return 0;
}

static int genie_timer_stop(int8_t index)
{
    ENTER_FUNC();
    ESP_LOGI(TAG, "timer stop %d", index);

    if (!g_genie_timer.init) {
        return -GENIE_TIMER_ERR_INIT;
    }

    if (!g_genie_timer.update) {
        return -GENIE_TIMER_ERR_LOCALTIME_NOTSET;
    }

    if (index >= VT_NUM) {
        return -GENIE_TIMER_ERR_INDEX;
    }

    struct genie_timer_t *vendor_timer = genie_timer_find(index);

    if (vendor_timer == NULL) {
        return -GENIE_TIMER_ERR_INDEX;
    } else {
        VT_LOCK;
        vendor_timer->index          = 0xFF;
        vendor_timer->state          = TIMER_INVAILD;
        vendor_timer->unixtime_match = 0;
        genie_slist_find_and_remove(&g_genie_timer.timer_list_active, &vendor_timer->next);
        genie_slist_append(&g_genie_timer.timer_list_idle, &vendor_timer->next);
        VT_UNLOCK;
    }

    return 0;
}

int genie_timer_remove(uint8_t index)
{
    ENTER_FUNC();
    int i   = 0;
    int ret = 0;

    ESP_LOGI(TAG, "timer remove %d", index);

    /* remove alll timers */
    if (index == 0xFF) {
        for (i = 0; i < VT_NUM; i++) {
            genie_timer_stop(i);
        }

        return 0;
    }

    ret = genie_timer_stop(index);

    genie_timer_save();

    return ret;
}

void genie_timer_local_time_show(void)
{
    ENTER_FUNC();
    ESP_LOGI(TAG, "unix_time revert %" PRIu32, convert_utc_to_unix(&local_time));
    ESP_LOGI(TAG, "%4d/%2d/%2d %2d:%2d:%d weekday %2d %04d",
             local_time.year, local_time.month + 1, local_time.day,
             local_time.hour, local_time.minutes, local_time.seconds,
             local_time.weekday, g_timing_data.timezone);
}

int genie_timer_timezone_update(int8_t timezone)
{
    ENTER_FUNC();
    ESP_LOGI(TAG, "timezone update %d", timezone);

    if (timezone < -12 || timezone > 12) {
        return -GENIE_TIMER_ERR_PARAM;
    }

    if (!g_genie_timer.init) {
        return -GENIE_TIMER_ERR_INIT;
    }

    g_timing_data.timezone = timezone;

    return 0;
}

int8_t genie_timer_timezone_get(void)
{
    ENTER_FUNC();

    return g_timing_data.timezone;
}

int genie_timer_time_sync_set(uint16_t period_time, uint8_t retry_delay,   uint8_t retry_times)
{
    ENTER_FUNC();
    ESP_LOGI(TAG, "timing sync set period_time %d retry_delay %d retry_times %d",
             period_time, retry_delay, retry_times);

    if (period_time == 0 || retry_delay == 0 || retry_times) {
        return -GENIE_TIMER_ERR_PARAM;
    }

    g_timing_data.timing_sync_config.period_time = period_time;
    g_timing_data.timing_sync_config.retry_delay = retry_delay;
    g_timing_data.timing_sync_config.retry_times = retry_times;

    g_genie_timer.unix_time_sync_match       = g_genie_timer.unix_time +  g_timing_data.timing_sync_config.period_time * MINU;
    g_genie_timer.unix_time_sync_retry_times = retry_times;

    return 0;
}

int genie_timer_time_sync_get(uint16_t *period_time, uint8_t *retry_delay, uint8_t *retry_times)
{
    ENTER_FUNC();

    *period_time = g_timing_data.timing_sync_config.period_time;
    *retry_delay = g_timing_data.timing_sync_config.retry_delay;
    *retry_times = g_timing_data.timing_sync_config.retry_times;

    return 0;
}

int genie_timer_local_time_update(uint32_t unix_time)
{
    ENTER_FUNC();
    if (!g_genie_timer.init) {
        ESP_LOGE(TAG, "g_genie_timer not init");
        return -GENIE_TIMER_ERR_INIT;
    }

    g_genie_timer.update    = 1;
    g_genie_timer.unix_time = unix_time;

    local_time = convert_unix_to_utc(unix_time + g_timing_data.timezone * HOUR);

    ESP_LOGI(TAG, "unix_time %" PRIu32, unix_time);
    ESP_LOGI(TAG, "localtime update %4d/%2d/%2d %2d:%2d:%d weekday %2d",
             local_time.year, local_time.month + 1, local_time.day,
             local_time.hour, local_time.minutes, local_time.seconds,
             local_time.weekday);
    ESP_LOGI(TAG, "unix_time revert %" PRIu32, convert_utc_to_unix(&local_time));

    return 0;
}

int genie_timer_init(genie_timer_event_func_t cb)
{
    ENTER_FUNC();
    int i = 0;

    if (g_genie_timer.init) {
        return 0;
    }

    if (cb == NULL) {
        return -GENIE_TIMER_ERR_INIT;
    }

    memset(&g_genie_timer, 0, sizeof(g_genie_timer));
    memset(&local_time, 0, sizeof(local_time));

    g_genie_timer.cb = cb;

    genie_slist_init(&g_genie_timer.timer_list_active);
    genie_slist_init(&g_genie_timer.timer_list_idle);

    util_semaphore_init(&g_genie_timer.lock, 1, 1);

    k_work_init(&g_genie_timer.work, genie_timer_check_work);

    esp_timer_create_args_t create_args = {
        .callback = genie_timer_update,
        .name     = "genie_timer"
    };

    ESP_ERROR_CHECK(esp_timer_create(&create_args, &g_genie_timer.timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(g_genie_timer.timer, 1000 * 1000));

    g_genie_timer.init = 1;

    if (genie_timer_restore()) {
        memset(&g_timing_data, 0, sizeof(g_timing_data));
        g_timing_data.timezone = 8;
        g_timing_data.magic    = 0xABCD;

        for (i = 0; i < VT_NUM; i++) {
            g_timing_data.timer_data[i].unixtime_match = 0xffffffff;
            g_timing_data.timer_data[i].index = 0xFF;
            g_timing_data.timer_data[i].state = TIMER_INVAILD;
            genie_slist_append(&g_genie_timer.timer_list_idle, &g_timing_data.timer_data[i].next);
        }
    }

    /* sync timing */
    // g_genie_timer.cb(GENIE_TIME_EVT_TIMING_SYNC, 0, NULL);

    return 0;
}
