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

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "genie_mesh.h"
#include "genie_util.h"
#include "genie_model_srv.h"
#include "genie_event.h"
#include "genie_reset.h"
#include "genie_timer.h"
#include "ble_mesh_example_nvs.h"

static const char *TAG = "genie_event";

extern void user_genie_event_handle(genie_event_t event, void *p_arg);
extern elem_state_t g_elem_state[];
extern nvs_handle_t NVS_HANDLE;

static uint8_t g_proving        = 0;
static bool g_genie_provisioned = 0;
static bool g_genie_silent_adv  = 0;

/**
 * @brief Execute reset action.
 *
 */
static void genie_reset_provision(void)
{
    ENTER_FUNC();
    g_genie_provisioned = 0;
    // erase reboot count
    ble_mesh_nvs_erase(NVS_HANDLE, GENIE_STORE_RESTART_COUNT_KEY);
    /* reset provision */
    ESP_ERROR_CHECK(esp_ble_mesh_node_local_reset());
}

/**
 * @brief Handle software reset.
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_sw_reset(void)
{
    ENTER_FUNC();
    /* reset provision information */
    genie_reset_provision();
    /* restart PB-ADV and PB-GATT */
    ESP_ERROR_CHECK(esp_ble_mesh_node_prov_enable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT));

    return GENIE_EVT_SDK_MESH_PBADV_START;
}

/**
 * @brief Handle hardware reset, and indicate hardware reset.
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_hw_reset_start(void)
{
    ENTER_FUNC();
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    if (esp_ble_mesh_node_is_provisioned()) {
        genie_indicate_hw_reset_event(); // Indicate hardware reset event to genie speaker
    }
#endif
    genie_reset_done_timer_start();

    return GENIE_EVT_HW_RESET_START;
}

/**
 * @brief Execute reset action
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_hw_reset_done(void)
{
    ENTER_FUNC();
    genie_reset_clean_count();
    /* reset provision information */
    genie_reset_provision();
    /* restart PB-ADV and PB-GATT */
    ESP_ERROR_CHECK(esp_ble_mesh_node_prov_enable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT));

    return GENIE_EVT_SDK_MESH_PBADV_START;
}

/**
 * @brief Genie Mesh Component Init,
 *      1. Handle Mesh Init according esp-ble-mesh init status
 *      2. Start power indeicate if ble-mesh already provisioned
 *      3. Start PB-ADV Timer if  ble-mesh not provisioned
 *      4. Hardware reset ble-mesh provision information if reset flag be configed.
 *
 * @param p_status
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_mesh_init(int *p_status)
{
    ENTER_FUNC();

    if (g_genie_silent_adv) {
        // ! note some expect behavior
        return GENIE_EVT_SDK_MESH_INIT;
    }

    if (*p_status == -EALREADY) {
        if (!genie_reset_get_flag()) {
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
            poweron_indicate_start();
#endif
            return GENIE_EVT_SDK_MESH_PROV_SUCCESS;
        }
    } else if (*p_status == 0) {
        return GENIE_EVT_SDK_MESH_PBADV_START;
    }

    if (genie_reset_get_flag()) {
        return GENIE_EVT_HW_RESET_START;
    }

    return GENIE_EVT_SDK_MESH_INIT;
}

/**
 * @brief After the Bluetooth Mesh device is powered on,  if it is in the unprovisioned state,
 *      it needs to broadcast the Unprovisioned Device Beacon, and the broadcast duration is 10 minutes by default
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_pbadv_start(void)
{
    ENTER_FUNC();
    genie_pbadv_timer_start();
    return GENIE_EVT_SDK_MESH_PBADV_START;
}

/**
 * @brief If the network is still not configured after the timeout,
 *      the device enters the silent broadcast state and closes the PB_GATT broadcast.
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_pbadv_timeout(void)
{
    ENTER_FUNC();
    genie_pbadv_timer_stop();
    return GENIE_EVT_SDK_MESH_SILENT_START;
}

/**
 * @brief Start silent adv
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_silent_start(void)
{
    ENTER_FUNC();
    g_genie_silent_adv = true;
    genie_pbadv_start_silent_adv();
    return GENIE_EVT_SDK_MESH_SILENT_START;
}

/**
 * @brief Start provisioning
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_prov_start(void)
{
    ENTER_FUNC();
    if (g_proving == 0) {
        g_proving = 1;
        /* disable adv timer */
        genie_pbadv_timer_stop();
        /* enable prov timer */
        genie_prov_timer_start();
    }
    return GENIE_EVT_SDK_MESH_PROV_START;
}

/**
 * @brief Provision timeout
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_prov_timeout(void)
{
    ENTER_FUNC();
    return GENIE_EVT_SDK_MESH_PROV_FAIL;
}

/**
 * @brief Provision success
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_prov_success(void)
{
    ENTER_FUNC();
    g_genie_provisioned = 1;
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    return GENIE_EVT_SDK_STATE_SYNC;
#else
    return GENIE_EVT_SDK_MESH_PROV_SUCCESS;
#endif
}

/**
 * @brief Provision fail
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_prov_fail(void)
{
    ENTER_FUNC();
    /* reset provision flag */
    g_proving = 0;
    genie_reset_provision();
    /* restart PB-ADV and PB-GATT */
    ESP_ERROR_CHECK(esp_ble_mesh_node_prov_enable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT));

    return GENIE_EVT_SDK_MESH_PBADV_START;
}

/**
 * @brief Sync device status.
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_sync(void)
{
    ENTER_FUNC();
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    g_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    g_indication_flag |= INDICATION_FLAG_LIGHTNESS;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    g_indication_flag |= INDICATION_FLAG_CTL;
#endif
#ifdef CONFIG_MESH_MODEL_HSL_SRV
    g_indication_flag |= INDICATION_FLAG_HSL;
#endif
    return GENIE_EVT_SDK_INDICATE;
}

/**
 * @brief AppKey add.
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_appkey_add(void)
{
    ENTER_FUNC();
    if (g_proving) {
        g_proving = 0;
        /* stop provision timer */
        genie_prov_timer_stop();

        return GENIE_EVT_SDK_MESH_PROV_SUCCESS;
    } else {
        return GENIE_EVT_SDK_APPKEY_ADD;
    }
    return GENIE_EVT_SDK_APPKEY_ADD;
}

/**
 * @brief Subscribe address add.
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_sub_add(void)
{
    ENTER_FUNC();

    return GENIE_EVT_SDK_SUB_ADD;
}

/**
 * @brief handle message.
 *
 * @param p_elem
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_analyze_msg(elem_state_t *p_elem)
{
    ENTER_FUNC();
#ifdef CONFIG_MESH_MODEL_TRANS
    if (p_elem->state.trans || p_elem->state.delay) {
        if (p_elem->state.delay) {
            return GENIE_EVT_SDK_DELAY_START;
        } else {
            return GENIE_EVT_SDK_TRANS_START;
        }
    }
#endif
    return GENIE_EVT_SDK_ACTION_DONE;
}

/**
 * @brief Execute color change action.
 *
 * @param p_elem
 * @param p_data
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_color_action(elem_state_t *p_elem, uint8_t *p_data)
{
    ENTER_FUNC();

#ifdef CONFIG_MESH_MODEL_HSL_SRV
    uint16_t lightness = *p_data++;
    lightness += (*p_data++ << 8);

    uint16_t hue = *p_data++;
    hue += (*p_data++ << 8);

    uint16_t saturation = *p_data++;
    saturation += (*p_data++ << 8);

    ESP_LOGD(TAG, "tar lightness(%d) hue(%d) saturation(%d)", p_elem->state.lightness[VALUE_TYPE_TAR], p_elem->state.hue[VALUE_TYPE_TAR], p_elem->state.saturation[VALUE_TYPE_TAR]);
#endif

    return GENIE_EVT_SDK_COLOR_ACTION;
}

/**
 * @brief action done.
 *
 * @param p_elem
 *
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_action_done(elem_state_t *p_elem)
{
    ENTER_FUNC();
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    ESP_LOGD(TAG, "onoff cur(%d) tar(%d)", p_elem->state.onoff[VALUE_TYPE_CUR], p_elem->state.onoff[VALUE_TYPE_TAR]);
    if (p_elem->state.onoff[VALUE_TYPE_CUR] != p_elem->state.onoff[VALUE_TYPE_TAR]) {
        p_elem->state.onoff[VALUE_TYPE_CUR] = p_elem->state.onoff[VALUE_TYPE_TAR];
    }
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    ESP_LOGD(TAG, "actual cur(%d) tar(%d)", p_elem->state.actual[VALUE_TYPE_CUR], p_elem->state.actual[VALUE_TYPE_TAR]);
    if (p_elem->state.actual[VALUE_TYPE_CUR] != p_elem->state.actual[VALUE_TYPE_TAR]) {
        p_elem->state.actual[VALUE_TYPE_CUR] = p_elem->state.actual[VALUE_TYPE_TAR];
    }
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    ESP_LOGD(TAG, "temp cur(%d) tar(%d)", p_elem->state.temp[VALUE_TYPE_CUR], p_elem->state.temp[VALUE_TYPE_TAR]);
    if (p_elem->state.temp[VALUE_TYPE_CUR] != p_elem->state.temp[VALUE_TYPE_TAR]) {
        p_elem->state.temp[VALUE_TYPE_CUR] = p_elem->state.temp[VALUE_TYPE_TAR];
    }
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
    ESP_LOGD(TAG, "hue cur(%d) tar(%d)", p_elem->state.hue[VALUE_TYPE_CUR], p_elem->state.hue[VALUE_TYPE_TAR]);
    if (p_elem->state.hue[VALUE_TYPE_CUR] != p_elem->state.hue[VALUE_TYPE_TAR]) {
        p_elem->state.hue[VALUE_TYPE_CUR] = p_elem->state.hue[VALUE_TYPE_TAR];
    }

    ESP_LOGD(TAG, "saturation cur(%d) tar(%d)", p_elem->state.saturation[VALUE_TYPE_CUR], p_elem->state.saturation[VALUE_TYPE_TAR]);
    if (p_elem->state.saturation[VALUE_TYPE_CUR] != p_elem->state.saturation[VALUE_TYPE_TAR]) {
        p_elem->state.saturation[VALUE_TYPE_CUR] = p_elem->state.saturation[VALUE_TYPE_TAR];
    }

    ESP_LOGD(TAG, "lightness cur(%d) tar(%d)", p_elem->state.lightness[VALUE_TYPE_CUR], p_elem->state.lightness[VALUE_TYPE_TAR]);
    if (p_elem->state.lightness[VALUE_TYPE_CUR] != p_elem->state.lightness[VALUE_TYPE_TAR]) {
        p_elem->state.lightness[VALUE_TYPE_CUR] = p_elem->state.lightness[VALUE_TYPE_TAR];
    }
#endif

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    if (esp_ble_mesh_node_is_provisioned()) {
        return GENIE_EVT_SDK_INDICATE;
    }
#endif
    return GENIE_EVT_SDK_ACTION_DONE;
}

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
/**
 * @brief power on indicate.
 *
 * @param p_elem
 * @return genie_event_t
 */
static genie_event_t genie_event_handle_poweron_indication(elem_state_t *p_elem)
{
    ENTER_FUNC();
    g_indication_flag |= INDICATION_FLAG_POWERON;
    return GENIE_EVT_SDK_INDICATE;
}

static void indicate_delay_timer_callback(void *arg)
{
    ENTER_FUNC();
    genie_standart_indication(arg);
}

static void reset_indicate_delay_timer(elem_state_t *p_elem)
{
    ENTER_FUNC();
    static esp_timer_handle_t timer = NULL;
    if (!timer) {
        esp_timer_create_args_t create_args = {
            .callback = indicate_delay_timer_callback,
            .arg      = (void *)p_elem,
            .name     = "delay_timer"
        };

        ESP_ERROR_CHECK(esp_timer_create(&create_args, &timer));
    }
    if (esp_timer_start_once(timer, 500 * 1000) != ESP_OK) {
        ESP_ERROR_CHECK(esp_timer_stop(timer));
        ESP_ERROR_CHECK(esp_timer_start_once(timer, 500 * 1000));
    }
}

static genie_event_t genie_event_handle_indicate(elem_state_t *p_elem)
{
    ENTER_FUNC();
    if (g_indication_flag) {
        reset_indicate_delay_timer(p_elem); // delay send indication to avoid data congestion
    }
    return GENIE_EVT_SDK_INDICATE;
}

static genie_event_t genie_event_handle_vendor_msg(genie_model_msg_t *p_msg)
{
    ENTER_FUNC();
    genie_vendor_model_msg_handle(p_msg);
    return GENIE_EVT_SDK_VENDOR_MSG;
}

#ifdef GENIE_MODEL_VENDOR_TIMER
static genie_event_t genie_event_handle_order_msg(genie_timer_attr_data_t *attr_data)
{
    ENTER_FUNC();
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    if (attr_data->type == GENIE_MODEL_ATTR_ONOFF) {
        g_indication_flag |= INDICATION_FLAG_ONOFF;
        g_elem_state[0].state.onoff[VALUE_TYPE_TAR] = attr_data->para;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    } else if (attr_data->type == GENIE_MODEL_ATTR_LIGHTNESS) {
        g_indication_flag |= INDICATION_FLAG_LIGHTNESS;
        // Todo
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    } else if (attr_data->type == GENIE_MODEL_ATTR_TEMPERATURE) {
        g_indication_flag |= INDICATION_FLAG_CTL;
        // Todo
#endif
#ifdef CONFIG_MESH_MODEL_HSL_SRV
    } else if (attr_data->type == GENIE_MODEL_ATTR_COLOR) {
        g_indication_flag |= INDICATION_FLAG_HSL;
        // Todo
#endif
    }
#ifdef CONFIG_MESH_MODEL_TRANS
    return GENIE_EVT_SDK_TRANS_CYCLE;
#else
    return GENIE_EVT_SDK_ACTION_DONE;
#endif
}
#endif /* GENIE_MODEL_VENDOR_TIMER */

#endif /* CONFIG_MESH_MODEL_VENDOR_SRV */

void genie_event(genie_event_t event, void *p_arg)
{
    uint8_t       ignore_user_event = 0;
    genie_event_t next_event        = event;
    ESP_LOGD(TAG, "genie_event event %d", event);

#ifdef CONFIG_MESH_MODEL_TRANS
    if (event != GENIE_EVT_SDK_TRANS_CYCLE) {
        GENIE_MESH_EVENT_PRINT(event);
    }
#endif
    switch (event) {
    case GENIE_EVT_SW_RESET:
        next_event = genie_event_handle_sw_reset();
        break;

    case GENIE_EVT_HW_RESET_START:
        genie_event_handle_hw_reset_start();
        break;

    case GENIE_EVT_HW_RESET_DONE:
        next_event = genie_event_handle_hw_reset_done();
        break;

    case GENIE_EVT_SDK_MESH_INIT:
        next_event = genie_event_handle_mesh_init((int *)p_arg);
        p_arg = (void *)&g_elem_state[0]; // update p_arg to element state.
        break;

    case GENIE_EVT_SDK_MESH_PBADV_START:
        next_event = genie_event_handle_pbadv_start();
        break;

    case GENIE_EVT_SDK_MESH_PBADV_TIMEOUT:
        next_event = genie_event_handle_pbadv_timeout();
        break;

    case GENIE_EVT_SDK_MESH_SILENT_START:
        next_event = genie_event_handle_silent_start();
        break;

    case GENIE_EVT_SDK_MESH_PROV_START:
        next_event = genie_event_handle_prov_start();
        break;

    case GENIE_EVT_SDK_MESH_PROV_TIMEOUT:
        next_event = genie_event_handle_prov_timeout();
        break;

    case GENIE_EVT_SDK_MESH_PROV_SUCCESS:
        next_event = genie_event_handle_prov_success();
        break;

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    case GENIE_EVT_SDK_STATE_SYNC:
        next_event = genie_event_handle_sync();
        break;
#endif

    case GENIE_EVT_SDK_MESH_PROV_FAIL:
        next_event = genie_event_handle_prov_fail();
        break;

    case GENIE_EVT_SDK_APPKEY_ADD: {
        next_event = genie_event_handle_appkey_add();
        if (next_event == GENIE_EVT_SDK_MESH_PROV_SUCCESS) {
            p_arg = &g_elem_state[0];
        }
        break;
    }

    case GENIE_EVT_SDK_APPKEY_DEL:
    case GENIE_EVT_SDK_APPKEY_UPDATE:
        break;

    case GENIE_EVT_SDK_NETKEY_ADD:
    case GENIE_EVT_SDK_NETKEY_DEL:
    case GENIE_EVT_SDK_NETKEY_UPDATE:
    case GENIE_EVT_SDK_IVI_UPDATE:
        break;

    case GENIE_EVT_SDK_SUB_ADD:
        next_event = genie_event_handle_sub_add();
        break;

    case GENIE_EVT_SDK_SUB_DEL:
        break;

    case GENIE_EVT_SDK_ANALYZE_MSG:
        next_event = genie_event_handle_analyze_msg((elem_state_t *)p_arg);
        break;

    case GENIE_EVT_SDK_COLOR_ACTION:
        ESP_LOGD(TAG, "GENIE_EVT_SDK_COLOR_ACTION");
        next_event = genie_event_handle_color_action(&g_elem_state[0], (uint8_t *)p_arg);
        break;

    case GENIE_EVT_SDK_ACTION_DONE:
        ESP_LOGD(TAG, "GENIE_EVT_SDK_ACTION_DONE");
        next_event = genie_event_handle_action_done((elem_state_t *)p_arg);
        break;

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
    case GENIE_EVT_SDK_MESH_PWRON_INDC:
        next_event = genie_event_handle_poweron_indication((elem_state_t *)p_arg);
        p_arg = &g_elem_state[0];
        break;

    case GENIE_EVT_SDK_INDICATE:
        ESP_LOGD(TAG, "GENIE_EVT_SDK_INDICATE");
        next_event = genie_event_handle_indicate((elem_state_t *)p_arg);
        break;

    case GENIE_EVT_SDK_VENDOR_MSG:
        ESP_LOGD(TAG, "GENIE_EVT_SDK_VENDOR_MSG");
        next_event = genie_event_handle_vendor_msg((genie_model_msg_t *)p_arg);
        break;
#endif

#ifdef GENIE_MODEL_VENDOR_TIMER
    case GENIE_EVT_TIME_OUT: {
        ESP_LOGD(TAG, "GENIE_EVT_TIME_OUT");
        next_event = genie_event_handle_order_msg((genie_timer_attr_data_t *)p_arg);
        p_arg = &g_elem_state[0];
        break;
    }
#endif

    case GENIE_EVT_APP_FAC_QUIT:
        ESP_LOGD(TAG, "GENIE_EVT_APP_FAC_QUIT");
        break;
    default:
        ESP_LOGD(TAG, "unhandle this event");
        break;
    }

    if (!ignore_user_event) {
        ESP_LOGD(TAG, "post to user_genie_event_handle");
        user_genie_event_handle(event, p_arg);
    }

    if (next_event != event) {
        ESP_LOGD(TAG, "next_event: %d, event: %d", next_event, event);
        genie_event(next_event, p_arg);
    }
}
