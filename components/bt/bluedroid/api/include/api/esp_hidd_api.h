// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
// Copyright 2019      Blake Felt
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ESP_HIDD_API_H__
#define __ESP_HIDD_API_H__

#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HIDD report types
 */
typedef enum {
  ESP_HIDD_REPORT_TYPE_OTHER = 0,
  ESP_HIDD_REPORT_TYPE_INPUT,
  ESP_HIDD_REPORT_TYPE_OUTPUT,
  ESP_HIDD_REPORT_TYPE_FEATURE,
  // special value for reports to be sent on INTR(INPUT is assumed)
  ESP_HIDD_REPORT_TYPE_INTRDATA
} esp_hidd_report_type_t;

/**
 * @brief whether HIDD app state is registered or not.
 */
typedef enum {
  ESP_HIDD_APP_STATE_NOT_REGISTERED,
  ESP_HIDD_APP_STATE_REGISTERED
} esp_hidd_application_state_t;

/**
 * @brief HIDD connection state
 */
typedef enum {
  ESP_HIDD_CONN_STATE_CONNECTED,
  ESP_HIDD_CONN_STATE_CONNECTING,
  ESP_HIDD_CONN_STATE_DISCONNECTED,
  ESP_HIDD_CONN_STATE_DISCONNECTING,
  ESP_HIDD_CONN_STATE_UNKNOWN
} esp_hidd_connection_state_t;

/**
 * @brief HIDD characteristics for SDP report
 */
typedef struct {
  const char* name;
  const char* description;
  const char* provider;
  uint8_t subclass;
  uint8_t* desc_list;
  int desc_list_len;
} esp_hidd_app_param_t;

/**
 * @brief HIDD Quality of Service parameters
 */
typedef struct {
  uint8_t service_type;
  uint32_t token_rate;
  uint32_t token_bucket_size;
  uint32_t peak_bandwidth;
  uint32_t access_latency;
  uint32_t delay_variation;
} esp_hidd_qos_param_t;

/**
 * @brief   HIDD callback for change in application state
 * @param   bd_addr:  bluetooth address
 * @param   state:    application state
 */
typedef void (*esp_hidd_application_state_callback)(esp_bd_addr_t bd_addr,
                                                esp_hidd_application_state_t state);

/**
 * @brief   HIDD callback for change in connection state
 * @param   bd_addr:  host's bluetooth address
 * @param   state:    connection state
 */
typedef void (*esp_hidd_connection_state_callback)(esp_bd_addr_t bd_addr,
                                               esp_hidd_connection_state_t state);

/**
 * @brief   HIDD callback for a get_report from host
 * @param   type:         requested report type
 * @param   id:           requested report id
 * @param   buffer_size:  requested buffer size
 */
typedef void (*esp_hidd_get_report_callback)(uint8_t type, uint8_t id,
                                         uint16_t buffer_size);
/**
 * @brief   HIDD callback for receiving a set_report from the host
 * @param   type:     type of report
 * @param   id:       id of report
 * @param   len:      length of report
 * @param   p_data:   the report
 */
typedef void (*esp_hidd_set_report_callback)(uint8_t type, uint8_t id, uint16_t len,
                                         uint8_t* p_data);
/**
 * @brief   HIDD callback for receiving a set_protocol from the host
 * @param   protocol:   the requested protocol from the host
 */
typedef void (*esp_hidd_set_protocol_callback)(uint8_t protocol);

/**
 * @brief   HIDD callback for receiving a report over interrupt from the host
 * @param   report_id:    id of report
 * @param   len:          length of report
 * @param   p_data:       the report
 */
typedef void (*esp_hidd_intr_data_callback)(uint8_t report_id, uint16_t len,
                                        uint8_t* p_data);
/**
 * @brief   HIDD callback for receiving a virtual unplug from the host
 */
typedef void (*esp_hidd_vc_unplug_callback)(void);

/**
 * @brief HIDD callbacks
 */
typedef struct {
  size_t size;
  esp_hidd_application_state_callback application_state_cb;
  esp_hidd_connection_state_callback connection_state_cb;
  esp_hidd_get_report_callback get_report_cb;
  esp_hidd_set_report_callback set_report_cb;
  esp_hidd_set_protocol_callback set_protocol_cb;
  esp_hidd_intr_data_callback intr_data_cb;
  esp_hidd_vc_unplug_callback vc_unplug_cb;
} esp_hidd_callbacks_t;

// used by backend
typedef union {
    esp_hidd_callbacks_t* callbacks;

    struct {
        esp_hidd_app_param_t* app_param;
        esp_hidd_qos_param_t* in_qos;
        esp_hidd_qos_param_t* out_qos;
    } register_app;

    struct {
        esp_hidd_report_type_t type;
        uint8_t id;
        uint16_t len;
        uint8_t* p_data;
    } send_report;

    uint8_t error;
} esp_hidd_args_t;

/**
 * @brief       This function initializes HIDD. Must be done after esp_hid_device_register_app.
 * 
 * @param[in]   callbacks:    pointer to the callback function struct
 * 
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_hid_device_init(esp_hidd_callbacks_t* callbacks);

/**
 * @brief     Removes HIDD interface.
 * 
 * @return    - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_hid_device_cleanup(void);

/**
 * @brief     Registers HIDD parameters with SDP and sets l2cap Quality of Service.
 * 
 * @param[in] app_param:  HIDD parameters
 * @param[in] in_qos:     incoming QoS parameters
 * @param[in] out_qos:    outgoing QoS parameters
 * 
 * @return    - ESP_OK: success
 *            - other: failed
 */ 
esp_err_t esp_hid_device_register_app(esp_hidd_app_param_t* app_param,
                                      esp_hidd_qos_param_t* in_qos,
                                      esp_hidd_qos_param_t* out_qos);

/**
 * @brief   Removes HIDD parameters from SDP and resets l2cap Quality of Service.
 * 
 * @return  - ESP_OK: success
 *          - other: failed
 */
esp_err_t esp_hid_device_unregister_app(void);

/**
 * @brief     This function connects HIDD interface to connected bluetooth device, if not done already.
 * 
 * @return
 *            - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_hid_device_connect(void);

/**
 * @brief     This function disconnects HIDD interface.
 * 
 * @return
 *            - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_hid_device_disconnect(void);

/**
 * @brief     Send HIDD report.
 * 
 * @param[in] type:   type of report
 * @param[in] id:     report id as defined by descriptor
 * @param[in] len:    length of report
 * @param[in] p_data: report
 * 
 * @return
 *            - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_hid_device_send_report(esp_hidd_report_type_t type, uint8_t id, uint16_t len, uint8_t* p_data);

/**
 * @brief     Sends HIDD handshake with error info for invalid set_report
 * 
 * @param[in] error: type of error
 * 
 * @return    - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_hid_device_report_error(uint8_t error);

/**
 * @brief     Unplug virtual cable of HIDD.
 * 
 * @return    - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_hid_device_virtual_cable_unplug(void);

#ifdef __cplusplus
}
#endif

#endif