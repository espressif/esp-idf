/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "btc_pan.h"
#include "btc/btc_manage.h"
#include "btc/btc_task.h"
#include "bta/bta_pan_api.h"
#include "bta/bta_pan_ci.h"
#include "common/bt_trace.h"
#include "osi/allocator.h"
#include "esp_pan_api.h"

#if (defined BTC_PAN_INCLUDED && BTC_PAN_INCLUDED == TRUE)

static bool s_btc_pan_init = false;
static esp_pan_cfg_t s_btc_pan_cfg;

static esp_pan_status_t btc_pan_status_from_bta(tBTA_PAN_STATUS status)
{
    return (status == BTA_PAN_SUCCESS) ? ESP_PAN_SUCCESS : ESP_PAN_FAILURE;
}

static void btc_pan_cb_to_app(esp_pan_cb_event_t event, esp_pan_cb_param_t *param)
{
    esp_pan_cb_t callback = (esp_pan_cb_t)btc_profile_cb_get(BTC_PID_PAN);
    if (callback) {
        callback(event, param);
    }
}

static void bte_pan_evt(tBTA_PAN_EVT event, tBTA_PAN *p_data)
{
    btc_msg_t msg;
    int param_len = 0;

    switch (event) {
    case BTA_PAN_ENABLE_EVT:
    case BTA_PAN_SET_ROLE_EVT:
        param_len = (event == BTA_PAN_SET_ROLE_EVT) ? sizeof(tBTA_PAN_SET_ROLE) : 0;
        break;
    case BTA_PAN_OPENING_EVT:
        param_len = sizeof(tBTA_PAN_OPENING);
        break;
    case BTA_PAN_OPEN_EVT:
        param_len = sizeof(tBTA_PAN_OPEN);
        break;
    case BTA_PAN_CLOSE_EVT:
        param_len = sizeof(tBTA_PAN_CLOSE);
        break;
    default:
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PAN;
    msg.act = event;

    if (btc_transfer_context(&msg, p_data, param_len, NULL, NULL) != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s: context transfer failed", __func__);
    }
}

static void btc_pan_set_role(void)
{
    tBTA_PAN_ROLE_INFO user_info = {0};
    tBTA_PAN_ROLE_INFO gn_info = {0};
    tBTA_PAN_ROLE_INFO nap_info = {0};

    if (s_btc_pan_cfg.role & ESP_PAN_ROLE_PANU) {
        user_info.p_srv_name = s_btc_pan_cfg.panu_service_name;
        user_info.app_id = BTC_PAN_APP_ID_PANU;
        user_info.sec_mask = s_btc_pan_cfg.panu_sec;
    }

    if (s_btc_pan_cfg.role & ESP_PAN_ROLE_GN) {
        gn_info.p_srv_name = s_btc_pan_cfg.gn_service_name;
        gn_info.app_id = BTC_PAN_APP_ID_GN;
        gn_info.sec_mask = s_btc_pan_cfg.gn_sec;
    }

    if (s_btc_pan_cfg.role & ESP_PAN_ROLE_NAP) {
        nap_info.p_srv_name = s_btc_pan_cfg.nap_service_name;
        nap_info.app_id = BTC_PAN_APP_ID_NAP;
        nap_info.sec_mask = s_btc_pan_cfg.nap_sec;
    }

    BTA_PanSetRole((tBTA_PAN_ROLE)s_btc_pan_cfg.role, &user_info, &gn_info, &nap_info);
}

static void btc_pan_free_cfg_strings(void)
{
    if (s_btc_pan_cfg.panu_service_name) {
        osi_free((void *)s_btc_pan_cfg.panu_service_name);
        s_btc_pan_cfg.panu_service_name = NULL;
    }
    if (s_btc_pan_cfg.gn_service_name) {
        osi_free((void *)s_btc_pan_cfg.gn_service_name);
        s_btc_pan_cfg.gn_service_name = NULL;
    }
    if (s_btc_pan_cfg.nap_service_name) {
        osi_free((void *)s_btc_pan_cfg.nap_service_name);
        s_btc_pan_cfg.nap_service_name = NULL;
    }
}

static void btc_pan_store_cfg(const esp_pan_cfg_t *cfg)
{
    btc_pan_free_cfg_strings();
    s_btc_pan_cfg = *cfg;

    if (cfg->panu_service_name) {
        size_t len = strlen(cfg->panu_service_name) + 1;
        char *name = (char *)osi_malloc(len);
        if (name) {
            memcpy(name, cfg->panu_service_name, len);
            s_btc_pan_cfg.panu_service_name = name;
        } else {
            s_btc_pan_cfg.panu_service_name = NULL;
        }
    }
    if (cfg->gn_service_name) {
        size_t len = strlen(cfg->gn_service_name) + 1;
        char *name = (char *)osi_malloc(len);
        if (name) {
            memcpy(name, cfg->gn_service_name, len);
            s_btc_pan_cfg.gn_service_name = name;
        } else {
            s_btc_pan_cfg.gn_service_name = NULL;
        }
    }
    if (cfg->nap_service_name) {
        size_t len = strlen(cfg->nap_service_name) + 1;
        char *name = (char *)osi_malloc(len);
        if (name) {
            memcpy(name, cfg->nap_service_name, len);
            s_btc_pan_cfg.nap_service_name = name;
        } else {
            s_btc_pan_cfg.nap_service_name = NULL;
        }
    }
}

static void btc_pan_init(const esp_pan_cfg_t *cfg)
{
    if (s_btc_pan_init) {
        esp_pan_cb_param_t param = {
            .init.status = ESP_PAN_NEED_DEINIT,
        };
        btc_pan_cb_to_app(ESP_PAN_INIT_EVT, &param);
        return;
    }

    if (cfg == NULL || cfg->role == 0) {
        esp_pan_cb_param_t param = {
            .init.status = ESP_PAN_FAILURE,
        };
        btc_pan_cb_to_app(ESP_PAN_INIT_EVT, &param);
        return;
    }

    btc_pan_store_cfg(cfg);
    s_btc_pan_init = true;
    BTA_PanEnable(bte_pan_evt);
}

static void btc_pan_deinit(void)
{
    if (!s_btc_pan_init) {
        esp_pan_cb_param_t param = {
            .deinit.status = ESP_PAN_NEED_INIT,
        };
        btc_pan_cb_to_app(ESP_PAN_DEINIT_EVT, &param);
        return;
    }

    s_btc_pan_init = false;
    BTA_PanDisable();
    btc_pan_free_cfg_strings();
    memset(&s_btc_pan_cfg, 0, sizeof(s_btc_pan_cfg));

    esp_pan_cb_param_t param = {
        .deinit.status = ESP_PAN_SUCCESS,
    };
    btc_pan_cb_to_app(ESP_PAN_DEINIT_EVT, &param);
}

static void btc_pan_connect(btc_pan_args_t *arg)
{
    if (!s_btc_pan_init) {
        esp_pan_cb_param_t param = {
            .open.status = ESP_PAN_NEED_INIT,
        };
        memcpy(param.open.remote_bda, arg->connect.bd_addr, ESP_BD_ADDR_LEN);
        btc_pan_cb_to_app(ESP_PAN_OPEN_EVT, &param);
        return;
    }

    BTA_PanOpen(arg->connect.bd_addr, (tBTA_PAN_ROLE)arg->connect.local_role,
                (tBTA_PAN_ROLE)arg->connect.peer_role);
}

static void btc_pan_disconnect(uint16_t handle)
{
    if (!s_btc_pan_init) {
        return;
    }

    BTA_PanClose(handle);
}

static void btc_pan_write(btc_pan_args_t *arg)
{
    esp_pan_cb_param_t param = {
        .write.handle = arg->write.handle,
        .write.status = ESP_PAN_SUCCESS,
    };

    if (!s_btc_pan_init) {
        param.write.status = ESP_PAN_NEED_INIT;
        btc_pan_cb_to_app(ESP_PAN_WRITE_EVT, &param);
        return;
    }

    if (arg->write.data == NULL || arg->write.len == 0 || arg->write.len > PAN_MAX_WRITE_LEN) {
        param.write.status = ESP_PAN_FAILURE;
        btc_pan_cb_to_app(ESP_PAN_WRITE_EVT, &param);
        return;
    }

    bta_pan_ci_rx_write(arg->write.handle, arg->write.dst, arg->write.src,
                        arg->write.protocol, arg->write.data, arg->write.len,
                        arg->write.ext);
    btc_pan_cb_to_app(ESP_PAN_WRITE_EVT, &param);
}

static void btc_pan_set_protocol_filters(btc_pan_args_t *arg)
{
    if (!s_btc_pan_init ||
            (arg->set_pfilter.num_filters > 0 &&
            (arg->set_pfilter.start_array == NULL || arg->set_pfilter.end_array == NULL))) {
        return;
    }

    bta_pan_ci_set_pfilters(arg->set_pfilter.handle, arg->set_pfilter.num_filters,
                            arg->set_pfilter.start_array, arg->set_pfilter.end_array);
}

static void btc_pan_set_multicast_filters(btc_pan_args_t *arg)
{
    if (!s_btc_pan_init ||
            (arg->set_mfilter.num_filters > 0 &&
            (arg->set_mfilter.start_array == NULL || arg->set_mfilter.end_array == NULL))) {
        return;
    }

    bta_pan_ci_set_mfilters(arg->set_mfilter.handle, arg->set_mfilter.num_filters,
                            (UINT8 *)arg->set_mfilter.start_array,
                            (UINT8 *)arg->set_mfilter.end_array);
}

void btc_pan_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_pan_args_t *dst = (btc_pan_args_t *)p_dest;
    btc_pan_args_t *src = (btc_pan_args_t *)p_src;

    memcpy(dst, src, sizeof(btc_pan_args_t));

    switch (msg->act) {
    case BTC_PAN_ACT_INIT:
        if (src->init.cfg.panu_service_name) {
            size_t len = strlen(src->init.cfg.panu_service_name) + 1;
            char *name = (char *)osi_malloc(len);
            if (name) {
                memcpy(name, src->init.cfg.panu_service_name, len);
                dst->init.cfg.panu_service_name = name;
            } else {
                dst->init.cfg.panu_service_name = NULL;
            }
        }
        if (src->init.cfg.gn_service_name) {
            size_t len = strlen(src->init.cfg.gn_service_name) + 1;
            char *name = (char *)osi_malloc(len);
            if (name) {
                memcpy(name, src->init.cfg.gn_service_name, len);
                dst->init.cfg.gn_service_name = name;
            } else {
                dst->init.cfg.gn_service_name = NULL;
            }
        }
        if (src->init.cfg.nap_service_name) {
            size_t len = strlen(src->init.cfg.nap_service_name) + 1;
            char *name = (char *)osi_malloc(len);
            if (name) {
                memcpy(name, src->init.cfg.nap_service_name, len);
                dst->init.cfg.nap_service_name = name;
            } else {
                dst->init.cfg.nap_service_name = NULL;
            }
        }
        break;
    case BTC_PAN_ACT_WRITE:
        if (src->write.len > 0 && src->write.data) {
            dst->write.data = (uint8_t *)osi_malloc(src->write.len);
            if (dst->write.data) {
                memcpy(dst->write.data, src->write.data, src->write.len);
            } else {
                dst->write.len = 0;
            }
        }
        break;
    case BTC_PAN_ACT_SET_PFILTER:
        if (src->set_pfilter.num_filters > 0) {
            size_t size = src->set_pfilter.num_filters * sizeof(uint16_t);
            dst->set_pfilter.start_array = (uint16_t *)osi_malloc(size);
            dst->set_pfilter.end_array = (uint16_t *)osi_malloc(size);
            if (dst->set_pfilter.start_array && dst->set_pfilter.end_array) {
                memcpy(dst->set_pfilter.start_array, src->set_pfilter.start_array, size);
                memcpy(dst->set_pfilter.end_array, src->set_pfilter.end_array, size);
            } else {
                if (dst->set_pfilter.start_array) {
                    osi_free(dst->set_pfilter.start_array);
                    dst->set_pfilter.start_array = NULL;
                }
                if (dst->set_pfilter.end_array) {
                    osi_free(dst->set_pfilter.end_array);
                    dst->set_pfilter.end_array = NULL;
                }
                dst->set_pfilter.num_filters = 0;
            }
        }
        break;
    case BTC_PAN_ACT_SET_MFILTER:
        if (src->set_mfilter.num_filters > 0) {
            size_t size = src->set_mfilter.num_filters * sizeof(esp_bd_addr_t);
            dst->set_mfilter.start_array = (esp_bd_addr_t *)osi_malloc(size);
            dst->set_mfilter.end_array = (esp_bd_addr_t *)osi_malloc(size);
            if (dst->set_mfilter.start_array && dst->set_mfilter.end_array) {
                memcpy(dst->set_mfilter.start_array, src->set_mfilter.start_array, size);
                memcpy(dst->set_mfilter.end_array, src->set_mfilter.end_array, size);
            } else {
                if (dst->set_mfilter.start_array) {
                    osi_free(dst->set_mfilter.start_array);
                    dst->set_mfilter.start_array = NULL;
                }
                if (dst->set_mfilter.end_array) {
                    osi_free(dst->set_mfilter.end_array);
                    dst->set_mfilter.end_array = NULL;
                }
                dst->set_mfilter.num_filters = 0;
            }
        }
        break;
    default:
        break;
    }
}

void btc_pan_arg_deep_free(btc_msg_t *msg)
{
    btc_pan_args_t *arg = (btc_pan_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_PAN_ACT_INIT:
        if (arg->init.cfg.panu_service_name) {
            osi_free((void *)arg->init.cfg.panu_service_name);
            arg->init.cfg.panu_service_name = NULL;
        }
        if (arg->init.cfg.gn_service_name) {
            osi_free((void *)arg->init.cfg.gn_service_name);
            arg->init.cfg.gn_service_name = NULL;
        }
        if (arg->init.cfg.nap_service_name) {
            osi_free((void *)arg->init.cfg.nap_service_name);
            arg->init.cfg.nap_service_name = NULL;
        }
        break;
    case BTC_PAN_ACT_WRITE:
        if (arg->write.data) {
            osi_free(arg->write.data);
            arg->write.data = NULL;
        }
        break;
    case BTC_PAN_ACT_SET_PFILTER:
        if (arg->set_pfilter.start_array) {
            osi_free(arg->set_pfilter.start_array);
            arg->set_pfilter.start_array = NULL;
        }
        if (arg->set_pfilter.end_array) {
            osi_free(arg->set_pfilter.end_array);
            arg->set_pfilter.end_array = NULL;
        }
        break;
    case BTC_PAN_ACT_SET_MFILTER:
        if (arg->set_mfilter.start_array) {
            osi_free(arg->set_mfilter.start_array);
            arg->set_mfilter.start_array = NULL;
        }
        if (arg->set_mfilter.end_array) {
            osi_free(arg->set_mfilter.end_array);
            arg->set_mfilter.end_array = NULL;
        }
        break;
    default:
        break;
    }
}

void btc_pan_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_pan_cb_param_t *dst = (esp_pan_cb_param_t *)p_dest;
    esp_pan_cb_param_t *src = (esp_pan_cb_param_t *)p_src;

    /* btc_transfer_context() already did a shallow memcpy; only deep-copy
     * pointer fields so variable-length payload is allocated once. */
    switch (msg->act) {
    case ESP_PAN_DATA_IND_EVT:
        dst->data_ind.data = NULL;
        if (src->data_ind.len > 0 && src->data_ind.data) {
            dst->data_ind.data = (uint8_t *)osi_malloc(src->data_ind.len);
            if (dst->data_ind.data) {
                memcpy(dst->data_ind.data, src->data_ind.data, src->data_ind.len);
            } else {
                BTC_TRACE_ERROR("%s: malloc data_ind failed len=%u", __func__, src->data_ind.len);
                dst->data_ind.len = 0;
            }
        }
        break;
    case ESP_PAN_PFILTER_EVT:
        dst->pfilter.filters = NULL;
        if (src->pfilter.len > 0 && src->pfilter.filters) {
            dst->pfilter.filters = (uint8_t *)osi_malloc(src->pfilter.len);
            if (dst->pfilter.filters) {
                memcpy(dst->pfilter.filters, src->pfilter.filters, src->pfilter.len);
            } else {
                BTC_TRACE_ERROR("%s: malloc pfilter failed len=%u", __func__, src->pfilter.len);
                dst->pfilter.len = 0;
            }
        }
        break;
    case ESP_PAN_MFILTER_EVT:
        dst->mfilter.filters = NULL;
        if (src->mfilter.len > 0 && src->mfilter.filters) {
            dst->mfilter.filters = (uint8_t *)osi_malloc(src->mfilter.len);
            if (dst->mfilter.filters) {
                memcpy(dst->mfilter.filters, src->mfilter.filters, src->mfilter.len);
            } else {
                BTC_TRACE_ERROR("%s: malloc mfilter failed len=%u", __func__, src->mfilter.len);
                dst->mfilter.len = 0;
            }
        }
        break;
    default:
        break;
    }
}

void btc_pan_cb_deep_free(btc_msg_t *msg)
{
    esp_pan_cb_param_t *param = (esp_pan_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_PAN_DATA_IND_EVT:
        if (param->data_ind.data) {
            osi_free(param->data_ind.data);
            param->data_ind.data = NULL;
        }
        break;
    case ESP_PAN_PFILTER_EVT:
        if (param->pfilter.filters) {
            osi_free(param->pfilter.filters);
            param->pfilter.filters = NULL;
        }
        break;
    case ESP_PAN_MFILTER_EVT:
        if (param->mfilter.filters) {
            osi_free(param->mfilter.filters);
            param->mfilter.filters = NULL;
        }
        break;
    default:
        break;
    }
}

void btc_pan_call_handler(btc_msg_t *msg)
{
    btc_pan_args_t *arg = (btc_pan_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_PAN_ACT_INIT:
        btc_pan_init(&arg->init.cfg);
        break;
    case BTC_PAN_ACT_DEINIT:
        btc_pan_deinit();
        break;
    case BTC_PAN_ACT_CONNECT:
        btc_pan_connect(arg);
        break;
    case BTC_PAN_ACT_DISCONNECT:
        btc_pan_disconnect(arg->disconnect.handle);
        break;
    case BTC_PAN_ACT_WRITE:
        btc_pan_write(arg);
        break;
    case BTC_PAN_ACT_SET_PFILTER:
        btc_pan_set_protocol_filters(arg);
        break;
    case BTC_PAN_ACT_SET_MFILTER:
        btc_pan_set_multicast_filters(arg);
        break;
    default:
        BTC_TRACE_ERROR("%s: unhandled act %d", __func__, msg->act);
        break;
    }

    btc_pan_arg_deep_free(msg);
}

void btc_pan_cb_handler(btc_msg_t *msg)
{
    esp_pan_cb_param_t param = {0};
    tBTA_PAN *p_data = (tBTA_PAN *)msg->arg;

    switch (msg->act) {
    case BTA_PAN_ENABLE_EVT:
        /* Ignore stale enable completion after esp_pan_deinit(). */
        if (!s_btc_pan_init) {
            break;
        }
        btc_pan_set_role();
        break;
    case BTA_PAN_SET_ROLE_EVT:
        if (!s_btc_pan_init) {
            break;
        }
        param.init.status = btc_pan_status_from_bta(p_data->set_role.status);
        btc_pan_cb_to_app(ESP_PAN_INIT_EVT, &param);
        param.set_role.status = param.init.status;
        param.set_role.role = (esp_pan_role_mask_t)p_data->set_role.role;
        btc_pan_cb_to_app(ESP_PAN_SET_ROLE_EVT, &param);
        break;
    case BTA_PAN_OPENING_EVT:
        param.opening.handle = p_data->opening.handle;
        memcpy(param.opening.remote_bda, p_data->opening.bd_addr, ESP_BD_ADDR_LEN);
        btc_pan_cb_to_app(ESP_PAN_OPENING_EVT, &param);
        break;
    case BTA_PAN_OPEN_EVT:
        param.open.status = btc_pan_status_from_bta(p_data->open.status);
        param.open.handle = p_data->open.handle;
        param.open.local_role = (esp_pan_role_t)p_data->open.local_role;
        param.open.peer_role = (esp_pan_role_t)p_data->open.peer_role;
        memcpy(param.open.remote_bda, p_data->open.bd_addr, ESP_BD_ADDR_LEN);
        btc_pan_cb_to_app(ESP_PAN_OPEN_EVT, &param);
        break;
    case BTA_PAN_CLOSE_EVT:
        param.close.handle = p_data->close.handle;
        btc_pan_cb_to_app(ESP_PAN_CLOSE_EVT, &param);
        break;
    case ESP_PAN_DATA_IND_EVT:
    case ESP_PAN_PFILTER_EVT:
    case ESP_PAN_MFILTER_EVT:
        btc_pan_cb_to_app(msg->act, (esp_pan_cb_param_t *)msg->arg);
        break;
    case ESP_PAN_CONG_EVT:
        memcpy(&param.cong, msg->arg, sizeof(param.cong));
        btc_pan_cb_to_app(ESP_PAN_CONG_EVT, &param);
        break;
    default:
        BTC_TRACE_ERROR("%s: unhandled event %d", __func__, msg->act);
        break;
    }

    btc_pan_cb_deep_free(msg);
}

void btc_pan_co_data_ind(UINT16 handle, BD_ADDR src, BD_ADDR dst, UINT16 protocol,
                         UINT8 *p_data, UINT16 len, BOOLEAN ext, BOOLEAN forward)
{
    btc_msg_t msg;
    esp_pan_cb_param_t param = {0};

    param.data_ind.handle = handle;
    memcpy(param.data_ind.src, src, ESP_BD_ADDR_LEN);
    memcpy(param.data_ind.dst, dst, ESP_BD_ADDR_LEN);
    param.data_ind.protocol = protocol;
    param.data_ind.len = len;
    param.data_ind.data = p_data;
    param.data_ind.ext = ext;
    param.data_ind.forward = forward;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PAN;
    msg.act = ESP_PAN_DATA_IND_EVT;

    if (btc_transfer_context(&msg, &param, sizeof(esp_pan_cb_param_t),
                             btc_pan_cb_deep_copy, btc_pan_cb_deep_free) != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s: context transfer failed", __func__);
    }
}

void btc_pan_co_rx_flow(UINT16 handle, BOOLEAN enable)
{
    btc_msg_t msg;
    esp_pan_cb_param_t param;

    param.cong.handle = handle;
    param.cong.cong = !enable;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PAN;
    msg.act = ESP_PAN_CONG_EVT;

    if (btc_transfer_context(&msg, &param.cong, sizeof(param.cong), NULL, NULL) != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s: context transfer failed", __func__);
    }
}

void btc_pan_co_pfilt_ind(UINT16 handle, BOOLEAN indication, tBTA_PAN_STATUS result,
                           UINT16 len, UINT8 *p_filters)
{
    btc_msg_t msg;
    esp_pan_cb_param_t param = {0};

    param.pfilter.handle = handle;
    param.pfilter.indication = indication;
    param.pfilter.status = btc_pan_status_from_bta(result);
    param.pfilter.len = len;
    param.pfilter.filters = p_filters;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PAN;
    msg.act = ESP_PAN_PFILTER_EVT;

    if (btc_transfer_context(&msg, &param, sizeof(esp_pan_cb_param_t),
                             btc_pan_cb_deep_copy, btc_pan_cb_deep_free) != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s: context transfer failed", __func__);
    }
}

void btc_pan_co_mfilt_ind(UINT16 handle, BOOLEAN indication, tBTA_PAN_STATUS result,
                           UINT16 len, UINT8 *p_filters)
{
    btc_msg_t msg;
    esp_pan_cb_param_t param = {0};

    param.mfilter.handle = handle;
    param.mfilter.indication = indication;
    param.mfilter.status = btc_pan_status_from_bta(result);
    param.mfilter.len = len;
    param.mfilter.filters = p_filters;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PAN;
    msg.act = ESP_PAN_MFILTER_EVT;

    if (btc_transfer_context(&msg, &param, sizeof(esp_pan_cb_param_t),
                             btc_pan_cb_deep_copy, btc_pan_cb_deep_free) != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s: context transfer failed", __func__);
    }
}

#endif /* BTC_PAN_INCLUDED */
