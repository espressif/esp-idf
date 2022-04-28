/*
 * Copyright (c) 2021 Espressif Systems (Shanghai) CO LTD
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Espressif Systems
 *    integrated circuit in a product or a software update for such product,
 *    must reproduce the above copyright notice, this list of conditions and
 *    the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "esp_log.h"
#include "esp_zb_light.h"

static bulb_device_ctx_t esp_light_ctx = {
    /* basic cluster attributes data */
    .basic_attr.zcl_version = ZB_ZCL_VERSION,
    .basic_attr.power_source = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN,
    /* identify cluster attributes data */
    .identify_attr.identify_time = 0,
    /* groups cluster attributes data */
    .groups_attr.name_support = 0,
    /* on/off cluster attributes data */
    .on_off_attr.on_off = ZB_ZCL_ON_OFF_IS_OFF,
};

static const char *TAG = "ESP_ZB_LIGHT";
/******************* Declare attributes ************************/

/* basic cluster attributes data */
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(basic_attr_list,
                                 &esp_light_ctx.basic_attr.zcl_version,
                                 &esp_light_ctx.basic_attr.power_source);

/* identify cluster attributes data */
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list,
                                    &esp_light_ctx.identify_attr.identify_time);

/* groups cluster attributes data */
ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attr_list,
                                  &esp_light_ctx.groups_attr.name_support);

/* scenes cluster attribute data */
ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attr_list,
                                  &esp_light_ctx.scenes_attr.scene_count,
                                  &esp_light_ctx.scenes_attr.current_scene,
                                  &esp_light_ctx.scenes_attr.current_group,
                                  &esp_light_ctx.scenes_attr.scene_valid,
                                  &esp_light_ctx.scenes_attr.name_support);

/* on/off cluster attributes data */
ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(on_off_attr_list,
                                  &esp_light_ctx.on_off_attr.on_off);

/********************* Declare device **************************/
ZB_HA_DECLARE_ON_OFF_OUTPUT_CLUSTER_LIST(custom_light_clusters,
        on_off_attr_list,
        basic_attr_list,
        identify_attr_list,
        groups_attr_list,
        scenes_attr_list);

ZB_HA_DECLARE_ON_OFF_OUTPUT_EP(custom_esp_light_ep,
                               HA_ESP_LIGHT_ENDPOINT,
                               custom_light_clusters);

ZB_HA_DECLARE_ON_OFF_OUTPUT_CTX(esp_zb_light_ctx,
                                custom_esp_light_ep);

/********************* Define functions **************************/
static void bdb_start_top_level_commissioning_cb(zb_uint8_t mode_mask)
{
    if (!bdb_start_top_level_commissioning(mode_mask)) {
        ESP_LOGE(TAG, "In BDB commissioning, an error occurred (for example: the device has already been running)");
    }
}

/**
 * @brief Zigbee zboss stack event signal handler.
 *
 * @param bufid   Zigbee zboss stack buffer id used to pass signal.
 */
void zboss_signal_handler(zb_bufid_t bufid)
{
    zb_uint8_t status = ZB_GET_APP_SIGNAL_STATUS(bufid);
    zb_zdo_app_signal_type_t sig = zb_get_app_signal(bufid, NULL);

    switch (sig) {
    case ZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(TAG, "Zigbee stack initialized");
        bdb_start_top_level_commissioning(ZB_BDB_INITIALIZATION);
        break;
    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        if (status == RET_OK) {
            ESP_LOGI(TAG, "Start network steering");
            bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        } else {
            ESP_LOGE(TAG, "Failed to initialize Zigbee stack (status: %d)", status);
        }
        break;
    case ZB_BDB_SIGNAL_STEERING:
        if (status == RET_OK) {
            zb_ext_pan_id_t extended_pan_id;
            zb_get_extended_pan_id(extended_pan_id);
            ESP_LOGI(TAG, "Joined network successfully (Extended PAN ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx)",
                     extended_pan_id[7], extended_pan_id[6], extended_pan_id[5], extended_pan_id[4],
                     extended_pan_id[3], extended_pan_id[2], extended_pan_id[1], extended_pan_id[0],
                     ZB_PIBCACHE_PAN_ID());
        } else {
            ESP_LOGI(TAG, "Network steering was not successful (status: %d)", status);
            ZB_SCHEDULE_APP_ALARM((zb_callback_t)bdb_start_top_level_commissioning_cb, ZB_BDB_NETWORK_STEERING, ZB_TIME_ONE_SECOND);
        }
        break;
    default:
        ESP_LOGI(TAG, "status: %d", status);
        break;
    }
    if (bufid) {
        zb_buf_free(bufid);
    }
}

/**
 * @brief Function for turning on/off the light bulb.
 *
 * @param power   Boolean light bulb state power on/off.
 */
static void esp_zb_light_set_value(zb_bool_t power)
{
    ZB_ZCL_SET_ATTRIBUTE(HA_ESP_LIGHT_ENDPOINT,
                         ZB_ZCL_CLUSTER_ID_ON_OFF,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                         (zb_uint8_t *)&power,
                         ZB_FALSE);

    light_driver_set_power(power);
}

/**
 * @brief Callback function for handling attribute value from ZCL.
 *
 * @param bufid   Zigbee zboss stack buffer id used to pass received data.
 */
static void esp_zb_light_cb(zb_bufid_t bufid)
{
    zb_zcl_device_callback_param_t *p_device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);
    zb_zcl_device_callback_id_t device_cb_id    = p_device_cb_param->device_cb_id;
    zb_uint16_t                 cluster_id      = p_device_cb_param->cb_param.set_attr_value_param.cluster_id;
    zb_uint16_t                 attr_id         = p_device_cb_param->cb_param.set_attr_value_param.attr_id;
    p_device_cb_param->status = RET_OK;

    switch (device_cb_id) {
    /* ZCL set attribute value */
    case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
        if (cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF) {
            uint8_t value = p_device_cb_param->cb_param.set_attr_value_param.values.data8;
            ESP_LOGI(TAG, "on/off attribute setting to %hd", value);

            if (attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID) {
                /* implemented light on/off control */
                esp_zb_light_set_value((zb_bool_t) value);
            }
        } else if (cluster_id == ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) {
            uint16_t value = p_device_cb_param->cb_param.set_attr_value_param.values.data16;
            ESP_LOGI(TAG, "level control attribute setting to %hd", value);

            if (attr_id == ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID) {
                /* implement light level control if needed */
            }
        } else if (cluster_id == ZB_ZCL_CLUSTER_ID_COLOR_CONTROL) {
            uint16_t value = p_device_cb_param->cb_param.set_attr_value_param.values.data8;
            ESP_LOGI(TAG, "attribute 0x%x setting to %d", attr_id, value);

            if (attr_id == ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_HUE_ID) {
                /* implement light color change if needed */
            }
        } else {
            /* other clusters attribute handled here */
            ESP_LOGI(TAG, "Unhandled cluster attribute id: %d", cluster_id);
        }
        break;
    default:
        p_device_cb_param->status = RET_ERROR;
        break;
    }
}

void app_main(void)
{
    zb_ret_t       zb_err_code;
    zb_esp_platform_config_t config = {
        .radio_config = ZB_ESP_DEFAULT_RADIO_CONFIG(),
        .host_config = ZB_ESP_DEFAULT_HOST_CONFIG(),
    };

    ESP_ERROR_CHECK(zb_esp_platform_config(&config));
    /* initialize Zigbee stack */
    ZB_INIT("light_bulb");
    zb_set_network_router_role(IEEE_CHANNEL_MASK);
    zb_set_max_children(MAX_CHILDREN);
    zb_set_nvram_erase_at_start(ERASE_PERSISTENT_CONFIG);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));
    /* hardware related and device init */
    light_driver_init(LIGHT_DEFAULT_OFF);
    /* register callback for handling ZCL commands */
    ZB_ZCL_REGISTER_DEVICE_CB(esp_zb_light_cb);
    /* register light device context (endpoints) */
    ZB_AF_REGISTER_DEVICE_CTX(&esp_zb_light_ctx);
    zb_err_code = zboss_start_no_autostart();
    ESP_ERROR_CHECK(zb_err_code);

    while (1) {
        zboss_main_loop_iteration();
    }
}
