/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "ble_cts_prph.h"
#include "services/cts/ble_svc_cts.h"
#include <time.h>
#include "sys/time.h"

static const char *TAG = "CTS_GATT_SVR";

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                    ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        MODLOG_DFLT(DEBUG, "registering characteristic %s with "
                    "def_handle=%d val_handle=%d\n",
                    ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                    ctxt->chr.def_handle,
                    ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                    ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

static struct ble_svc_cts_local_time_info local_info = { .timezone = 0, .dst_offset = TIME_STANDARD };
static struct timeval last_updated;
uint8_t adjust_reason;
int fetch_current_time(struct ble_svc_cts_curr_time *ctime) {
    time_t now;
    struct tm timeinfo;
    struct timeval tv_now;
    /* time given by 'time()' api does not persist after reboots */
    time(&now);
    localtime_r(&now, &timeinfo);
    gettimeofday(&tv_now, NULL);
    if(ctime != NULL) {
        /* fill date_time */
        ctime->et_256.d_d_t.d_t.year = timeinfo.tm_year + 1900;
        ctime->et_256.d_d_t.d_t.month = timeinfo.tm_mon + 1;
        ctime->et_256.d_d_t.d_t.day = timeinfo.tm_mday;
        ctime->et_256.d_d_t.d_t.hours = timeinfo.tm_hour;
        ctime->et_256.d_d_t.d_t.minutes = timeinfo.tm_min;
        ctime->et_256.d_d_t.d_t.seconds = timeinfo.tm_sec;

        /* day of week */
        /* time gives day range of [0, 6], current_time_sevice
           has day range of [1,7] */
        ctime->et_256.d_d_t.day_of_week = timeinfo.tm_wday + 1;

        /* fractions_256 */
        ctime->et_256.fractions_256 = (((uint64_t)tv_now.tv_usec * 256L )/ 1000000L);

        ctime->adjust_reason = adjust_reason;
    }
    return 0;
}

int set_current_time(struct ble_svc_cts_curr_time ctime) {
    time_t now;
    struct tm timeinfo;
    struct timeval tv_now = {0};

    /* Basic sanity validation for incoming time fields */
    if (ctime.et_256.d_d_t.d_t.year < 1900 ||
        ctime.et_256.d_d_t.d_t.year > 9999 ||
        ctime.et_256.d_d_t.d_t.month < 1 || ctime.et_256.d_d_t.d_t.month > 12 ||
        ctime.et_256.d_d_t.d_t.day < 1 || ctime.et_256.d_d_t.d_t.day > 31 ||
        ctime.et_256.d_d_t.d_t.hours > 23 ||
        ctime.et_256.d_d_t.d_t.minutes > 59 ||
        ctime.et_256.d_d_t.d_t.seconds > 59 ||
        ctime.et_256.d_d_t.day_of_week < 1 || ctime.et_256.d_d_t.day_of_week > 7) {
        ESP_LOGE(TAG, "Invalid current time parameters");
        return -1;
    }

    /* fill date_time */
    timeinfo.tm_year= ctime.et_256.d_d_t.d_t.year - 1900 ;
    timeinfo.tm_mon = ctime.et_256.d_d_t.d_t.month - 1;
    timeinfo.tm_mday = ctime.et_256.d_d_t.d_t.day;
    timeinfo.tm_hour = ctime.et_256.d_d_t.d_t.hours;
    timeinfo.tm_min = ctime.et_256.d_d_t.d_t.minutes;
    timeinfo.tm_sec = ctime.et_256.d_d_t.d_t.seconds;
    timeinfo.tm_wday = ctime.et_256.d_d_t.day_of_week - 1;
    now = mktime(&timeinfo);
    if (now == (time_t)-1) {
        ESP_LOGE(TAG, "Failed to convert current time");
        return -1;
    }
    tv_now.tv_sec = now;
    tv_now.tv_usec = 0;
    settimeofday(&tv_now, NULL);
    /* set the last updated */
    gettimeofday(&last_updated, NULL);
    adjust_reason = ctime.adjust_reason;
    return 0;
}

int fetch_local_time_info(struct ble_svc_cts_local_time_info *info) {

    if(info != NULL) {
        memcpy(info, &local_info, sizeof local_info);
    }
    return 0;
}

int set_local_time_info(struct ble_svc_cts_local_time_info info) {
    /* just store the dst offset and timezone locally
        as we don't have the access to time using ntp server */
    local_info.timezone = info.timezone;
    local_info.dst_offset = info.dst_offset;
    gettimeofday(&last_updated, NULL);
    return 0;
}
int fetch_reference_time_info(struct ble_svc_cts_reference_time_info *info) {
    struct timeval tv_now;
    uint64_t days_since_update;
    uint64_t hours_since_update;

    gettimeofday(&tv_now, NULL);
    /* subtract the time when the last time was updated */
    tv_now.tv_sec -= last_updated.tv_sec; /* ignore microseconds */
    info->time_source = TIME_SOURCE_MANUAL;
    info->time_accuracy = 0;
    days_since_update = (tv_now.tv_sec / 86400L);
    hours_since_update = (tv_now.tv_sec / 3600);
    info->days_since_update = days_since_update < 255 ? days_since_update : 255;

    if(days_since_update > 254) {
        info->hours_since_update = 255;
    }
    else {
        hours_since_update = (tv_now.tv_sec % 86400L) / 3600;
        info->hours_since_update = hours_since_update;
    }
    adjust_reason = (CHANGE_OF_DST_MASK | CHANGE_OF_TIME_ZONE_MASK);

    return 0;
}
int
gatt_svr_init(void)
{
    struct ble_svc_cts_cfg cfg;

    ble_svc_gap_init();
    ble_svc_gatt_init();

    cfg.fetch_time_cb = fetch_current_time;
    cfg.local_time_info_cb = fetch_local_time_info;
    cfg.ref_time_info_cb = fetch_reference_time_info;
    cfg.set_time_cb = set_current_time;
    cfg.set_local_time_info_cb = set_local_time_info;
    ble_svc_cts_init(cfg);

    return 0;
}
