/*
 * AliGenie - Example
 *
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"

#include "mbedtls/sha256.h"

#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_health_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_lighting_model_api.h"
#include "esp_ble_mesh_time_scene_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "board.h"
#include "genie_mesh.h"
#include "ble_mesh_example_init.h"
#include "ble_mesh_example_nvs.h"

static const char *TAG  = "genie_demo";

#define MESH_ELEM_COUNT         1
#define MESH_ELEM_STATE_COUNT   MESH_ELEM_COUNT
#define LIGHT_STATUS_STORE_KEY   "light_status"

nvs_handle_t NVS_HANDLE;

elem_state_t g_elem_state[MESH_ELEM_STATE_COUNT] = {0};
model_powerup_t g_powerup[MESH_ELEM_STATE_COUNT] = {0};

static uint8_t static_val[16] = {0x64, 0xda, 0x19, 0x8f, 0x58, 0xea, 0x55, 0x85,
                                 0x75, 0x84, 0x0d, 0xbf, 0x4f, 0x6e, 0x36, 0x8f
                                };

static uint8_t dev_uuid[16] = {
    0xA8, 0x01, // CID
    0x71,       // PID Bit0-3 ble adv version, 0x01
    // bit4: One machine one secret
    // bit5: Support OTA
    // bit6~7：
    // 00：BLE4.0
    // 01：BLE4.2
    // 10：BLE5.0
    // 11：BLE5.0 above
    0x7F, 0x2C, 0x00, 0x00,             // ProductID // 00002C7F
    0xda, 0x91, 0x01, 0x7a, 0xfa, 0x28, // MAC Address // 28fa7a0191da
#if GENIE_VENDOR_MODEL_VERSION == 0
    0x00,       // Feature Flag bit7-1：uuid version, version 0
    // bit0  0: unprovisioned adv
    //       1: silent adv
#elif GENIE_VENDOR_MODEL_VERSION == 1
    0x02,       // Feature Flag bit7-1：uuidversion, version 1
    // bit0  0: unprovisioned adv
    //       1: silent adv
#endif
    0x00, 0x00, // RFU
};

static esp_ble_mesh_cfg_srv_t config_server = {
    /* 3 transmissions with 20ms interval */
    .net_transmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .relay = ESP_BLE_MESH_RELAY_ENABLED,
    /* 3 transmissions with 20ms interval */
    .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .beacon = ESP_BLE_MESH_BEACON_ENABLED,
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BLE_MESH_FRIEND)
    .friend_state = ESP_BLE_MESH_FRIEND_ENABLED,
#else
    .friend_state = ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,
#endif
    .default_ttl = 7,
};

uint8_t test_ids[1] = {0x00};

/** ESP BLE Mesh Health Server Model Context */
ESP_BLE_MESH_MODEL_PUB_DEFINE(health_pub, 2 + 11, ROLE_NODE);
static esp_ble_mesh_health_srv_t health_server = {
    .health_test = {
        .id_count = 1,
        .test_ids = test_ids,
    },
};

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_pub_0, 2 + 3, ROLE_NODE);
static esp_ble_mesh_gen_onoff_srv_t onoff_server_0 = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    },
};
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
/* Light Lightness state related context */
static esp_ble_mesh_light_lightness_state_t lightness_state;

/* Light Lightness Server related context */
ESP_BLE_MESH_MODEL_PUB_DEFINE(lightness_pub, 2 + 5, ROLE_NODE);
static esp_ble_mesh_light_lightness_srv_t lightness_server = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    },
    .state = &lightness_state,
};

/* Light Lightness Setup Server related context */
ESP_BLE_MESH_MODEL_PUB_DEFINE(lightness_setup_pub, 2 + 5, ROLE_NODE);
static esp_ble_mesh_light_lightness_setup_srv_t lightness_setup_server = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    },
    .state = &lightness_state,
};
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
/* Light CTL state related context */
static esp_ble_mesh_light_ctl_state_t ctl_state;

/* Light CTL Server related context */
ESP_BLE_MESH_MODEL_PUB_DEFINE(ctl_pub, 2 + 9, ROLE_NODE);
static esp_ble_mesh_light_ctl_srv_t ctl_server = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    },
    .state = &ctl_state,
};

/* Light CTL Setup Server related context */
ESP_BLE_MESH_MODEL_PUB_DEFINE(ctl_setup_pub, 2 + 6, ROLE_NODE);
static esp_ble_mesh_light_ctl_setup_srv_t ctl_setup_server = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    },
    .state = &ctl_state,
};
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
/* Light HSL state related context */
static esp_ble_mesh_light_hsl_state_t hsl_state;

/* Light HSL Server related context */
ESP_BLE_MESH_MODEL_PUB_DEFINE(hsl_pub, 2 + 9, ROLE_NODE);
static esp_ble_mesh_light_hsl_srv_t hsl_server = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    },
    .state = &hsl_state,
};

/* Light HSL Setup Server related context */
ESP_BLE_MESH_MODEL_PUB_DEFINE(hsl_setup_pub, 2 + 9, ROLE_NODE);
static esp_ble_mesh_light_hsl_setup_srv_t hsl_setup_server = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    },
    .state = &hsl_state,
};
#endif

static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_HEALTH_SRV(&health_server, &health_pub),
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_pub_0, &onoff_server_0),
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SRV(&lightness_pub, &lightness_server),
    ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SETUP_SRV(&lightness_setup_pub, &lightness_setup_server),
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    ESP_BLE_MESH_MODEL_LIGHT_CTL_SRV(&ctl_pub, &ctl_server),
    ESP_BLE_MESH_MODEL_LIGHT_CTL_SETUP_SRV(&ctl_setup_pub, &ctl_setup_server),
#endif
#ifdef CONFIG_MESH_MODEL_HSL_SRV
    ESP_BLE_MESH_MODEL_LIGHT_HSL_SRV(&hsl_pub, &hsl_server),
    ESP_BLE_MESH_MODEL_LIGHT_HSL_SETUP_SRV(&hsl_setup_pub, &hsl_setup_server),
#endif
};

/** @def g_vendor_model_alibaba_op
 *
 *  @brief vendor model operations struct
 *
 */
static esp_ble_mesh_model_op_t g_vendor_model_alibaba_op[] = {
    ESP_BLE_MESH_MODEL_OP(GENIE_MESSAGE_OP_ATTR_GET_STATUS,        2),
    ESP_BLE_MESH_MODEL_OP(GENIE_MESSAGE_OP_ATTR_SET_ACK,           2),
    ESP_BLE_MESH_MODEL_OP(GENIE_MESSAGE_OP_ATTR_SET_UNACK,         2),
    ESP_BLE_MESH_MODEL_OP(GENIE_MESSAGE_OP_ATTR_STATUS,            1),
    ESP_BLE_MESH_MODEL_OP(GENIE_MESSAGE_OP_ATTR_INDICATION,        1),
    ESP_BLE_MESH_MODEL_OP(GENIE_MESSAGE_OP_ATTR_CONFIRMATION,      1),
    ESP_BLE_MESH_MODEL_OP(GENIE_MESSAGE_OP_ATTR_TRANSPARENT_MSG,   1),
    ESP_BLE_MESH_MODEL_OP_END,
};

static esp_ble_mesh_model_t ali_vnd_models[] = {
    ESP_BLE_MESH_VENDOR_MODEL(CID_ALIBABA, GENIE_VENDOR_MODEL_SRV_ID, g_vendor_model_alibaba_op, NULL, NULL),
};

ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_pub_1, 2 + 3, ROLE_NODE);
static esp_ble_mesh_gen_onoff_srv_t onoff_server_1 = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
};

ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_pub_2, 2 + 3, ROLE_NODE);
static esp_ble_mesh_gen_onoff_srv_t onoff_server_2 = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
};

static esp_ble_mesh_model_t extend_model_0[] = {
    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_pub_1, &onoff_server_1),
};

static esp_ble_mesh_model_t extend_model_1[] = {
    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_pub_2, &onoff_server_2),
};

esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models,    ali_vnd_models),
    ESP_BLE_MESH_ELEMENT(0, extend_model_0, ESP_BLE_MESH_MODEL_NONE),
    ESP_BLE_MESH_ELEMENT(0, extend_model_1, ESP_BLE_MESH_MODEL_NONE),
};

static esp_ble_mesh_comp_t composition = {
    .cid           = CID_ALIBABA,
    .element_count = ARRAY_SIZE(elements),
    .elements      = elements,
};

/* Disable OOB security for SILabs Android app */
static esp_ble_mesh_prov_t provision = {
    .uuid           = dev_uuid,
    .static_val     = static_val,
    .static_val_len = 16,
    .output_size    = 0,                        // not support output OOB
    .output_actions = ESP_BLE_MESH_NO_OUTPUT,
    .input_size     = 0,                        // not support Input OOB
    .input_actions  = ESP_BLE_MESH_NO_INPUT,
};

static void light_color_set(uint16_t lightness, uint16_t hue, uint16_t saturation);

void set_silent_broadcast(void)
{
    dev_uuid[13] |= GENIE_UNPROV_ADV_FEATURE_SILENT_ADV;
}

void set_unprovision_broadcast(void)
{
    dev_uuid[13] &= ~GENIE_UNPROV_ADV_FEATURE_SILENT_ADV;
}

void user_prov_complete(uint16_t net_idx, uint16_t addr)
{
    ENTER_FUNC();
    // todo: add custom code
}

void user_prov_reset(void)
{
    ENTER_FUNC();
    // todo: add custom code
    esp_restart();
}

void reset_light_para(void)
{
    ENTER_FUNC();
    uint8_t i = 0;

    while (i < MESH_ELEM_STATE_COUNT) {
#ifdef CONFIG_MESH_SIMPLE_MODLE
        g_elem_state[i].state.onoff[VALUE_TYPE_CUR]  = GENIE_ONOFF_DEFAULT;
        g_elem_state[i].state.onoff[VALUE_TYPE_TAR]  = GENIE_ONOFF_DEFAULT;
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        g_elem_state[i].state.actual[VALUE_TYPE_CUR] = GENIE_LIGHTNESS_DEFAULT;
        g_elem_state[i].state.actual[VALUE_TYPE_TAR] = GENIE_LIGHTNESS_DEFAULT;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        g_elem_state[i].state.temp[VALUE_TYPE_CUR]   = GENIE_CTL_TEMP_DEFAULT;
        g_elem_state[i].state.temp[VALUE_TYPE_TAR]   = GENIE_CTL_TEMP_DEFAULT;
#endif
#ifdef CONFIG_MESH_MODEL_TRANS
        g_elem_state[i].state.trans            = 0;
        g_elem_state[i].state.delay            = 0;
        g_elem_state[i].state.trans_start_time = 0;
        g_elem_state[i].state.trans_end_time   = 0;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        g_elem_state[i].powerup.last_actual = GENIE_LIGHTNESS_DEFAULT;
        g_powerup   [i].last_actual         = GENIE_LIGHTNESS_DEFAULT;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        g_elem_state[i].powerup.last_temp   = GENIE_CTL_TEMP_DEFAULT;
        g_powerup   [i].last_temp           = GENIE_CTL_TEMP_DEFAULT;
#endif
#endif
        i++;
    }

    ble_mesh_nvs_store(NVS_HANDLE, POWERUP_KEY, g_powerup, sizeof(g_powerup));
}

void save_light_state(elem_state_t *p_elem)
{
    ENTER_FUNC();
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    if (p_elem->state.actual[VALUE_TYPE_CUR] != 0) {
        p_elem->powerup.last_actual = p_elem->state.actual[VALUE_TYPE_CUR];
        g_powerup[p_elem->elem_index].last_actual = p_elem->state.actual[VALUE_TYPE_CUR];
    }
    ESP_LOGD(TAG, "elem %d, actual %d", p_elem->elem_index, g_powerup[p_elem->elem_index].last_actual);
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    p_elem->powerup.last_temp = p_elem->state.temp[VALUE_TYPE_CUR];
    g_powerup[p_elem->elem_index].last_temp = p_elem->state.temp[VALUE_TYPE_CUR];
    ESP_LOGD(TAG, "elem %d, temp %d", p_elem->elem_index, g_powerup[p_elem->elem_index].last_temp);
#endif

    ble_mesh_nvs_store(NVS_HANDLE, POWERUP_KEY, g_powerup, sizeof(g_powerup));
}

void load_light_state(void)
{
    ENTER_FUNC();
    uint8_t   i     = 0;
    bool      exist = false;
    esp_err_t ret   = ESP_OK;

    ret = ble_mesh_nvs_restore(NVS_HANDLE, POWERUP_KEY, g_powerup, sizeof(g_powerup), &exist);

    if (ret == ESP_OK) {
        while (i < MESH_ELEM_STATE_COUNT) {
            memcpy(&g_elem_state[i].powerup, &g_powerup[i], sizeof(model_powerup_t));
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            ESP_LOGI(TAG, "elem %d, actual %d", i, g_powerup[i].last_actual);
            if (g_powerup[i].last_actual) {
                g_elem_state[i].state.actual[VALUE_TYPE_TAR] = g_powerup[i].last_actual;
            }
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
            ESP_LOGI(TAG, "elem %d, temp %d", i, g_powerup[i].last_temp);
            if (g_powerup[i].last_temp) {
                g_elem_state[i].state.temp[VALUE_TYPE_TAR] = g_powerup[i].last_temp;
            }
#endif
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            if (g_elem_state[i].state.actual[VALUE_TYPE_TAR] == 0) {
                g_elem_state[i].state.onoff[VALUE_TYPE_TAR] = 0;
            } else {
                g_elem_state[i].state.onoff[VALUE_TYPE_TAR] = 1;
            }
#endif
#ifdef CONFIG_MESH_MODEL_TRANS
            if (g_elem_state[i].state.onoff[VALUE_TYPE_TAR] == 0) {
                g_elem_state[i].state.trans            = 0;
                g_elem_state[i].state.delay            = 0;
                g_elem_state[i].state.trans_start_time = 0;
                g_elem_state[i].state.trans_end_time   = 0;
            } else {
                g_elem_state[i].state.trans            = 0x41;
                g_elem_state[i].state.delay            = 100;
                g_elem_state[i].state.trans_start_time = k_uptime_get() + g_elem_state[i].state.delay * 5;
                g_elem_state[i].state.trans_end_time   = g_elem_state[i].state.trans_start_time + get_transition_time(g_elem_state[i].state.trans);
            }
#endif
#endif
            i++;
        }
    }
}

void user_genie_event_handle(genie_event_t event, void *p_arg)
{
    genie_event_t next_event = event;

    ESP_LOGD(TAG, "event: %02d, %p", event, p_arg);

    switch (event) {
    case GENIE_EVT_RESET_BY_REPEAT_NOTIFY:
        ESP_LOGI(TAG, "GENIE_EVT_RESET_BY_REPEAT_NOTIFY");
        lightbulb_set_switch(false);
        lightbulb_effect_config_t effect1 = {
            .red = 0,
            .green = 255,
            .blue = 0,
            .max_brightness = 100,
            .min_brightness = 0,
            .effect_cycle_ms = CONFIG_LIGHT_BLINK_PERIOD_MS,
            .effect_type = EFFECT_BLINK,
            .mode = WORK_COLOR,
        };
        lightbulb_basic_effect_start(&effect1);   /**< green blink */
        break;
    case GENIE_EVT_SW_RESET:
    case GENIE_EVT_HW_RESET_START:
        ESP_LOGI(TAG, "GENIE_EVT_HW_RESET_START");
        lightbulb_set_switch(false);
        lightbulb_effect_config_t effect2 = {
            .red = 0,
            .green = 255,
            .blue = 0,
            .max_brightness = 100,
            .min_brightness = 0,
            .effect_cycle_ms = CONFIG_LIGHT_BLINK_PERIOD_MS,
            .effect_type = EFFECT_BLINK,
            .mode = WORK_COLOR,
        };
        lightbulb_basic_effect_start(&effect2);   /**< green blink */
        ble_mesh_nvs_erase(NVS_HANDLE, LIGHT_STATUS_STORE_KEY); // erase led status
        reset_light_para();
        break;
    case GENIE_EVT_SDK_MESH_INIT:
        ESP_LOGI(TAG, "GENIE_EVT_SDK_MESH_INIT");
        break;
    case GENIE_EVT_SDK_MESH_PROV_SUCCESS:
        ESP_LOGI(TAG, "GENIE_EVT_SDK_MESH_PROV_SUCCESS");

        g_elem_state[0].state.onoff[VALUE_TYPE_TAR]  = 1;
        g_indication_flag |= INDICATION_FLAG_ONOFF;
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        g_elem_state[0].state.actual[VALUE_TYPE_TAR] = 0xFFFE;
        g_indication_flag |= INDICATION_FLAG_LIGHTNESS;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        g_elem_state[0].state.temp[VALUE_TYPE_TAR]   = 0xFFFE;
        g_indication_flag |= INDICATION_FLAG_CTL;
#endif
        ESP_LOGI(TAG, "light_driver_breath_stop %s", __FUNCTION__);
        lightbulb_basic_effect_stop();
        // update led status
        genie_event(GENIE_EVT_SDK_ANALYZE_MSG, &g_elem_state[0]);
        break;
#ifdef CONFIG_MESH_MODEL_TRANS
    case GENIE_EVT_SDK_TRANS_CYCLE:
#endif
    case GENIE_EVT_SDK_ACTION_DONE: {
        ESP_LOGI(TAG, "GENIE_EVT_SDK_ACTION_DONE");
        elem_state_t *p_elem = (elem_state_t *)p_arg;
#ifdef CONFIG_MESH_MODEL_HSL_SRV
        ESP_LOGI(TAG, "hue: %d, saturation: %d, lightness: %d", p_elem->state.hue[VALUE_TYPE_CUR], p_elem->state.saturation[VALUE_TYPE_CUR], p_elem->state.lightness[VALUE_TYPE_CUR]);
        board_led_hsl(p_elem->elem_index, p_elem->state.hue[VALUE_TYPE_CUR], p_elem->state.saturation[VALUE_TYPE_CUR], p_elem->state.lightness[VALUE_TYPE_CUR]);
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        ESP_LOGI(TAG, "temperature: %d", p_elem->state.temp[VALUE_TYPE_CUR]);
        board_led_temperature(p_elem->elem_index, p_elem->state.temp[VALUE_TYPE_CUR]);
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        ESP_LOGI(TAG, "lightness actual: %d", p_elem->state.actual[VALUE_TYPE_CUR]);
        board_led_lightness(p_elem->elem_index, p_elem->state.actual[VALUE_TYPE_CUR]);
#endif
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        ESP_LOGI(TAG, "onoff: %d", p_elem->state.onoff[VALUE_TYPE_CUR]);
        board_led_switch(p_elem->elem_index, p_elem->state.onoff[VALUE_TYPE_CUR]);
#endif
        if (event == GENIE_EVT_SDK_ACTION_DONE) {
            save_light_state(p_elem);
        }
        break;
    }
    case GENIE_EVT_HW_RESET_DONE:
        ESP_LOGI(TAG, "GENIE_EVT_HW_RESET_DONE");
        ble_mesh_nvs_erase(NVS_HANDLE, LIGHT_STATUS_STORE_KEY); // erase led status
        // esp_restart();
        break;
    case GENIE_EVT_APP_FAC_QUIT:
        ESP_LOGI(TAG, "GENIE_EVT_APP_FAC_QUIT");
        break;
    case GENIE_EVT_BUTTON_TAP:
        ESP_LOGI(TAG, "GENIE_EVT_BUTTON_TAP");
        g_elem_state[0].state.onoff[VALUE_TYPE_TAR] = !g_elem_state[0].state.onoff[VALUE_TYPE_TAR];
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
        g_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
        // update led status
        next_event = GENIE_EVT_SDK_ANALYZE_MSG;
        p_arg = &g_elem_state[0];
        break;
    case GENIE_EVT_SDK_COLOR_ACTION: {
        ESP_LOGI(TAG, "GENIE_EVT_SDK_COLOR_ACTION");
#ifdef CONFIG_MESH_MODEL_HSL_SRV
        uint8_t *p_data = (uint8_t *)p_arg;

        uint16_t lightness = *p_data++;
        lightness += (*p_data++ << 8);

        uint16_t hue = *p_data++;
        hue += (*p_data++ << 8);

        uint16_t saturation = *p_data++;
        saturation += (*p_data++ << 8);

        light_color_set(lightness, hue, saturation);
#endif
        break;
    }
    default:
        ESP_LOGD(TAG, "unhandle this event: %2d", event);
        break;
    }

    if (next_event != event) {
        // send event to genie event loop
        genie_event(next_event, p_arg);
    }
}

esp_err_t local_operation(uint16_t app_idx)
{
    ESP_LOGD(TAG, "app_idx: %d, primary element address: %d", app_idx, esp_ble_mesh_get_primary_element_address());

    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_HEALTH_SRV, GENIE_LIGHT_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_HEALTH_SRV, GENIE_ALL_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_node_bind_app_key_to_local_model(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_HEALTH_SRV, app_idx));

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, GENIE_LIGHT_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, GENIE_ALL_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_node_bind_app_key_to_local_model(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, app_idx));

    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address() + 1, BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, GENIE_LIGHT_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address() + 1, BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, GENIE_ALL_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_node_bind_app_key_to_local_model(esp_ble_mesh_get_primary_element_address() + 1, BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, app_idx));

    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address() + 2, BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, GENIE_LIGHT_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address() + 2, BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, GENIE_ALL_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_node_bind_app_key_to_local_model(esp_ble_mesh_get_primary_element_address() + 2, BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, app_idx));
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, GENIE_LIGHT_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, GENIE_ALL_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_node_bind_app_key_to_local_model(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, app_idx));

    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, GENIE_LIGHT_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, GENIE_ALL_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_node_bind_app_key_to_local_model(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, app_idx));
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV, GENIE_LIGHT_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV, GENIE_ALL_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_node_bind_app_key_to_local_model(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV, app_idx));

    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV, GENIE_LIGHT_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV, GENIE_ALL_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_node_bind_app_key_to_local_model(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV, app_idx));
#endif
#ifdef CONFIG_MESH_MODEL_HSL_SRV
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV, GENIE_LIGHT_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV, GENIE_ALL_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_node_bind_app_key_to_local_model(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV, app_idx));

    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV, GENIE_LIGHT_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV, GENIE_ALL_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_node_bind_app_key_to_local_model(esp_ble_mesh_get_primary_element_address(), BLE_MESH_CID_NVAL, ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV, app_idx));
#endif
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), CID_ALIBABA, GENIE_VENDOR_MODEL_SRV_ID, GENIE_LIGHT_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_model_subscribe_group_addr(esp_ble_mesh_get_primary_element_address(), CID_ALIBABA, GENIE_VENDOR_MODEL_SRV_ID, GENIE_ALL_GROUP_ADDR));
    ESP_ERROR_CHECK(esp_ble_mesh_node_bind_app_key_to_local_model(esp_ble_mesh_get_primary_element_address(), CID_ALIBABA, GENIE_VENDOR_MODEL_SRV_ID, app_idx));

    return ESP_OK;
}

#ifdef CONFIG_MESH_MODEL_HSL_SRV
void bind_onoff_with_hsl(elem_state_t *p_elem, value_type_t type)
{
    ENTER_FUNC();
    model_state_t *p_state = &p_elem->state;

    ESP_LOGI(TAG, "hsl tar(%d, %d, %d)", p_state->hue[VALUE_TYPE_TAR], p_state->saturation[VALUE_TYPE_TAR], p_state->lightness[VALUE_TYPE_TAR]);

    if (type == VALUE_TYPE_TAR) {
        if (p_state->onoff[VALUE_TYPE_TAR] == 0) {
            //turn on
            p_state->onoff[VALUE_TYPE_TAR] = 1;
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
            g_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
        }
    } else if (type == VALUE_TYPE_CUR) {
    }
}
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
void bind_onoff_with_ctl(elem_state_t *p_elem, value_type_t type)
{
    ENTER_FUNC();
    model_state_t *p_state = &p_elem->state;

    // ESP_LOGI(TAG, "temperature cur(%d) tar(%d)", p_state->temp[VALUE_TYPE_CUR], p_state->temp[VALUE_TYPE_TAR]);
    if (type == VALUE_TYPE_TAR) {
        if (p_state->onoff[VALUE_TYPE_TAR] == 0) {
            //turn on
            p_state->onoff[VALUE_TYPE_TAR] = 1;
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
            g_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
        }
    } else if (type == VALUE_TYPE_CUR) {
    }
}
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
void bind_onoff_with_lightness(elem_state_t *p_elem, value_type_t type)
{
    ENTER_FUNC();
    model_state_t *p_state = &p_elem->state;

    ESP_LOGI(TAG, "lightness cur(%d) tar(%d)", p_state->actual[VALUE_TYPE_CUR], p_state->actual[VALUE_TYPE_TAR]);
    if (type == VALUE_TYPE_TAR) {
        if (p_state->actual[VALUE_TYPE_CUR] == 0 && p_state->actual[VALUE_TYPE_TAR] != 0) {
            //turn on
            p_state->onoff[VALUE_TYPE_TAR] = 1;
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
            g_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
        } else if (p_state->actual[VALUE_TYPE_CUR] != 1 && p_state->actual[VALUE_TYPE_TAR] == 0) {
            //turn off
            p_state->onoff[VALUE_TYPE_TAR] = 0;
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
            g_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
        }
    } else if (type == VALUE_TYPE_CUR) {
        if (p_state->actual[VALUE_TYPE_CUR] == 0 && p_state->actual[VALUE_TYPE_TAR] == 0) {
            //turn off
            p_state->onoff[VALUE_TYPE_CUR] = 0;
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
            g_indication_flag |= INDICATION_FLAG_ONOFF;
#endif
        }
    }
}

void bind_lightness_with_onoff(elem_state_t *p_elem, value_type_t type)
{
    ENTER_FUNC();
    model_state_t *p_state = &p_elem->state;

    ESP_LOGI(TAG, "onoff cur(%d) tar(%d)", p_state->onoff[VALUE_TYPE_CUR], p_state->onoff[VALUE_TYPE_TAR]);
    if (type == VALUE_TYPE_TAR) {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        if (p_state->onoff[VALUE_TYPE_CUR] == 0 && p_state->onoff[VALUE_TYPE_TAR] == 1) {
            //turn on, set default lightness
#ifdef CONFIG_MESH_SIMPLE_MODLE
            if (p_elem->powerup.last_actual) {
                p_state->actual[VALUE_TYPE_TAR] = p_elem->powerup.last_actual;
            } else {
                p_state->actual[VALUE_TYPE_TAR] = GENIE_LIGHTNESS_DEFAULT;
            }
#endif
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
            g_indication_flag |= INDICATION_FLAG_LIGHTNESS;
#endif
        } else if (p_state->onoff[VALUE_TYPE_CUR] == 1 && p_state->onoff[VALUE_TYPE_TAR] == 0) {
            //turn off, set zero
            p_state->actual[VALUE_TYPE_TAR] = 0;
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
            g_indication_flag |= INDICATION_FLAG_LIGHTNESS;
#endif
        }
#endif
    } else if (type == VALUE_TYPE_CUR) {
    }
    ESP_LOGI(TAG, "actual cur(%d) tar(%d)", p_state->actual[VALUE_TYPE_CUR], p_state->actual[VALUE_TYPE_TAR]);
}
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
void bind_ctl_with_onoff(elem_state_t *p_elem, value_type_t type)
{
    ENTER_FUNC();
    model_state_t *p_state = &p_elem->state;

    ESP_LOGI(TAG, "onoff cur(%d) tar(%d)", p_state->onoff[VALUE_TYPE_CUR], p_state->onoff[VALUE_TYPE_TAR]);
    if (type == VALUE_TYPE_TAR) {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        if (p_state->onoff[VALUE_TYPE_CUR] == 0 && p_state->onoff[VALUE_TYPE_TAR] == 1) {
#ifdef CONFIG_MESH_SIMPLE_MODLE
            //turn on, check temperature
            ESP_LOGI(TAG, "temp cur(%d) tar(%d)", p_state->temp[VALUE_TYPE_CUR], p_state->temp[VALUE_TYPE_TAR]);
            if (p_state->temp[VALUE_TYPE_CUR] != p_state->temp[VALUE_TYPE_TAR]) {
                if (p_elem->powerup.last_temp) {
                    p_state->temp[VALUE_TYPE_TAR] = p_elem->powerup.last_temp;
                } else {
                    p_state->temp[VALUE_TYPE_TAR] = GENIE_CTL_TEMP_DEFAULT;
                }
#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
                g_indication_flag |= INDICATION_FLAG_CTL;
#endif
            }
#endif
        }
#endif
    } else if (type == VALUE_TYPE_CUR) {
    }
    ESP_LOGI(TAG, "temp cur(%04x) tar(%04x)", p_state->temp[VALUE_TYPE_CUR], p_state->temp[VALUE_TYPE_TAR]);
}
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
static void light_color_set(uint16_t lightness, uint16_t hue, uint16_t saturation)
{
    ENTER_FUNC();
    ESP_LOGI(TAG, "hue: %d, saturation: %d, lightness: %d", hue, saturation, lightness);

    g_elem_state[0].state.hue[VALUE_TYPE_TAR]        = hue;
    g_elem_state[0].state.saturation[VALUE_TYPE_TAR] = saturation;
    g_elem_state[0].state.lightness[VALUE_TYPE_TAR]  = lightness;
    if (g_elem_state[0].state.hue[VALUE_TYPE_CUR] != g_elem_state[0].state.hue[VALUE_TYPE_TAR] || g_elem_state[0].state.saturation[VALUE_TYPE_CUR] != g_elem_state[0].state.saturation[VALUE_TYPE_TAR]
            || g_elem_state[0].state.lightness[VALUE_TYPE_CUR] != g_elem_state[0].state.lightness[VALUE_TYPE_TAR]) {
        g_indication_flag |= INDICATION_FLAG_HSL;

        /* Change corresponding binded states in root element */
        bind_onoff_with_hsl(&g_elem_state[0], VALUE_TYPE_TAR);

        if (esp_ble_mesh_node_is_provisioned()) {
            // update led status
            genie_event(GENIE_EVT_SDK_ANALYZE_MSG, &g_elem_state[0]);
        }
    }
}
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
static void light_ctl_set(uint32_t opcode, esp_ble_mesh_model_t *model,
                          esp_ble_mesh_msg_ctx_t *ctx,
                          uint16_t lightness, uint16_t temperature, uint16_t delta_uv)
{
    ENTER_FUNC();
    ESP_LOGI(TAG, "lightness 0x%d", lightness);

    if (opcode == ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_SET) {
        esp_ble_mesh_server_model_send_msg(model, ctx, ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS,
                                           sizeof(lightness), (uint8_t *)&lightness);
    }

    g_elem_state[0].state.actual[VALUE_TYPE_TAR] = lightness;
    g_elem_state[0].state.temp[VALUE_TYPE_TAR] = temperature;
    if (g_elem_state[0].state.actual[VALUE_TYPE_CUR] != g_elem_state[0].state.actual[VALUE_TYPE_TAR] || g_elem_state[0].state.temp[VALUE_TYPE_CUR] != g_elem_state[0].state.temp[VALUE_TYPE_TAR]) {
        g_indication_flag |= INDICATION_FLAG_LIGHTNESS;
        g_indication_flag |= INDICATION_FLAG_CTL;

        esp_ble_mesh_server_model_send_msg(model, ctx, ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS,
                                           sizeof(g_elem_state[0].state.temp[VALUE_TYPE_CUR]), (uint8_t *)&g_elem_state[0].state.temp[VALUE_TYPE_CUR]);

        /* Change corresponding binded states in root element */
        bind_onoff_with_ctl(&g_elem_state[0], VALUE_TYPE_TAR);

        if (esp_ble_mesh_node_is_provisioned()) {
            // update led status
            genie_event(GENIE_EVT_SDK_ANALYZE_MSG, &g_elem_state[0]);
        }
    }
}
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
static void light_lightness_actual_set(uint32_t opcode, esp_ble_mesh_model_t *model,
                                       esp_ble_mesh_msg_ctx_t *ctx,
                                       uint16_t lightness)
{
    ENTER_FUNC();
    ESP_LOGI(TAG, "lightness %d", lightness);

    if (opcode == ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET) {
        esp_ble_mesh_server_model_send_msg(model, ctx, ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS,
                                           sizeof(lightness), (uint8_t *)&lightness);
    }

    g_elem_state[0].state.actual[VALUE_TYPE_TAR] = lightness;
    if (g_elem_state[0].state.actual[VALUE_TYPE_CUR] != g_elem_state[0].state.actual[VALUE_TYPE_TAR]) {
        g_indication_flag |= INDICATION_FLAG_LIGHTNESS;

        esp_ble_mesh_server_model_send_msg(model, ctx, ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS,
                                           sizeof(g_elem_state[0].state.actual[VALUE_TYPE_CUR]), (uint8_t *)&g_elem_state[0].state.actual[VALUE_TYPE_CUR]);

        /* Change corresponding binded states in root element */
        bind_onoff_with_lightness(&g_elem_state[0], VALUE_TYPE_TAR);

        if (esp_ble_mesh_node_is_provisioned()) {
            // update led status
            genie_event(GENIE_EVT_SDK_ANALYZE_MSG, &g_elem_state[0]);
        }
    }
}

static void light_lightness_linear_set(uint32_t opcode, esp_ble_mesh_model_t *model,
                                       esp_ble_mesh_msg_ctx_t *ctx,
                                       uint16_t lightness)
{
    ENTER_FUNC();
    ESP_LOGI(TAG, "lightness 0x%d", lightness);

    if (opcode == ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET) {
        esp_ble_mesh_server_model_send_msg(model, ctx, ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS,
                                           sizeof(lightness), (uint8_t *)&lightness);
    }

    g_elem_state[0].state.linear[VALUE_TYPE_TAR] = lightness;
    if (g_elem_state[0].state.linear[VALUE_TYPE_CUR] != g_elem_state[0].state.linear[VALUE_TYPE_TAR]) {
        g_indication_flag |= INDICATION_FLAG_LIGHTNESS;

        esp_ble_mesh_server_model_send_msg(model, ctx, ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS,
                                           sizeof(g_elem_state[0].state.linear[VALUE_TYPE_CUR]), (uint8_t *)&g_elem_state[0].state.linear[VALUE_TYPE_CUR]);

        /* Change corresponding binded states in root element */
        bind_onoff_with_lightness(&g_elem_state[0], VALUE_TYPE_TAR);

        if (esp_ble_mesh_node_is_provisioned()) {
            // update led status
            genie_event(GENIE_EVT_SDK_ANALYZE_MSG, &g_elem_state[0]);
        }
    }
}
#endif

static void light_onoff_set(esp_ble_mesh_model_t *model,
                            esp_ble_mesh_msg_ctx_t *ctx, uint8_t onoff)
{
    ENTER_FUNC();
    uint16_t primary_addr    = esp_ble_mesh_get_primary_element_address();
    uint8_t  elem_count      = esp_ble_mesh_get_element_count();
    elem_state_t *led_state = NULL;

    g_elem_state[0].state.onoff[VALUE_TYPE_TAR] = onoff;
    if (g_elem_state[0].state.onoff[VALUE_TYPE_CUR] != g_elem_state[0].state.onoff[VALUE_TYPE_TAR]) {
        g_indication_flag |= INDICATION_FLAG_ONOFF;
        // g_elem_state[0].state.onoff[VALUE_TYPE_CUR] = g_elem_state[0].state.onoff[VALUE_TYPE_TAR];

        if (ESP_BLE_MESH_ADDR_IS_UNICAST(ctx->recv_dst)) {
            for (uint8_t i = 0; i < elem_count; i++) {
                if (ctx->recv_dst == (primary_addr + i)) {
                    led_state = &g_elem_state[i];
                }
            }
        } else if (ESP_BLE_MESH_ADDR_IS_GROUP(ctx->recv_dst)) {
            if (esp_ble_mesh_is_model_subscribed_to_group(model, ctx->recv_dst)) {
                led_state = &g_elem_state[model->element->element_addr - primary_addr];
            }
        } else if (ctx->recv_dst == 0xFFFF) {
            led_state = &g_elem_state[model->element->element_addr - primary_addr];
        }

        if (led_state != NULL) {
            ESP_LOGI(TAG, "led onoff 0x%d", g_elem_state[0].state.onoff[VALUE_TYPE_TAR]);
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
            // update lightness
            bind_lightness_with_onoff(&g_elem_state[0], VALUE_TYPE_TAR);
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
            // update ctl
            bind_ctl_with_onoff(&g_elem_state[0], VALUE_TYPE_TAR);
#endif
            // update led status
            genie_event(GENIE_EVT_SDK_ANALYZE_MSG, &g_elem_state[0]);
        }
    }
}

static void prov_complete(uint16_t net_idx, uint16_t addr, uint8_t flags, uint32_t iv_index)
{
    ENTER_FUNC();
    ESP_LOGI(TAG, "net_idx: 0x%04x, addr: 0x%04x", net_idx, addr);
    ESP_LOGI(TAG, "flags: 0x%02x, iv_index: 0x%08" PRIx32, flags, iv_index);
}

static void example_ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event,
        esp_ble_mesh_prov_cb_param_t *param)
{
    switch (event) {
    case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROV_REGISTER_COMP_EVT, err_code %d", param->prov_register_comp.err_code);
        break;
    case ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT, err_code %d", param->node_prov_enable_comp.err_code);
        // send event to genie event loop
        genie_event(GENIE_EVT_SDK_MESH_INIT, &param->node_prov_enable_comp.err_code);

        if (param->node_prov_enable_comp.err_code == -EALREADY) {
            // genie mesh init
            genie_mesh_init();
        }
        break;
    case ESP_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT");
        break;
    case ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT, bearer %s",
                 param->node_prov_link_open.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");

        // send event to genie event loop
        genie_event(GENIE_EVT_SDK_MESH_PROV_START, NULL);

        break;
    case ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT, bearer %s",
                 param->node_prov_link_close.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
        break;
    case ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT: // already have configuration when device restart, and provisioning completed.
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT");
        ESP_LOG_BUFFER_HEX("NetKey: ", param->node_prov_complete.net_key, 16);
        ESP_LOGI(TAG, "net_idx 0x%04x, NetKey: %s, Primary address: 0x%04x", param->node_prov_complete.net_idx,
                 util_hex2str(param->node_prov_complete.net_key, 16), param->node_prov_complete.addr);

        prov_complete(param->node_prov_complete.net_idx, param->node_prov_complete.addr,
                      param->node_prov_complete.flags, param->node_prov_complete.iv_index);

        user_prov_complete(param->node_prov_complete.net_idx, param->node_prov_complete.addr);
        break;
    case ESP_BLE_MESH_NODE_PROV_RESET_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_RESET_EVT");

        // send event to genie event loop
        genie_event(GENIE_EVT_SW_RESET, NULL);

        user_prov_reset();
        break;
    case ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT, err_code %d", param->node_set_unprov_dev_name_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_STORE_NODE_COMP_DATA_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_STORE_NODE_COMP_DATA_COMP_EVT, err_code %d", param->provisioner_store_node_comp_data_comp.err_code);
        break;
    case ESP_BLE_MESH_MODEL_SUBSCRIBE_GROUP_ADDR_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_SUBSCRIBE_GROUP_ADDR_COMP_EVT, err_code %d, element_addr 0x%04x, company_id 0x%04x, model_id 0x%04x, group_addr 0x%04x",
                 param->model_sub_group_addr_comp.err_code, param->model_sub_group_addr_comp.element_addr,
                 param->model_sub_group_addr_comp.company_id, param->model_sub_group_addr_comp.model_id,
                 param->model_sub_group_addr_comp.group_addr);
        break;
    case ESP_BLE_MESH_NODE_BIND_APP_KEY_TO_MODEL_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_BIND_APP_KEY_TO_MODEL_COMP_EVT, err_code %d, element_addr 0x%04x, company_id 0x%04x, model_id 0x%04x, app_idx 0x%04x",
                 param->node_bind_app_key_to_model_comp.err_code, param->node_bind_app_key_to_model_comp.element_addr,
                 param->node_bind_app_key_to_model_comp.company_id, param->node_bind_app_key_to_model_comp.model_id,
                 param->node_bind_app_key_to_model_comp.app_idx);
        break;
    default:
        ESP_LOGI(TAG, "event %d", event);
        break;
    }
}

static void example_ble_mesh_config_server_cb(esp_ble_mesh_cfg_server_cb_event_t event,
        esp_ble_mesh_cfg_server_cb_param_t *param)
{
    if (event == ESP_BLE_MESH_CFG_SERVER_STATE_CHANGE_EVT) {
        switch (param->ctx.recv_op) {
        case ESP_BLE_MESH_MODEL_OP_NET_KEY_ADD:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_NET_KEY_ADD");
            ESP_LOGI(TAG, "net_idx 0x%04x, NetKey: %s",
                     param->value.state_change.netkey_add.net_idx,
                     util_hex2str(param->value.state_change.netkey_add.net_key, 16));
            break;
        case ESP_BLE_MESH_MODEL_OP_NET_KEY_UPDATE:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_NET_KEY_UPDATE");
            break;
        case ESP_BLE_MESH_MODEL_OP_NET_KEY_DELETE:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_NET_KEY_DELETE");
            break;

        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD");
            ESP_LOGI(TAG, "net_idx 0x%04x, app_idx 0x%04x, AppKey: %s",
                     param->value.state_change.appkey_add.net_idx, param->value.state_change.appkey_add.app_idx,
                     util_hex2str(param->value.state_change.appkey_add.app_key, 16));

            // send event to genie event loop
            genie_event(GENIE_EVT_SDK_APPKEY_ADD, &param->value.state_change.appkey_add.app_idx);

#if GENIE_VENDOR_MODEL_VERSION == 0

#elif GENIE_VENDOR_MODEL_VERSION == 1
            // local bind AppKEY and Subcribe Group Address
            local_operation(param->value.state_change.appkey_add.app_idx);
            // genie mesh init
            genie_mesh_init();
#endif
            break;
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_UPDATE:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_APP_KEY_UPDATE");
            break;
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_DELETE:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_APP_KEY_DELETE");
            break;

        case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND");
            ESP_LOGI(TAG, "elem_addr 0x%04x, app_idx 0x%04x, cid 0x%04x, mod_id 0x%04x",
                     param->value.state_change.mod_app_bind.element_addr,
                     param->value.state_change.mod_app_bind.app_idx,
                     param->value.state_change.mod_app_bind.company_id,
                     param->value.state_change.mod_app_bind.model_id);
            break;

        case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD");
            ESP_LOGI(TAG, "elem_addr 0x%04x, sub_addr 0x%04x, cid 0x%04x, mod_id 0x%04x",
                     param->value.state_change.mod_sub_add.element_addr,
                     param->value.state_change.mod_sub_add.sub_addr,
                     param->value.state_change.mod_sub_add.company_id,
                     param->value.state_change.mod_sub_add.model_id);
#if GENIE_VENDOR_MODEL_VERSION == 0
            // genie mesh init
            genie_mesh_init();
#elif GENIE_VENDOR_MODEL_VERSION == 1

#endif
            break;
        case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE");
            break;

        case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET");
            break;
        case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET");
            break;
        default:
            ESP_LOGW(TAG, "%s, event %d", __FUNCTION__, event);
            break;
        }
    }
}

static void example_ble_mesh_generic_server_cb(esp_ble_mesh_generic_server_cb_event_t event,
        esp_ble_mesh_generic_server_cb_param_t *param)
{
    ESP_LOGD(TAG, "event 0x%02x, opcode 0x%04" PRIx32 ", src 0x%04x, dst 0x%04x",
             event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);

    switch (event) {
    case ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT");
        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET ||
                param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {

            light_onoff_set(param->model, &param->ctx, param->value.state_change.onoff_set.onoff);

            /* Update bound states second way */
            // light_lightness_actual_set(param->ctx.recv_op, param->model,
            //     &param->ctx, param->value.state_change.onoff_set.onoff);
        }
        break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT");
        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET) {
            esp_ble_mesh_gen_onoff_srv_t *srv = (esp_ble_mesh_gen_onoff_srv_t *)param->model->user_data;
            ESP_LOGI(TAG, "onoff: 0x%02x", srv->state.onoff);
            esp_ble_mesh_server_model_send_msg(param->model, &param->ctx,
                                               ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS, sizeof(srv->state.onoff), &srv->state.onoff);
        }
        break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT");
        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET ||
                param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
            ESP_LOGI(TAG, "onoff 0x%02x, tid 0x%02x", param->value.set.onoff.onoff, param->value.set.onoff.tid);
            if (param->value.set.onoff.op_en) {
                ESP_LOGI(TAG, "trans_time 0x%02x, delay 0x%02x",
                         param->value.set.onoff.trans_time, param->value.set.onoff.delay);
            }

            esp_ble_mesh_gen_onoff_srv_t *srv = (esp_ble_mesh_gen_onoff_srv_t *)param->model->user_data;
            if (param->value.set.onoff.op_en == false) {
                srv->state.onoff = param->value.set.onoff.onoff;
            } else {
                /* TODO: Delay and state transition */
                srv->state.onoff = param->value.set.onoff.onoff;
            }
            g_indication_flag |= INDICATION_FLAG_ONOFF;
            if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
                esp_ble_mesh_server_model_send_msg(param->model, &param->ctx,
                                                   ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS, sizeof(srv->state.onoff), &srv->state.onoff);
            }

            light_onoff_set(param->model, &param->ctx, srv->state.onoff);
        }
        break;
    default:
        ESP_LOGW(TAG, "Unknown Generic Server event 0x%02x", event);
        break;
    }
}

static void example_ble_mesh_lighting_server_cb(esp_ble_mesh_lighting_server_cb_event_t event,
        esp_ble_mesh_lighting_server_cb_param_t *param)
{
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    uint16_t lightness;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    uint16_t temperature;
    uint16_t delta_uv;
#endif
    esp_ble_mesh_server_state_value_t state = {0};

    ESP_LOGD(TAG, "event 0x%02x, opcode 0x%04" PRIx32 ", src 0x%04x, dst 0x%04x",
             event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);

    switch (event) {
    case ESP_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT:
        switch (param->ctx.recv_op) {
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
        /*!< Light Lightness Message Opcode */
        case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
        case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK: {
            /* Light Lightness Server Model - Primary Element */
            ESP_LOGI(TAG, "lightness actual %d", param->value.state_change.lightness_set.lightness);
            /* Update bound states */
            lightness = param->value.state_change.lightness_set.lightness;
            state.light_lightness_linear.lightness = convert_lightness_actual_to_linear(lightness);
            esp_ble_mesh_server_model_update_state(lightness_server.model, ESP_BLE_MESH_LIGHT_LIGHTNESS_LINEAR_STATE, &state);
            state.gen_onoff.onoff = lightness ? LED_ON : LED_OFF;
            esp_ble_mesh_server_model_update_state(onoff_server_0.model, ESP_BLE_MESH_GENERIC_ONOFF_STATE, &state);

            /* Update bound states second way */
            light_lightness_actual_set(param->ctx.recv_op, param->model,
                                       &param->ctx, lightness);
            break;
        }
        case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET:
        case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK: {
            /* Light Lightness Server Model - Primary Element */
            ESP_LOGI(TAG, "lightness linear %d", param->value.state_change.lightness_linear_set.lightness);
            /* Update bound states */
            lightness = convert_lightness_linear_to_actual(param->value.state_change.lightness_linear_set.lightness);
            state.light_lightness_actual.lightness = lightness;
            esp_ble_mesh_server_model_update_state(lightness_server.model, ESP_BLE_MESH_LIGHT_LIGHTNESS_ACTUAL_STATE, &state);
            state.gen_onoff.onoff = lightness ? LED_ON : LED_OFF;
            esp_ble_mesh_server_model_update_state(onoff_server_0.model, ESP_BLE_MESH_GENERIC_ONOFF_STATE, &state);

            /* Update bound states second way */
            light_lightness_linear_set(param->ctx.recv_op, param->model,
                                       &param->ctx, lightness);
            break;
        }
        /*!< Light Lightness Setup Message Opcode */
        case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET:
        case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK:
            /* Light Lightness Setup Server Model - Primary Element */
            ESP_LOGI(TAG, "lightness default %d", param->value.state_change.lightness_default_set.lightness);
            break;
        case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET:
        case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK:
            /* Light Lightness Setup Server Model - Primary Element */
            ESP_LOGI(TAG, "lightness min %d, max %d",
                     param->value.state_change.lightness_range_set.range_min,
                     param->value.state_change.lightness_range_set.range_max);
            break;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
        /*!< Light CTL Message Opcode */
        case ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_SET:
        case ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK:
            /* Light CTL Server Model - Primary Element */
            ESP_LOGI(TAG, "lightness 0x%04x, temperature 0x%04x, delta uv 0x%04x",
                     param->value.state_change.ctl_set.lightness,
                     param->value.state_change.ctl_set.temperature,
                     param->value.state_change.ctl_set.delta_uv);
            /* Update bound states */
            lightness = param->value.state_change.ctl_set.lightness;
            temperature = param->value.state_change.ctl_set.temperature;
            delta_uv = param->value.state_change.ctl_set.delta_uv;
            state.light_lightness_actual.lightness = lightness;
            esp_ble_mesh_server_model_update_state(lightness_server.model, ESP_BLE_MESH_LIGHT_LIGHTNESS_ACTUAL_STATE, &state);
            state.light_lightness_linear.lightness = convert_lightness_actual_to_linear(lightness);
            esp_ble_mesh_server_model_update_state(lightness_server.model, ESP_BLE_MESH_LIGHT_LIGHTNESS_LINEAR_STATE, &state);
            state.gen_onoff.onoff = lightness ? LED_ON : LED_OFF;
            esp_ble_mesh_server_model_update_state(onoff_server_0.model, ESP_BLE_MESH_GENERIC_ONOFF_STATE, &state);

            /* Update bound states second way */
            light_ctl_set(param->ctx.recv_op, param->model,
                          &param->ctx, lightness, temperature, delta_uv);
            break;
        case ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET:
        case ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK:
            /* Light CTL Temperature Server Model - Secondary Element */
            ESP_LOGI(TAG, "temperature 0x%04x, delta uv 0x%04x",
                     param->value.state_change.ctl_temp_set.temperature,
                     param->value.state_change.ctl_temp_set.delta_uv);
            break;
        /*!< Light CTL Setup Message Opcode */
        case ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET:
        case ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK:
            /* Light CTL Setup Server Model - Primary Element */
            ESP_LOGI(TAG, "lightness 0x%04x, temperature 0x%04x, delta uv 0x%04x",
                     param->value.state_change.ctl_default_set.lightness,
                     param->value.state_change.ctl_default_set.temperature,
                     param->value.state_change.ctl_default_set.delta_uv);
            break;
        case ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET:
        case ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK:
            /* Light CTL Setup Server Model - Primary Element */
            ESP_LOGI(TAG, "temperature min 0x%04x, max 0x%04x",
                     param->value.state_change.ctl_temp_range_set.range_min,
                     param->value.state_change.ctl_temp_range_set.range_max);
            break;
#endif
        }
        break;
    default:
        ESP_LOGE(TAG, "Unknown Lighting Server event 0x%02x", event);
        break;
    }
}

static void example_ble_mesh_custom_model_cb(esp_ble_mesh_model_cb_event_t event,
        esp_ble_mesh_model_cb_param_t *param)
{
    ESP_LOGD(TAG, "event 0x%02x", event);

    switch (event) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT:
        ESP_LOGD(TAG, "ESP_BLE_MESH_MODEL_OPERATION_EVT");
        ESP_LOGD(TAG, "opcode: 0x%06" PRIx32, param->model_operation.opcode);
        genie_model_dispatch(param->model_operation.opcode, param->model_operation.model,
                             param->model_operation.ctx, param->model_operation.msg, param->model_operation.length);
        break;
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT:
        ESP_LOGD(TAG, "ESP_BLE_MESH_MODEL_SEND_COMP_EVT");
        ESP_LOGD(TAG, "opcode: 0x%06" PRIx32, param->model_send_comp.opcode);
        if (param->model_send_comp.err_code) {
            ESP_LOGE(TAG, "Failed to send message: 0x%06" PRIx32 ", err: %d", param->model_send_comp.opcode, param->model_send_comp.err_code);
            break;
        }
        break;
    case ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT:
        ESP_LOGD(TAG, "ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT");
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT:
        ESP_LOGD(TAG, "ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT");
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT:
        ESP_LOGD(TAG, "ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT");
        break;
    case ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT:
        ESP_LOGD(TAG, "ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT");
        break;
    case ESP_BLE_MESH_SERVER_MODEL_UPDATE_STATE_COMP_EVT:
        ESP_LOGD(TAG, "ESP_BLE_MESH_SERVER_MODEL_UPDATE_STATE_COMP_EVT");
        ESP_LOGD(TAG, "result %d, model id 0x%04x, type 0x%02x",
                 param->server_model_update_state.err_code,
                 param->server_model_update_state.model->model_id,
                 param->server_model_update_state.type);
        break;
    case ESP_BLE_MESH_MODEL_EVT_MAX:
        ESP_LOGW(TAG, "ESP_BLE_MESH_MODEL_EVT_MAX");
        break;
    default:
        break;
    }
}

static esp_err_t ble_mesh_init(void)
{
    esp_err_t err;

    ESP_ERROR_CHECK(esp_ble_mesh_register_prov_callback(example_ble_mesh_provisioning_cb));
    ESP_ERROR_CHECK(esp_ble_mesh_register_config_server_callback(example_ble_mesh_config_server_cb));
    ESP_ERROR_CHECK(esp_ble_mesh_register_generic_server_callback(example_ble_mesh_generic_server_cb));
    ESP_ERROR_CHECK(esp_ble_mesh_register_lighting_server_callback(example_ble_mesh_lighting_server_cb));
    ESP_ERROR_CHECK(esp_ble_mesh_register_custom_model_callback(example_ble_mesh_custom_model_cb));

    err = esp_ble_mesh_init(&provision, &composition);
    if (err) {
        ESP_LOGE(TAG, "Initializing mesh failed (err %d)", err);
        return err;
    }

    ESP_ERROR_CHECK(esp_ble_mesh_node_prov_enable((esp_ble_mesh_prov_bearer_t)(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT)));

    if (esp_ble_mesh_node_is_provisioned()) {
        ESP_LOGW(TAG, "node already provisioned");
    } else {
        ESP_LOGW(TAG, "node not provisioned");
        lightbulb_set_switch(false);
        lightbulb_effect_config_t effect3 = {
            .red = 0,
            .green = 255,
            .blue = 0,
            .max_brightness = 100,
            .min_brightness = 0,
            .effect_cycle_ms = CONFIG_LIGHT_BLINK_PERIOD_MS,
            .effect_type = EFFECT_BLINK,
            .mode = WORK_COLOR,
        };
        lightbulb_basic_effect_start(&effect3); /**< green blink */
    }

    return err;
}

void config_triples(void)
{
    // AuthValue = SHA256(Product ID,MAC,Secret)
    char *authvalue_string = NULL;
    asprintf(&authvalue_string, "%08x,%s,%s", CONFIG_TRIPLES_PRODUCT_ID, CONFIG_TRIPLES_DEVICE_NAME, CONFIG_TRIPLES_DEVICE_SECRET);
    ESP_LOGI(TAG, "authvalue_string: %s", authvalue_string);

    uint8_t sha256_out[32] = {0};
    mbedtls_sha256((const unsigned char *)authvalue_string, strlen(authvalue_string), sha256_out, 0);
    memcpy(static_val, sha256_out, 16);
    provision.static_val = static_val;

    uint8_t product_id[4];
    mac_str2hex(authvalue_string, product_id);
    swap_buf(dev_uuid + 3, product_id, 4);

    uint8_t ali_mac_address[6];
    mac_str2hex(CONFIG_TRIPLES_DEVICE_NAME, ali_mac_address);
    swap_buf(dev_uuid + 7, ali_mac_address, 6);
    ali_mac_address[5] -= 2;
    esp_base_mac_addr_set(ali_mac_address);

    free(authvalue_string);
}

void app_main(void)
{
    esp_err_t err;

    esp_log_level_set("genie_event", ESP_LOG_DEBUG);
    ESP_LOGI(TAG, "Initializing...");

    board_init();

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_LOGI(TAG, "nvs_flash_erase...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* Open nvs namespace for storing/restoring mesh example info */
    err = ble_mesh_nvs_open(&NVS_HANDLE);
    if (err) {
        ESP_LOGE(TAG, "ble_mesh_nvs_open failed (err %d)", err);
        return;
    }

#ifdef CONFIG_GENIE_RESET_BY_REPEAT
    genie_reset_by_repeat_init();
#endif

    config_triples();

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
    }
}
