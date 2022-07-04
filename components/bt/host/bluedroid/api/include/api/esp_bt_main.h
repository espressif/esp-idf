// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_BT_MAIN_H__
#define __ESP_BT_MAIN_H__

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bluetooth stack status type, to indicate whether the bluetooth stack is ready
 */
typedef enum {
    ESP_BLUEDROID_STATUS_UNINITIALIZED   = 0,        /*!< Bluetooth not initialized */
    ESP_BLUEDROID_STATUS_INITIALIZED,                /*!< Bluetooth initialized but not enabled */
    ESP_BLUEDROID_STATUS_ENABLED                     /*!< Bluetooth initialized and enabled */
} esp_bluedroid_status_t;

typedef enum {
    ADV_PKT   = 0,
    OTHER_PKT = 1
} hci_recv_pkt_type_t;

typedef void (* hci_recv_fail_cb_t)(hci_recv_pkt_type_t type, uint16_t pkt_len, uint32_t free_heap_size);

extern esp_err_t esp_bt_register_hci_recv_fail_callback(hci_recv_fail_cb_t callback);

extern uint32_t get_lld_evt_sch_num(void);

extern uint32_t get_llm_adv_ind_num(void);

/**
 * @brief     Get bluetooth stack status
 *
 * @return    Bluetooth stack status
 *
 */
esp_bluedroid_status_t esp_bluedroid_get_status(void);
    
/**
 * @brief     Enable bluetooth, must after esp_bluedroid_init()
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_bluedroid_enable(void);

/**
 * @brief     Disable bluetooth, must prior to esp_bluedroid_deinit()
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_bluedroid_disable(void);

/**
 * @brief     Init and alloc the resource for bluetooth, must be prior to every bluetooth stuff
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_bluedroid_init(void);

/**
 * @brief     Deinit and free the resource for bluetooth, must be after every bluetooth stuff
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_bluedroid_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_MAIN_H__ */
