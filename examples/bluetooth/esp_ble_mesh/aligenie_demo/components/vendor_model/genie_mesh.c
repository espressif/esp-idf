// Copyright (C) 2018-2020 Alibaba Group Holding Limited
/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_random.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "genie_mesh.h"
#include "genie_model_srv.h"
#include "genie_event.h"
#include "genie_util.h"
#include "genie_timer.h"

static const char *TAG  = "genie_mesh";

extern esp_ble_mesh_elem_t elements[];
extern void set_silent_broadcast(void);

uint8_t g_indication_flag;

static util_timer_t g_pbadv_timer;
static util_timer_t g_prov_timer;
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
static util_timer_t g_indc_timer;
static util_timer_t g_time_sync_timer;
#endif

#ifdef CONFIG_MESH_MODEL_TRANS
void mesh_timer_stop(elem_state_t *p_elem)
{
    util_timer_stop(&p_elem->state.delay_timer);
    util_timer_stop(&p_elem->state.trans_timer);
}

static void mesh_delay_timer_cb(void *p_timer)
{
    elem_state_t *p_elem = (elem_state_t *)p_timer;

    mesh_timer_stop(p_elem);
    genie_event(GENIE_EVT_SDK_DELAY_END, p_timer);
}

void clear_trans_para(elem_state_t *p_elem)
{
    p_elem->state.trans            = 0;
    p_elem->state.trans_start_time = 0;
    p_elem->state.trans_end_time   = 0;
}

static void mesh_trans_timer_cycle(void *p_timer)
{
    elem_state_t  *p_elem  = (elem_state_t *)p_timer;
    model_state_t *p_state = &p_elem->state;

    mesh_timer_stop(p_elem);

    // do cycle
    genie_event(GENIE_EVT_SDK_TRANS_CYCLE, p_timer);
    // ESP_LOGI(TAG, ">>>>>%d %d", (uint32_t)cur_time, (uint32_t)p_elem->state.trans_end_time);

    if (p_state->trans == 0) {
        genie_event(GENIE_EVT_SDK_TRANS_END, p_timer);
    } else {
        util_timer_start(&p_state->trans_timer, GENIE_MESH_TRNSATION_CYCLE);
    }
}

uint8_t calc_cur_state(elem_state_t *p_elem)
{
    model_state_t *p_state = &p_elem->state;
    uint32_t      cur_time = k_uptime_get();
    uint8_t       cycle    = 0;

    // stop cycle when timeout
    if (cur_time <= p_state->trans_end_time - GENIE_MESH_TRNSATION_CYCLE) {
#if defined(CONFIG_MESH_MODEL_LIGHTNESS_SRV) || defined(CONFIG_MESH_MODEL_CTL_SRV)
        uint16_t step = (p_state->trans_end_time - cur_time) / GENIE_MESH_TRNSATION_CYCLE;
#endif

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        if (p_state->onoff[VALUE_TYPE_CUR] == 0 && p_state->onoff[VALUE_TYPE_TAR] == 1) {
            p_state->onoff[VALUE_TYPE_CUR] = 1;
        } else if (p_state->onoff[VALUE_TYPE_CUR] == 1 && p_state->onoff[VALUE_TYPE_TAR] == 0) {
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            // turn off when lightness is zero
            if (p_state->actual[VALUE_TYPE_CUR] == 0)
#endif
                p_state->onoff[VALUE_TYPE_CUR] = 0;
        }
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        if (p_state->actual[VALUE_TYPE_CUR] != p_state->actual[VALUE_TYPE_TAR]) {
            if (p_state->actual[VALUE_TYPE_TAR] > p_state->actual[VALUE_TYPE_CUR]) {
                p_state->actual[VALUE_TYPE_CUR] += (p_state->actual[VALUE_TYPE_TAR] - p_state->actual[VALUE_TYPE_CUR]) / step;
            } else {
                p_state->actual[VALUE_TYPE_CUR] -= (p_state->actual[VALUE_TYPE_CUR] - p_state->actual[VALUE_TYPE_TAR]) / step;
            }
            //model_bind_operation(B_LIGHTNESS_ID, p_elem, VALUE_TYPE_CUR);
            cycle = 1;
        }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        if (p_state->temp[VALUE_TYPE_CUR] != p_state->temp[VALUE_TYPE_TAR]) {
            if (p_state->temp[VALUE_TYPE_TAR] > p_state->temp[VALUE_TYPE_CUR]) {
                p_state->temp[VALUE_TYPE_CUR] += (p_state->temp[VALUE_TYPE_TAR] - p_state->temp[VALUE_TYPE_CUR]) / step;
            } else {
                p_state->temp[VALUE_TYPE_CUR] -= (p_state->temp[VALUE_TYPE_CUR] - p_state->temp[VALUE_TYPE_TAR]) / step;
            }
            cycle = 1;
        }
#endif
    }
#if 0
    ESP_LOGI(TAG, "next: %d->%d|%02x->%02x|%02x->%02x", p_state->onoff[VALUE_TYPE_CUR], p_state->onoff[VALUE_TYPE_TAR],
             p_state->actual[VALUE_TYPE_CUR], p_state->actual[VALUE_TYPE_TAR], p_state->temp[VALUE_TYPE_CUR], p_state->temp[VALUE_TYPE_TAR]);
#endif
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    if (p_state->onoff[VALUE_TYPE_CUR] == p_state->onoff[VALUE_TYPE_TAR])
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        if (p_state->actual[VALUE_TYPE_CUR] == p_state->actual[VALUE_TYPE_TAR])
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
            if (p_state->temp[VALUE_TYPE_CUR] == p_state->temp[VALUE_TYPE_TAR])
#endif
                cycle = 0;

    // ESP_LOGI(TAG, "cycle %d", cycle);
    if (cycle == 0) {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        // ESP_LOGI(TAG, "onoff %d->%d", p_state->onoff[VALUE_TYPE_CUR], p_state->onoff[VALUE_TYPE_TAR]);
        p_state->onoff[VALUE_TYPE_CUR] = p_state->onoff[VALUE_TYPE_TAR];
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        //ESP_LOGI(TAG, "actual %02x->%02x", p_state->actual[VALUE_TYPE_CUR], p_state->actual[VALUE_TYPE_TAR]);
        p_state->actual[VALUE_TYPE_CUR] = p_state->actual[VALUE_TYPE_TAR];
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        // ESP_LOGI(TAG, "temp %02x->%02x", p_state->temp[VALUE_TYPE_CUR], p_state->temp[VALUE_TYPE_TAR]);
        p_state->temp[VALUE_TYPE_CUR] = p_state->temp[VALUE_TYPE_TAR];
#endif
        p_state->trans = 0;
    }
    return cycle;
}
#endif

static void genie_pbadv_timer_cb(void *p_timer)
{
    ENTER_FUNC();
    genie_event(GENIE_EVT_SDK_MESH_PBADV_TIMEOUT, NULL);
}

void genie_pbadv_timer_start(void)
{
    ENTER_FUNC();
    static uint8_t inited = 0;

    if (!inited) {
        util_timer_init(&g_pbadv_timer, genie_pbadv_timer_cb, NULL);
        inited = 1;
    }
    util_timer_start(&g_pbadv_timer, GENIE_PBADV_TIMEOUT);
}

void genie_pbadv_timer_stop(void)
{
    ENTER_FUNC();
    util_timer_stop(&g_pbadv_timer);
}

void genie_pbadv_start_silent_adv(void)
{
    set_silent_broadcast();
    /* disable gatt adv */
    esp_ble_mesh_node_prov_disable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT);
    esp_ble_mesh_node_prov_enable(ESP_BLE_MESH_PROV_ADV);
    // todo: modify beacon interbal
}

static void genie_prov_timer_cb(void *p_timer)
{
    ENTER_FUNC();
    genie_event(GENIE_EVT_SDK_MESH_PROV_TIMEOUT, NULL);
}

void genie_prov_timer_start(void)
{
    ENTER_FUNC();
    static uint8_t inited = 0;

    if (!inited) {
        util_timer_init(&g_prov_timer, genie_prov_timer_cb, NULL);
        inited = 1;
    }
    util_timer_start(&g_prov_timer, GENIE_PROVISIONING_TIMEOUT);
}

void genie_prov_timer_stop(void)
{
    ENTER_FUNC();
    util_timer_stop(&g_prov_timer);
}

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
static void poweron_indicate_cb(void *p_timer)
{
    ENTER_FUNC();
    genie_prov_timer_stop();
    genie_event(GENIE_EVT_SDK_MESH_PWRON_INDC, NULL);
}

void poweron_indicate_start(void)
{
    ENTER_FUNC();
    static  uint8_t inited = 0;
    uint8_t random_time    = 0;

    if (!inited) {
        util_timer_init(&g_indc_timer, poweron_indicate_cb, NULL);
        inited = 1;
    }
    esp_fill_random(&random_time, 1);
#ifdef CONFIG_MESH_MODEL_TRANS
    random_time = 2000 + 8000 * random_time / 255;
#else
    random_time = 500 + 9500 * random_time / 255;
#endif
    ESP_LOGI(TAG, "power on indicate random: %d ms", random_time);
    util_timer_start(&g_indc_timer, random_time);
}

uint16_t genie_indicate_hw_reset_event(void)
{
    ENTER_FUNC();
    genie_model_msg_t reply_msg  = {0};
    uint8_t           payload[3] = {0};

    payload[0] = GENIE_MODEL_ATTR_DEVICE_EVENT & 0xff;
    payload[1] = (GENIE_MODEL_ATTR_DEVICE_EVENT >> 8) & 0xff;
    payload[2] = (uint8_t)EVENT_HW_RESET;

    reply_msg.opid         = GENIE_OP_ATTR_INDICATE;
    reply_msg.tid          = 0;
    reply_msg.data         = payload;
    reply_msg.len          = 3;
    reply_msg.p_elem       = &elements[0];
    reply_msg.retry_period = 120 + 300;
    reply_msg.retry        = GENIE_MODEL_MSG_MAX_RETRY_TIMES;

    genie_mesh_msg_send(&reply_msg);

    return 0;
}
#endif

#ifdef GENIE_MODEL_VENDOR_TIMER
static void genie_timer_operate_status(uint8_t tid, uint16_t attr_type)
{
    ENTER_FUNC();
    uint16_t          payload_len = 0;
    uint8_t           payload[10] = {0};
    genie_model_msg_t reply_msg   = {0};

    switch (attr_type) {
    case TIMER_ATTR_UNIX_TIME: {
        uint32_t unix_time = genie_timer_local_unixtime_get();
        payload_len = 2 + 4;
        payload[0] = TIMER_ATTR_UNIX_TIME & 0xff;
        payload[1] = (TIMER_ATTR_UNIX_TIME >> 8) & 0xff;
        payload[2] = (uint8_t)(unix_time & 0xFF);
        payload[3] = (uint8_t)((unix_time >> 8) & 0xFF);
        payload[4] = (uint8_t)((unix_time >> 16) & 0xFF);
        payload[5] = (uint8_t)((unix_time >> 24) & 0xFF);
    }
    break;
    case TIMER_ATTR_TIMEZONE_SETTING: {
        int8_t timezone = genie_timer_timezone_get();
        payload_len = 2 + 1;
        payload[0] = TIMER_ATTR_TIMEZONE_SETTING & 0xff;
        payload[1] = (TIMER_ATTR_TIMEZONE_SETTING >> 8) & 0xff;
        payload[2] = timezone;
    }
    break;
    case TIMER_ATTR_TIMING_SYNC: {
        uint16_t period_time = 0;
        uint8_t  retry_delay = 0;
        uint8_t  retry_times = 0;
        genie_timer_time_sync_get(&period_time, &retry_delay, &retry_times);
        payload_len = 2 + 4;
        payload[0] = TIMER_ATTR_TIMING_SYNC & 0xff;
        payload[1] = (TIMER_ATTR_TIMING_SYNC >> 8) & 0xff;
        payload[2] = period_time & 0xff;;
        payload[3] = (period_time >> 8) & 0xff;
        payload[4] = retry_delay;
        payload[5] = retry_times;
    }
    break;

    default:
        ESP_LOGW(TAG, "don't handle this attr_type");
        return;
    }
    reply_msg.opid         = GENIE_OP_ATTR_STATUS;
    reply_msg.tid          = tid;
    reply_msg.data         = payload;
    reply_msg.len          = payload_len;
    reply_msg.p_elem       = &elements[0];
    reply_msg.retry_period = 120 + 300;
    // reply_msg.retry        = 1;
    reply_msg.retry        = GENIE_MODEL_MSG_MAX_RETRY_TIMES;

    genie_mesh_msg_send(&reply_msg);
}

static void genie_timer_unixtime_indicate(void)
{
    ENTER_FUNC();
    genie_model_msg_t request_msg = {0};
    uint8_t           payload[2]  = {0};

    payload[0] = TIMER_ATTR_UNIX_TIME & 0xff;
    payload[1] = (TIMER_ATTR_UNIX_TIME >> 8) & 0xff;

    request_msg.opid         = GENIE_OP_ATTR_INDICATE_TG;
    request_msg.tid          = 0;
    request_msg.data         = payload;
    request_msg.len          = sizeof(payload);
    request_msg.p_elem       = &elements[0];
    request_msg.retry_period = 120 + 300;
    request_msg.retry        = GENIE_MODEL_MSG_MAX_RETRY_TIMES;

    genie_mesh_msg_send(&request_msg);
}

static void genie_timer_errcode_status(uint16_t attr_type, uint8_t err_code, uint8_t index, uint8_t tid)
{
    ENTER_FUNC();
    genie_model_msg_t reply_msg      = {0};
    uint8_t           payload[2 + 4] = {0};

    payload[0] = TIMER_ATTR_ERROR_CODE & 0xff;
    payload[1] = (TIMER_ATTR_ERROR_CODE >> 8) & 0xff;
    payload[2] = attr_type & 0xff;
    payload[3] = (attr_type >> 8) & 0xff;
    payload[4] = err_code;
    payload[5] = index;

    reply_msg.opid         = GENIE_OP_ATTR_STATUS;
    reply_msg.tid          = tid;
    reply_msg.data         = payload;
    reply_msg.len          = sizeof(payload);
    reply_msg.p_elem       = &elements[0];
    reply_msg.retry_period = 120 + 300;
    // reply_msg.retry        = 1;
    reply_msg.retry        = GENIE_MODEL_MSG_MAX_RETRY_TIMES;

    genie_mesh_msg_send(&reply_msg);

}

static void genie_timer_timing_setting_event(uint8_t op, uint8_t *msg, uint16_t msg_length, uint8_t tid)
{
    ENTER_FUNC();
    struct {
        uint32_t unix_time;
        uint8_t index;
        genie_timer_attr_data_t attr_data;
    } timing_setting_attr;

    uint8_t  *pmsg   = msg;
    uint16_t msg_len = msg_length;

    uint16_t attr_type = *pmsg++;
    attr_type += (*pmsg++ << 8);
    msg_len   -= 2;

    if (attr_type != TIMER_ATTR_TIMING_SETTING) {
        return;
    }

    if (op != GENIE_OP_ATTR_SET_ACK && op != GENIE_OP_ATTR_GET_STATUS) {
        return;
    }

    ESP_LOGI(TAG, "msg data: ");
    ESP_LOG_BUFFER_HEXDUMP(TAG, msg, msg_length, ESP_LOG_INFO);

    if (op == GENIE_OP_ATTR_SET_ACK) {
        while (msg_len > 0) {
            if (msg_len < sizeof(timing_setting_attr)) {
                break;
            }

            timing_setting_attr.index = *pmsg++;
            uint32_t unixtime_tmp = (pmsg[0]) | (pmsg[1] << 8) | (pmsg[2] << 16) | (pmsg[3] << 24);
            pmsg += 4;

            timing_setting_attr.unix_time = unixtime_tmp / 60 * 60;

            if (unixtime_tmp % 60 != 1) {
                //return genie_timer_errcode_status(TIMER_ATTR_TIMING_SETTING, ERR_CODE_NOTSUP_ATTR_OP,
                //                                  timing_setting_attr.index, tid);
            }

            timing_setting_attr.attr_data.type = (pmsg[0]) | (pmsg[1] << 8);
            pmsg += 2;

            if (timing_setting_attr.attr_data.type != GENIE_MODEL_ATTR_ONOFF) {
                return genie_timer_errcode_status(TIMER_ATTR_TIMING_SETTING, ERR_CODE_NOTSUP_ATTR_OP,
                                                    timing_setting_attr.index, tid);
            }

            timing_setting_attr.attr_data.para = *pmsg++;

            if (timing_setting_attr.attr_data.para > 1) {
                return genie_timer_errcode_status(TIMER_ATTR_TIMING_SETTING, ERR_CODE_NOTSUP_ATTR_PARAM,
                                                    timing_setting_attr.index, tid);
            }

            int ret = genie_timer_start(timing_setting_attr.index,
                                        timing_setting_attr.unix_time,
                                        &timing_setting_attr.attr_data);

            if (ret) {
                uint8_t errcode;

                if (ret == -GENIE_TIMER_ERR_INIT || ret == GENIE_TIMER_ERR_LOCALTIME_NOTSET) {
                    errcode = ERR_CODE_UNIXTIME;
                } else if (ret == -GENIE_TIMER_ERR_INDEX) {
                    errcode = ERR_CODE_TIMER_INDEX;
                } else if (ret == -GENIE_TIMER_ERR_NORESOURCE) {
                    errcode = ERR_CODE_TIMER_FULL;
                } else if (ret == -GENIE_TIMER_ERR_PARAM) {
                    errcode = ERR_CODE_TIMER_SETTING;
                } else {
                    errcode = ERR_CODE_UNIXTIME;
                }

                return genie_timer_errcode_status(TIMER_ATTR_TIMING_SETTING, errcode,
                                                    timing_setting_attr.index, tid);
            }

            msg_len -= sizeof(timing_setting_attr);
        }

    }

    genie_model_msg_t reply_msg = {0};

    reply_msg.opid         = GENIE_OP_ATTR_STATUS;
    reply_msg.tid          = tid;
    reply_msg.data         = msg;
    reply_msg.len          = msg_length;
    reply_msg.p_elem       = &elements[0];
    reply_msg.retry_period = 120 + 300;
    // reply_msg.retry        = 1;
    reply_msg.retry        = GENIE_MODEL_MSG_MAX_RETRY_TIMES;

    genie_mesh_msg_send(&reply_msg);
}

static void genie_timer_priordic_timing_setting_event(uint8_t op, uint8_t *msg, uint16_t msg_length, uint8_t tid)
{
    ENTER_FUNC();
    struct {
        uint8_t index;
        uint16_t prioridc_time;
        uint8_t  schedule;
        genie_timer_attr_data_t attr_data;
    } priordic_timing_attr;

    uint8_t  *pmsg   = msg;
    uint16_t msg_len = msg_length;

    uint16_t attr_type = *pmsg++;
    attr_type += (*pmsg++ << 8);
    msg_len   -= 2;

    if (attr_type != TIMER_ATTR_TIMING_PERIODIC_SETTING) {
        return;
    }

    if (op != GENIE_OP_ATTR_SET_ACK && op != GENIE_OP_ATTR_GET_STATUS) {
        return;
    }

    if (op == GENIE_OP_ATTR_SET_ACK) {
        while (msg_len > 0) {
            if (msg_len < 7) { // sizeof(priordic_timing_attr) by bytes
                break;
            }

            priordic_timing_attr.index = *pmsg++;
            uint32_t priordic_tmp = (pmsg[0]) | (pmsg[1] << 8);
            pmsg += 2;

            priordic_timing_attr.prioridc_time = priordic_tmp & 0x0FFF;

            if (((priordic_tmp >> 12) & 0x0F) != 1) {
                //return genie_timer_errcode_status(TIMER_ATTR_TIMING_PERIODIC_SETTING, ERR_CODE_NOTSUP_ATTR_OP,
                //                                  priordic_timing_attr.index, tid);
            }

            priordic_timing_attr.schedule = *pmsg++;

            if (priordic_timing_attr.schedule > 0x7F) {
                return genie_timer_errcode_status(TIMER_ATTR_TIMING_PERIODIC_SETTING, ERR_CODE_TIMER_PRIORDIC_PARAM,
                                                    priordic_timing_attr.index, tid);
            }

            priordic_timing_attr.attr_data.type = (pmsg[0]) | (pmsg[1] << 8);
            pmsg += 2;

            if (priordic_timing_attr.attr_data.type != GENIE_MODEL_ATTR_ONOFF) {
                return genie_timer_errcode_status(TIMER_ATTR_TIMING_PERIODIC_SETTING, ERR_CODE_NOTSUP_ATTR_OP,
                                                    priordic_timing_attr.index, tid);
            }

            priordic_timing_attr.attr_data.para = *pmsg++;

            if (priordic_timing_attr.attr_data.para > 1) {
                return genie_timer_errcode_status(TIMER_ATTR_TIMING_PERIODIC_SETTING, ERR_CODE_NOTSUP_ATTR_PARAM,
                                                    priordic_timing_attr.index, tid);
            }

            int ret = genie_timer_periodic_start(priordic_timing_attr.index,
                                                 priordic_timing_attr.prioridc_time * 60,
                                                 priordic_timing_attr.schedule,
                                                 &priordic_timing_attr.attr_data);

            if (ret) {
                uint8_t errcode = 0;

                if (ret == -GENIE_TIMER_ERR_INIT || ret == GENIE_TIMER_ERR_LOCALTIME_NOTSET) {
                    errcode = ERR_CODE_UNIXTIME;
                } else if (ret == -GENIE_TIMER_ERR_INDEX) {
                    errcode = ERR_CODE_TIMER_INDEX;
                } else if (ret == -GENIE_TIMER_ERR_NORESOURCE) {
                    errcode = ERR_CODE_TIMER_FULL;
                } else if (ret == -GENIE_TIMER_ERR_PARAM) {
                    errcode = ERR_CODE_TIMER_SETTING;
                } else {
                    errcode = ERR_CODE_TIMER_PRIORDIC_PARAM;
                }

                return genie_timer_errcode_status(TIMER_ATTR_TIMING_PERIODIC_SETTING, errcode,
                                                    priordic_timing_attr.index, tid);
            }

            msg_len -= 7; // sizeof(priordic_timing_attr) by bytes
        }

    }

    genie_model_msg_t reply_msg = {0};

    reply_msg.opid         = GENIE_OP_ATTR_STATUS;
    reply_msg.tid          = tid;
    reply_msg.data         = msg;
    reply_msg.len          = msg_length;
    reply_msg.p_elem       = &elements[0];
    reply_msg.retry_period = 120 + 300;
    // reply_msg.retry        = 1;
    reply_msg.retry        = GENIE_MODEL_MSG_MAX_RETRY_TIMES;

    genie_mesh_msg_send(&reply_msg);
}

static void genie_timer_timing_remove_event(uint8_t op, uint8_t *msg, uint16_t msg_length, uint8_t tid)
{
    ENTER_FUNC();
    uint8_t  *pmsg   = msg;
    uint16_t msg_len = msg_length;

    uint16_t attr_type = *pmsg++;
    attr_type += (*pmsg++ << 8);
    msg_len   -= 2;

    if (attr_type != TIMER_ATTR_TIMING_DELETE) {
        return;
    }

    if (op != GENIE_OP_ATTR_SET_ACK) {
        return;
    }

    // if (op == GENIE_OP_ATTR_SET_ACK)
    while (msg_len > 0) {
        uint8_t index = *pmsg++;
        msg_len--;

        int ret = genie_timer_remove(index);

        if (ret) {
            //return genie_timer_errcode_status(TIMER_ATTR_TIMING_DELETE, ERR_CODE_TIMER_INDEX, index, tid);
        }
    }

    genie_model_msg_t reply_msg = {0};

    reply_msg.opid         = GENIE_OP_ATTR_STATUS;
    reply_msg.tid          = tid;
    reply_msg.data         = msg;
    reply_msg.len          = msg_length;
    reply_msg.p_elem       = &elements[0];
    reply_msg.retry_period = 120 + 300;
    // reply_msg.retry        = 1;
    reply_msg.retry        = GENIE_MODEL_MSG_MAX_RETRY_TIMES;

    genie_mesh_msg_send(&reply_msg);
}

static void genie_timer_timeout_indicate(uint8_t index)
{
    ENTER_FUNC();
    genie_model_msg_t request_msg    = {0};
    uint8_t           payload[2 + 2] = {0};

    payload[0] = TIMER_ATTR_TIMING_TIMEOUT & 0xff;
    payload[1] = (TIMER_ATTR_TIMING_TIMEOUT >> 8) & 0xff;
    payload[2] = EVENT_TIMING_TIMEOUT;
    payload[3] = index;

    request_msg.opid         = GENIE_OP_ATTR_INDICATE;
    request_msg.tid          = 0;
    request_msg.data         = payload;
    request_msg.len          = sizeof(payload);
    request_msg.p_elem       = &elements[0];
    request_msg.retry_period = 120 + 300;
    request_msg.retry        = GENIE_MODEL_MSG_MAX_RETRY_TIMES;

    genie_mesh_msg_send(&request_msg);
}
#endif

uint16_t genie_vendor_model_msg_handle(genie_model_msg_t *p_msg)
{
    ENTER_FUNC();
    uint8_t *p_data = NULL;

    if (!p_msg || !p_msg->data) {
        return -1;
    }
    p_data = p_msg->data;
    ESP_LOGI(TAG, "opcode:0x%x, tid:%d, len:%d", p_msg->opid, p_msg->tid, p_msg->len);
    if (p_data && p_msg->len) {
        ESP_LOGI(TAG, "payload: %s", util_hex2str(p_data, p_msg->len));
    }

    switch (p_msg->opid) {
    case GENIE_OP_ATTR_INDICATE: {
        //uint16_t attr_type = *p_data++ + (*p_data++ << 8);
        uint16_t attr_type = *p_data++;
        attr_type += (*p_data++ << 8);
        ESP_LOGI(TAG, "attr_type:0x%x", attr_type);
        if (attr_type == GENIE_MODEL_ATTR_DEVICE_EVENT) {
            uint8_t event_id = *p_data;
            switch (event_id) {
            case EVENT_DEV_UP:
                poweron_indicate_start(); // When receiving genie's device up status, indication device's state and device up event in sequence
                break;
            default:
                break;
            }
        }

        break;
    }
    case GENIE_OP_ATTR_GET_STATUS: {
        uint16_t attr_type = *p_data++;
        attr_type += (*p_data++ << 8);
        ESP_LOGI(TAG, "attr_type:0x%x", attr_type);
#ifdef GENIE_MODEL_VENDOR_TIMER
        if (attr_type == TIMER_ATTR_UNIX_TIME ||
                attr_type == TIMER_ATTR_TIMEZONE_SETTING ||
                attr_type == TIMER_ATTR_TIMING_SYNC) {
            genie_timer_operate_status(p_msg->tid, attr_type);
        } else if (attr_type == TIMER_ATTR_TIMING_SETTING) {
            genie_timer_timing_setting_event(GENIE_OP_ATTR_GET_STATUS, p_msg->data, p_msg->len, p_msg->tid);
        } else if (attr_type == TIMER_ATTR_TIMING_PERIODIC_SETTING) {
            genie_timer_priordic_timing_setting_event(GENIE_OP_ATTR_GET_STATUS, p_msg->data, p_msg->len, p_msg->tid);
        }
#endif
        break;
    }

    case GENIE_OP_ATTR_SET_ACK: {
        uint16_t attr_type = *p_data++;
        attr_type += (*p_data++ << 8);
        ESP_LOGI(TAG, "attr_type: 0x%04x", attr_type);
        if (attr_type == GENIE_MODEL_ATTR_COLOR) {
            genie_event(GENIE_EVT_SDK_COLOR_ACTION, (void *)p_data);
        }
#ifdef GENIE_MODEL_VENDOR_TIMER
        if (attr_type == TIMER_ATTR_UNIX_TIME) {
            uint32_t unix_time = (p_data[0]) | (p_data[1] << 8) | (p_data[2] << 16) | (p_data[3] << 24);
            p_data += 4;
            genie_timer_local_time_update(unix_time);
            genie_timer_operate_status(p_msg->tid, attr_type);
        } else if (attr_type == TIMER_ATTR_TIMEZONE_SETTING) {
            int8_t timezone = *p_data++;
            genie_timer_timezone_update(timezone);
            genie_timer_operate_status(p_msg->tid, attr_type);
        } else if (attr_type == TIMER_ATTR_TIMING_SYNC) {
            uint16_t period_time = (p_data[0]) | (p_data[1] << 8);
            p_data += 2;
            uint8_t retry_delay = *p_data++;
            uint8_t retry_times = *p_data++;
            genie_timer_time_sync_set(period_time, retry_delay, retry_times);
            genie_timer_operate_status(p_msg->tid, attr_type);
        } else if (attr_type == TIMER_ATTR_TIMING_SETTING) {
            genie_timer_timing_setting_event(GENIE_OP_ATTR_SET_ACK, p_msg->data, p_msg->len, p_msg->tid);
        } else if (attr_type == TIMER_ATTR_TIMING_PERIODIC_SETTING) {
            genie_timer_priordic_timing_setting_event(GENIE_OP_ATTR_SET_ACK, p_msg->data, p_msg->len, p_msg->tid);
        } else if (attr_type == TIMER_ATTR_TIMING_DELETE) {
            genie_timer_timing_remove_event(GENIE_OP_ATTR_SET_ACK, p_msg->data, p_msg->len, p_msg->tid);
        }
#endif
        break;
    }

    case GENIE_OP_ATTR_CONFIME_TG: {
        uint16_t attr_type = *p_data++;
        attr_type += (*p_data++ << 8);
        ESP_LOGI(TAG, "attr_type: 0x%x", attr_type);
#ifdef GENIE_MODEL_VENDOR_TIMER
        if (attr_type == TIMER_ATTR_UNIX_TIME) {
            uint32_t unix_time = (p_data[0]) | (p_data[1] << 8) | (p_data[2] << 16) | (p_data[3] << 24);
            p_data += 4;
            genie_timer_local_time_update(unix_time);
        }
#endif
        break;
    }

    default:
        break;
    }

    return 0;
}

uint8_t get_seg_count(uint16_t msg_len)
{
    ENTER_FUNC();
    if (msg_len <= 11) {
        return 1;
    }

    msg_len -= 8;
    if (msg_len % 12) {
        return msg_len / 12 + 2;
    }

    return msg_len / 12 + 1;
}

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
static int genie_timer_event(uint8_t event, uint8_t index, genie_timer_attr_data_t *data);

static void time_sync_request_cb(void *p_timer)
{
    ENTER_FUNC();
    genie_timer_event(GENIE_TIME_EVT_TIMING_SYNC, 0, NULL);
}

void time_sync_request_start(void)
{
    ENTER_FUNC();
    static   uint8_t inited = 0;
    uint16_t random_time    = 0;

    if (!inited) {
        util_timer_init(&g_time_sync_timer, time_sync_request_cb, NULL);
        inited = 1;
    }
    esp_fill_random(&random_time, 1);
#ifdef CONFIG_MESH_MODEL_TRANS
    random_time = 10000 + 5000 * random_time / 255;
#else
    random_time = 5000 + 10000 * random_time / 255;
#endif
    ESP_LOGD(TAG, "time sync indicate random: %d ms", random_time);
    util_timer_start(&g_time_sync_timer, random_time);
}

void genie_standart_indication(elem_state_t *p_elem)
{
    ENTER_FUNC();
    uint16_t i                   = 0;
    uint8_t  seg_count           = 0;
    uint8_t  buff[22]            = {0};
    uint8_t  cur_indication_flag = g_indication_flag;

    ESP_LOGD(TAG, "start flag %02x", g_indication_flag);

    if (cur_indication_flag & INDICATION_FLAG_POWERON) {
        g_indication_flag &= ~INDICATION_FLAG_POWERON;
        // the device will indication all states when powerup
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        cur_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        cur_indication_flag |= INDICATION_FLAG_LIGHTNESS;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        cur_indication_flag |= INDICATION_FLAG_CTL;
#endif
#ifdef CONFIG_MESH_MODEL_HSL_SRV
        cur_indication_flag |= INDICATION_FLAG_HSL;
#endif
    } else {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        if (g_indication_flag & INDICATION_FLAG_ONOFF) {
            g_indication_flag &= ~INDICATION_FLAG_ONOFF;
        }
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        if (g_indication_flag & INDICATION_FLAG_LIGHTNESS) {
            g_indication_flag &= ~INDICATION_FLAG_LIGHTNESS;
        }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        if (g_indication_flag & INDICATION_FLAG_CTL) {
            g_indication_flag &= ~INDICATION_FLAG_CTL;
        }
#endif
#ifdef CONFIG_MESH_MODEL_HSL_SRV
        if (g_indication_flag & INDICATION_FLAG_HSL) {
            g_indication_flag &= ~INDICATION_FLAG_HSL;
        }
#endif
    }
    ESP_LOGD(TAG, "real flag %02x", cur_indication_flag);

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    if (cur_indication_flag & INDICATION_FLAG_ONOFF) {
        buff[i++] = GENIE_MODEL_ATTR_ONOFF & 0xff;
        buff[i++] = (GENIE_MODEL_ATTR_ONOFF >> 8) & 0xff;
        buff[i++] = p_elem->state.onoff[VALUE_TYPE_CUR];
    }
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    if (cur_indication_flag & INDICATION_FLAG_LIGHTNESS) {
        buff[i++] = GENIE_MODEL_ATTR_LIGHTNESS & 0xff;
        buff[i++] = (GENIE_MODEL_ATTR_LIGHTNESS >> 8) & 0xff;
        buff[i++] = p_elem->state.actual[VALUE_TYPE_CUR] & 0xff;
        buff[i++] = (p_elem->state.actual[VALUE_TYPE_CUR] >> 8) & 0xff;
    }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    if (cur_indication_flag & INDICATION_FLAG_CTL) {
        buff[i++] = GENIE_MODEL_ATTR_TEMPERATURE & 0xff;
        buff[i++] = (GENIE_MODEL_ATTR_TEMPERATURE >> 8) & 0xff;
        buff[i++] = p_elem->state.temp[VALUE_TYPE_CUR] & 0xff;
        buff[i++] = (p_elem->state.temp[VALUE_TYPE_CUR] >> 8) & 0xff;
    }
#endif
#ifdef CONFIG_MESH_MODEL_HSL_SRV
    if (cur_indication_flag & INDICATION_FLAG_HSL) {
        buff[i++] = GENIE_MODEL_ATTR_COLOR & 0xff;
        buff[i++] = (GENIE_MODEL_ATTR_COLOR >> 8) & 0xff;
        buff[i++] = p_elem->state.lightness[VALUE_TYPE_CUR] & 0xff;
        buff[i++] = (p_elem->state.lightness[VALUE_TYPE_CUR] >> 8) & 0xff;
        buff[i++] = p_elem->state.hue[VALUE_TYPE_CUR] & 0xff;
        buff[i++] = (p_elem->state.hue[VALUE_TYPE_CUR] >> 8) & 0xff;
        buff[i++] = p_elem->state.saturation[VALUE_TYPE_CUR] & 0xff;
        buff[i++] = (p_elem->state.saturation[VALUE_TYPE_CUR] >> 8) & 0xff;
    }
#endif
    if (cur_indication_flag & INDICATION_FLAG_POWERON) {
        buff[i++] = GENIE_MODEL_ATTR_DEVICE_EVENT & 0xff;
        buff[i++] = (GENIE_MODEL_ATTR_DEVICE_EVENT >> 8) & 0xff;
        buff[i++] = EVENT_DEV_UP;
        cur_indication_flag &= ~INDICATION_FLAG_POWERON;
        // 1. request to sync time with random delay
        time_sync_request_start();
    }
    ESP_LOGD(TAG, "end flag %02x", g_indication_flag);

    if (i) {
        genie_model_msg_t reply_msg = {0};
        seg_count = get_seg_count(i + 4);

        reply_msg.opid         = GENIE_OP_ATTR_INDICATE;
        reply_msg.tid          = 0;
        reply_msg.data         = buff;
        reply_msg.len          = i;
        reply_msg.p_elem       = &elements[p_elem->elem_index];
        reply_msg.retry_period = 120 * seg_count + 400;
        if (seg_count > 1) {
            reply_msg.retry_period *= 2;
        }
        reply_msg.retry = GENIE_MODEL_MSG_MAX_RETRY_TIMES;

        genie_mesh_msg_send(&reply_msg);
    }
}
#endif

int16_t genie_mesh_msg_send(genie_model_msg_t *p_vendor_msg)
{
    ENTER_FUNC();
    int16_t r    = -1;
    uint8_t opid = 0;

    if (!p_vendor_msg) {
        return r;
    }

    // vnedor confirm message contains 0 extra data except opid
    if (!p_vendor_msg->data && !p_vendor_msg->len) {
        return r;
    }

    opid = p_vendor_msg->opid;
    ESP_LOGD(TAG, "VND msg send: opcode:0x%x, tid:%02x, len:%d", opid, p_vendor_msg->tid, p_vendor_msg->len);
    ESP_LOGD(TAG, "payload: %s", p_vendor_msg->len ? util_hex2str(p_vendor_msg->data, p_vendor_msg->len) : "empty");

    switch (opid) {
    case GENIE_OP_ATTR_STATUS:
    case GENIE_OP_ATTR_INDICATE:
    case GENIE_OP_ATTR_INDICATE_TG:
    case GENIE_OP_ATTR_TRANS_MSG:
        genie_model_msg_send(p_vendor_msg);
        break;
    default:
        ESP_LOGW(TAG, "unknown opid:0x%x", opid);
        break;
    }
    return 0;
}

uint8_t elem_state_init(uint8_t state_count, elem_state_t *p_elem)
{
    ENTER_FUNC();
    uint8_t i = 0;

    while (i < state_count) {
        p_elem[i].elem_index = i;
#ifdef CONFIG_MESH_MODEL_TRANS
        // memcpy(&elem[i].powerup, &f_power_up[i], sizeof(model_powerup_t));
        util_timer_init(&p_elem[i].state.delay_timer, mesh_delay_timer_cb, &p_elem[i]);
        util_timer_init(&p_elem[i].state.trans_timer, mesh_trans_timer_cycle, &p_elem[i]);
#endif
#ifdef CONFIG_MESH_SIMPLE_MODLE
        p_elem[i].state.onoff[VALUE_TYPE_TAR] = GENIE_ONOFF_DEFAULT;
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        p_elem[i].state.actual[VALUE_TYPE_TAR] = GENIE_LIGHTNESS_DEFAULT;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        p_elem[i].state.temp[VALUE_TYPE_TAR] = GENIE_CTL_TEMP_DEFAULT;
#endif
#ifdef CONFIG_MESH_MODEL_TRANS
        p_elem[i].state.trans = 0x41;
        p_elem[i].state.delay = 100;
        if (p_elem[i].state.trans) {
            p_elem[i].state.trans_start_time = k_uptime_get() + p_elem[i].state.delay * 5;
            p_elem[i].state.trans_end_time = p_elem[i].state.trans_start_time + get_transition_time(p_elem[i].state.trans);
        }
#endif
#endif
        i++;
    }

    return 0;
}

#ifdef CONFIG_MESH_MODEL_TRANS  // transation api
uint16_t TRANS_TIMES[] = {1, 10, 100, 6000};

static uint8_t get_transition_byte(uint32_t time)
{
    ENTER_FUNC();
    ESP_LOGI(TAG, "time: %d", time);

    time /= 100;

    if (time > TRANS_TIMES[3] * 62) {
        return 0;
    } else if (time > TRANS_TIMES[2] * 62) {
        return (time / TRANS_TIMES[3]) | 0xC0;
    } else if (time > TRANS_TIMES[1] * 62) {
        return (time / TRANS_TIMES[2]) | 0x80;
    } else if (time > TRANS_TIMES[0] * 62) {
        return (time / TRANS_TIMES[1]) | 0x40;
    } else {
        return (time / TRANS_TIMES[0]);
    }
}

// unit is 1ms
uint32_t get_transition_time(uint8_t byte)
{
    ENTER_FUNC();

    if ((byte & 0x3F) == 0x3F) {
        ESP_LOGE(TAG, "%s ERROR, invalid 0x%02X!!!", __func__, byte);
        return 0xFFFFFFFF;
    }

    return (byte & 0x3F) * TRANS_TIMES[byte >> 6] * 100;
}

uint8_t get_remain_byte(model_state_t *p_state, bool is_ack)
{
    ENTER_FUNC();
    uint8_t  remain_byte = p_state->trans;
    uint32_t cur_time    = k_uptime_get();

    if (!is_ack && p_state->trans_start_time < cur_time) {
        cur_time -= p_state->trans_start_time;
        uint32_t l_trans = get_transition_time(p_state->trans);
        if (l_trans == 0xFFFFFFFF) {
            remain_byte = 0x3F;
        } else if (l_trans > cur_time) {
            remain_byte = get_transition_byte(l_trans - cur_time);
        } else {
            remain_byte = 0;
        }

    }

    ESP_LOGI(TAG, "remain_byte: 0x%02x", remain_byte);

    return remain_byte;
}
#endif

#define RECV_MSG_TID_QUEUE_SIZE 5

#define TMALL_GENIE_UADDR_START 0x0001
#define TMALL_GENIE_UADDR_END   0x0010

typedef struct {
    uint8_t tid;
    uint16_t addr;
    uint32_t time;
} _TID_QUEUE_S;

_TID_QUEUE_S tid_queue[RECV_MSG_TID_QUEUE_SIZE];

genie_error_type_t genie_msg_check_tid(uint16_t src_addr, uint8_t tid)
{
    ENTER_FUNC();
    static   uint8_t cur_index = 0;
    uint8_t  i                 = cur_index;
    uint8_t  ri                = 0;
    uint32_t cur_time          = k_uptime_get();
    uint32_t end_time          = 0;

    if (src_addr >= TMALL_GENIE_UADDR_START && src_addr <= TMALL_GENIE_UADDR_END) {
        src_addr = TMALL_GENIE_UADDR_START;
    }

    while (i < cur_index + RECV_MSG_TID_QUEUE_SIZE) {
        ri = i % RECV_MSG_TID_QUEUE_SIZE;
        if (tid_queue[ri].tid == tid && tid_queue[ri].addr == src_addr) {
            end_time = tid_queue[ri].time + 6000;
            if (cur_time < end_time) {
                break;
            }
            // ESP_LOGI(TAG, "---- cur(%lld) last(%lld)", cur_time, end_time);
        }
        i++;
    }
    if (i < cur_index + RECV_MSG_TID_QUEUE_SIZE) {
        return GENIE_MESH_TID_REPEAT_ERROR;
    } else {
        tid_queue[cur_index].tid  = tid;
        tid_queue[cur_index].addr = src_addr;
        tid_queue[cur_index].time = cur_time;
        cur_index++;
        cur_index %= RECV_MSG_TID_QUEUE_SIZE;
        return GENIE_MESH_SUCCESS;
    }
}

#ifdef GENIE_MODEL_VENDOR_TIMER
static int genie_timer_event(uint8_t event, uint8_t index, genie_timer_attr_data_t *data)
{
    ENTER_FUNC();
    if (event == GENIE_TIME_EVT_TIMEOUT) {
        ESP_LOGD(TAG, "timer index %d timeout", index);
        genie_timer_local_time_show();
        genie_event(GENIE_EVT_TIME_OUT, data);
        genie_timer_timeout_indicate(index);
    } else if (event == GENIE_TIME_EVT_TIMING_SYNC) {
        ESP_LOGD(TAG, "timer timing sync");
        genie_timer_unixtime_indicate();
    }

    return 0;
}
#endif

void genie_mesh_init(void)
{
    ENTER_FUNC();
    ESP_LOGI(TAG, "Initializing genie mesh...");

#ifdef GENIE_MODEL_VENDOR_TIMER
    genie_timer_init(genie_timer_event);
#endif
}
