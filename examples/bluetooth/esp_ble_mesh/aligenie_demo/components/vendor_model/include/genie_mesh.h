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

#ifndef _GENIE_MESH_H_
#define _GENIE_MESH_H_

#include "esp_timer.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"

#include "genie_slist.h"
#include "genie_dlist.h"

#include "genie_util.h"
#include "genie_event.h"
#include "genie_reset.h"
#include "genie_timer.h"
#include "genie_model_srv.h"

#ifdef __cplusplus
extern "C" {
#endif /**< __cplusplus */

#define CID_ALIBABA                     0x01A8  /**< Alibaba Incorporated */
#define GENIE_VENDOR_MODEL_SRV_ID       0x0000  /**< Genie Vendor Model Server ID */
#define GENIE_VENDOR_MODEL_CLI_ID       0x0001  /**< Genie Vendor Model Client ID */

#define GENIE_RECV_ADDR                 0xF000  /**< Genie Receive Address */
#define GENIE_OTA_GROUP_ADDR            0xF100  /**< Genie OTA Group Address */
#define GENIE_ALL_GROUP_ADDR            0xCFFF  /**< Genie All Product Group Address */

#define GENIE_LIGHT_GROUP_ADDR          0xC000  /**< Genie Light Product Group Address */
#define GENIE_SWITCH_GROUP_ADDR         0xC001  /**< Genie Switch Product Group Address */

#define GENIE_OP_ATTR_GET_STATUS       (0xD0)
#define GENIE_OP_ATTR_SET_ACK          (0xD1)
#define GENIE_OP_ATTR_SET_UNACK        (0xD2)
#define GENIE_OP_ATTR_STATUS           (0xD3)
#define GENIE_OP_ATTR_INDICATE         (0xD4)
#define GENIE_OP_ATTR_CONFIME          (0xD5)
#define GENIE_OP_ATTR_INDICATE_TG      (0xDE)
#define GENIE_OP_ATTR_CONFIME_TG       (0xDF)
#define GENIE_OP_ATTR_TRANS_MSG        (0xCF)
#define GENIE_OP_ATTR_TRANS_INDICATE   (0xCE)
#define GENIE_OP_ATTR_TRANS_ACK        (0xCD)

#define GENIE_MESSAGE_OP_ATTR_GET_STATUS       ESP_BLE_MESH_MODEL_OP_3(GENIE_OP_ATTR_GET_STATUS,  CID_ALIBABA)
#define GENIE_MESSAGE_OP_ATTR_SET_ACK          ESP_BLE_MESH_MODEL_OP_3(GENIE_OP_ATTR_SET_ACK,     CID_ALIBABA)
#define GENIE_MESSAGE_OP_ATTR_SET_UNACK        ESP_BLE_MESH_MODEL_OP_3(GENIE_OP_ATTR_SET_UNACK,   CID_ALIBABA)
#define GENIE_MESSAGE_OP_ATTR_STATUS           ESP_BLE_MESH_MODEL_OP_3(GENIE_OP_ATTR_STATUS,      CID_ALIBABA)
#define GENIE_MESSAGE_OP_ATTR_INDICATION       ESP_BLE_MESH_MODEL_OP_3(GENIE_OP_ATTR_INDICATE,    CID_ALIBABA)
#define GENIE_MESSAGE_OP_ATTR_CONFIRMATION     ESP_BLE_MESH_MODEL_OP_3(GENIE_OP_ATTR_CONFIME,     CID_ALIBABA)
#define GENIE_MESSAGE_OP_ATTR_INDICATION_TG    ESP_BLE_MESH_MODEL_OP_3(GENIE_OP_ATTR_INDICATE_TG, CID_ALIBABA)
#define GENIE_MESSAGE_OP_ATTR_CONFIRMATION_TG  ESP_BLE_MESH_MODEL_OP_3(GENIE_OP_ATTR_CONFIME_TG,  CID_ALIBABA)
#define GENIE_MESSAGE_OP_ATTR_TRANSPARENT_MSG  ESP_BLE_MESH_MODEL_OP_3(GENIE_OP_ATTR_TRANS_MSG,   CID_ALIBABA)
#define GENIE_MESSAGE_OP_ATTR_TRANSPARENT_ACK  ESP_BLE_MESH_MODEL_OP_3(GENIE_OP_ATTR_TRANS_ACK,   CID_ALIBABA)

#define GENIE_VENDOR_MODEL_VERSION      1               /**< Support UUID version */
#define GENIE_PROVISIONING_TIMEOUT      (60 * 1000)     /**< Mesh Provision Timeout, unit: ms */
#define GENIE_PBADV_TIMEOUT             (600 * 1000)    /**< Mesh Provision Advertise Timeout, unit: ms */

#define GENIE_STATIC_OOB_LENGTH         16
#define GENIE_MESH_TRNSATION_CYCLE      10

#define GENIE_UNPROV_ADV_FEATURE_AUTO_BIND_MODEL_SUB  0x02
#define GENIE_UNPROV_ADV_FEATURE_SILENT_ADV           0x01
#define GENIE_UNPROV_ADV_FLAG_GENIE_MESH_STACK        0x10  // bit4-7
#define GENIE_UNPROV_ADV_FEATURE_ULTRA_PROV           0x03  // bit0-1

#define GENIE_CTL_TEMP_MIN              800
#define GENIE_CTL_TEMP_MAX              20000
#define GENIE_CTL_TEMP_DEFAULT          GENIE_CTL_TEMP_MAX

#define GENIE_ONOFF_DEFAULT             1
#define GENIE_LIGHTNESS_DEFAULT         0xE666  // 90%

/* Vendor Model Attr Parameter */
#define GENIE_MODEL_ATTR_ONOFF           0x0100
#define GENIE_MODEL_ATTR_LIGHTNESS       0x0121
#define GENIE_MODEL_ATTR_TEMPERATURE     0x0122
#define GENIE_MODEL_ATTR_COLOR           0x0123
#define GENIE_MODEL_ATTR_DEVICE_EVENT    0xF009

/* Event List - Refer to Device Event List defined in https://yuque.antfin-inc.com/iotconnect/wwfb/dbyytw#1c38cf1b */
#define EVENT_FAULT             0x00    /* malfunction event */
#define EVENT_LOW_BAT           0x01    /* low battery event */
#define EVENT_DEV_UP            0x03    /* device power up */
#define EVENT_HW_RESET          0x23    /* hardware reset event */
#define EVENT_TIMING_TIMEOUT    0x11    /* timer timeout event */

/* Vendor timer error code */
#define ERR_CODE_UNIXTIME               0x80
#define ERR_CODE_NOTSUP_ATTR_OP         0x82
#define ERR_CODE_NOTSUP_ATTR_PARAM      0x83
#define ERR_CODE_TIMER_SETTING          0x84
#define ERR_CODE_TIMER_INDEX            0x85
#define ERR_CODE_TIMER_FULL             0x86
#define ERR_CODE_TIMER_PRIORDIC_PARAM   0x87

/* Vendor timer ali attr type */
#define TIMER_ATTR_ERROR_CODE                0x0000
#define TIMER_ATTR_TIMING_TIMEOUT            0xF009
#define TIMER_ATTR_TIMING_SETTING            0xF010
#define TIMER_ATTR_TIMING_PERIODIC_SETTING   0xF011
#define TIMER_ATTR_TIMING_DELETE             0xF012
#define TIMER_ATTR_TIMING_SYNC               0xF01D
#define TIMER_ATTR_TIMEZONE_SETTING          0xF01E
#define TIMER_ATTR_UNIX_TIME                 0xF01F

#define GENIE_MODEL_MSG_DFT_RETRY_TIMES    6
#define GENIE_MODEL_MSG_MAX_RETRY_TIMES    10
#define GENIE_MODEL_MSG_RETRY_PERIOD       400

#define CONFIG_MESH_SIMPLE_MODLE        1
#define GENIE_MODEL_VENDOR_TIMER        1
#define CONFIG_MESH_MODEL_VENDOR_SRV    1
#define CONFIG_MESH_MODEL_GEN_ONOFF_SRV 1
#define CONFIG_MESH_MODEL_LIGHTNESS_SRV 1
// #define CONFIG_MESH_MODEL_GEN_LEVEL_SRV 1
// #define CONFIG_MESH_MODEL_CTL_SRV       1
#define CONFIG_MESH_MODEL_HSL_SRV       1
// #define CONFIG_MESH_MODEL_TRANS         1

#define FUNC_TRACE 1

#ifdef FUNC_TRACE
#define ENTER_FUNC() ESP_LOGD(TAG, "enter %s", __FUNCTION__)
#define EXIT_FUNC()  ESP_LOGD(TAG, "exit %s", __FUNCTION__)
#else
#define ENTER_FUNC()
#define EXIT_FUNC()
#endif

typedef enum {
    VALUE_TYPE_CUR = 0,
    VALUE_TYPE_TAR,
    VALUE_TYPE_NUM,
} value_type_t;

typedef struct {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    uint8_t onoff[VALUE_TYPE_NUM];
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    uint16_t actual[VALUE_TYPE_NUM];
// #ifndef CONFIG_MESH_SIMPLE_MODLE
    uint16_t linear[VALUE_TYPE_NUM];
// #endif
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    uint16_t temp[VALUE_TYPE_NUM];
    uint16_t UV[VALUE_TYPE_NUM];
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
    uint16_t hue[VALUE_TYPE_NUM];
    uint16_t saturation[VALUE_TYPE_NUM];
    uint16_t lightness[VALUE_TYPE_NUM];
#endif

#ifdef CONFIG_MESH_MODEL_TRANS
    uint8_t delay;    // unit:5ms
    uint8_t trans;    // unit:100ms

    uint32_t trans_start_time;
    uint32_t trans_last_time;
    uint32_t trans_end_time;
    int16_t actual_trans_step;
    int16_t temp_trans_step;

    util_timer_t delay_timer;
    util_timer_t trans_timer;
#endif
} model_state_t;

typedef struct {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    uint8_t default_onoff;
    uint8_t last_onoff;
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    uint16_t last_actual;
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    uint16_t last_temp;
#endif

#ifdef CONFIG_MESH_MODEL_HSL_SRV
    uint16_t last_hue;
    uint16_t last_saturation;
    uint16_t last_lightness;
#endif

#ifndef CONFIG_MESH_SIMPLE_MODLE
#ifdef CONFIG_MESH_MODEL_TRANS
    uint8_t def_trans;
#endif

#ifndef CONFIG_MESH_SIMPLE_MODLE
    uint8_t range_status;
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    uint16_t default_actual;
    uint16_t min_actual;
    uint16_t max_actual;
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    //temp
    uint16_t default_temp;
    uint16_t min_temp;
    uint16_t max_temp;

    uint16_t default_UV;
    uint16_t last_UV;
#endif
#endif
} model_powerup_t;

typedef struct {
    uint8_t elem_index;
    model_state_t state;        /**< Used to save the current state and target state of the device */
    model_powerup_t powerup;    /**< Used to save the device's power-down state and the previous state */
    void *user_data;
} elem_state_t;

typedef enum {
    GENIE_MESH_SUCCESS = 0,
    GENIE_MESH_TID_REPEAT_ERROR,
    GENIE_MESH_ANALYZE_SIZE_ERROR,
    GENIE_MESH_ANALYZE_ARGS_ERROR,
    GENIE_MESH_SET_TRANSTION_ERROR,
} genie_error_type_t;

#ifdef CONFIG_MESH_MODEL_VENDOR_SRV
#define INDICATION_FLAG_POWERON 0x80
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
#define INDICATION_FLAG_ONOFF 0x01
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
#define INDICATION_FLAG_LIGHTNESS 0x02
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
#define INDICATION_FLAG_CTL 0x04
#endif
#ifdef CONFIG_MESH_MODEL_HSL_SRV
#define INDICATION_FLAG_HSL 0x05
#endif
#endif

extern uint8_t g_indication_flag;

/**
 * @brief check whether there is this tid in record, and record it if not.
 * @param[in] src_addr indicates the device which hold this tid.
 * @param[in] tid
 * @return GENIE_MESH_SUCCESS means successed, otherwise failed.
 */
genie_error_type_t genie_msg_check_tid(uint16_t src_addr, uint8_t tid);

/**
 * @brief get the remain bytes of message
 * @param[in] p_state: model state
 * @param[in] is_ack: ack or not
 * @return the bytes
 */
uint8_t get_remain_byte(model_state_t *p_state, bool is_ack);

#ifdef CONFIG_MESH_MODEL_TRANS
/**
 * @brief get the transition time.
 * @param[in] byte means the byte of message.
 * @return the time in milliseconds.
 */
uint32_t get_transition_time(uint8_t byte);
#endif

/**
 * @brief send the mesh message
 *
 * @param[in] p_vendor_msg refers to the message to be sent
 *
 * @return 0 for success; negative for failure
 */
int16_t genie_mesh_msg_send(genie_model_msg_t *p_vendor_msg);

/**
 * @brief stop the delay_timer and trans_timer for element.
 *
 * @param[in] p_elem refers to the element.
 *
 */
void mesh_timer_stop(elem_state_t *p_elem);

/**
 * @brief handle the vendor message
 * @param[in] p_msg refers to the message to be handled
 *
 * @return 0 for successed, -1 for failed.
 */
uint16_t genie_vendor_model_msg_handle(genie_model_msg_t *p_msg);

/**
 * @brief start poweron indicate timer.
 *
 */
void poweron_indicate_start(void);

/**
 * @brief start PB-ADV timer.
 *
 */
void genie_pbadv_timer_start(void);

/**
 * @brief stop PB-ADV timer.
 *
 */
void genie_pbadv_timer_stop(void);

/**
 * @brief start provision timer.
 *
 */
void genie_prov_timer_start(void);

/**
 * @brief stop provision timer.
 *
 */
void genie_prov_timer_stop(void);

/**
 * @brief clear trans parameter.
 *
 * @param p_elem pointer of element state.
 */
void clear_trans_para(elem_state_t *p_elem);

/**
 * @brief calculater current state.
 *
 * @param p_elem pointer of element state.
 *
 * @return uint8_t
 */
uint8_t calc_cur_state(elem_state_t *p_elem);

/**
 * @brief Start silent adv.
 *
 */
void genie_pbadv_start_silent_adv(void);

/**
 * @brief Handle hardware reset.
 *
 * @return uint16_t
 */
uint16_t genie_indicate_hw_reset_event(void);

/**
 * @brief Initializing element state.
 *
 * @param state_count the number of element.
 * @param p_elem pointer of element state.
 *
 * @return uint8_t
 */
uint8_t elem_state_init(uint8_t state_count, elem_state_t *p_elem);

/**
 * @brief Genie vendor model indicate self status.
 *
 * @param p_elem pointer of element state.
 */
void genie_standart_indication(elem_state_t *p_elem);

/**
 * @brief Initializing Genie Mesh components.
 *
 */
void genie_mesh_init(void);

#ifdef __cplusplus
}
#endif /**< __cplusplus */

#endif /* _GENIE_MESH_H_ */
