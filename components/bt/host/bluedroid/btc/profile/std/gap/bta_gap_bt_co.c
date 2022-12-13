// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include "btc_gap_bt.h"
#include "btc/btc_util.h"

#if (BTC_GAP_BT_INCLUDED == TRUE)
void btc_gap_bt_config_eir_cmpl_callback (uint8_t status, uint8_t eir_type_num, uint8_t *eir_type)
{
    esp_bt_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_CONFIG_EIR_DATA_EVT;

    param.config_eir_data.stat =  btc_bta_status_to_esp_status(status);
    param.config_eir_data.eir_type_num = eir_type_num;
    memcpy(param.config_eir_data.eir_type, eir_type, eir_type_num);

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_bt_gap_cb_param_t), NULL, NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}
#endif /// (BTC_GAP_BT_INCLUDED == TRUE)