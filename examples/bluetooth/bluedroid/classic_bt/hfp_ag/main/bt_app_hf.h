/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __BT_APP_HF_H__
#define __BT_APP_HF_H__

#include <stdint.h>
#include "esp_hf_ag_api.h"
#include "esp_bt_defs.h"

esp_bd_addr_t hf_peer_addr; // Declaration of peer device bdaddr

#define BT_HF_TAG               "BT_APP_HF"

/**
 * @brief     callback function for HF client
 */
void bt_app_hf_cb(esp_hf_cb_event_t event, esp_hf_cb_param_t *param);


#endif /* __BT_APP_HF_H__*/
 