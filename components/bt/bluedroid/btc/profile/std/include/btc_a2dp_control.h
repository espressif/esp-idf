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

/*******************************************************************************
 *
 *  Filename:      btc_a2dp_control.h
 *
 *******************************************************************************/

#ifndef __BTC_A2DP_CONTROL_H__
#define __BTC_A2DP_CONTROL_H__

#include <stdbool.h>
#include "bt_target.h"
#include "bta_api.h"
#include "btc_av_api.h"
#include "esp_a2dp_api.h"

#if BTC_AV_INCLUDED
/*******************************************************************************
 **  Public functions
 *******************************************************************************/

/*******************************************************************************
 **
 ** Function         btc_a2dp_control_media_ctrl
 **
 ** Description      Handle the media_ctrl command
 **
 *******************************************************************************/
void btc_a2dp_control_media_ctrl(esp_a2d_media_ctrl_t ctrl);


/*******************************************************************************
 **
 ** Function         btc_a2dp_control_datapath_ctrl
 **
 ** Description      Handle the media datapath event, which is adapted from UIPC
 **                  data channel from bluedroid
 **
 *******************************************************************************/
void btc_a2dp_control_datapath_ctrl(uint32_t dp_evt);


/*******************************************************************************
 **
 ** Function         btc_a2dp_control_command_ack
 **
 ** Description      Acknowledge the pending media_ctrl command
 **
 *******************************************************************************/
void btc_a2dp_control_command_ack(int status);


/*******************************************************************************
 **
 ** Function         btc_a2dp_control_get_datachnl_stat
 **
 ** Description      Check whether the data channel state is open
 **
 ** Return           TRUE if the data channel state is open
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_control_get_datachnl_stat(void);


/*******************************************************************************
 **
 ** Function         btc_a2dp_control_set_datachnl_stat
 **
 ** Description      Set the data channel state flag
 **
 *******************************************************************************/
void btc_a2dp_control_set_datachnl_stat(BOOLEAN open);


/*******************************************************************************
 **
 ** Function         btc_a2dp_control_init
 **
 ** Description      Initialize the A2DP control module. It should be called during
 **                  the startup stage of A2DP streaming.
 **
 *******************************************************************************/
bool btc_a2dp_control_init(void);


/*******************************************************************************
 **
 ** Function         btc_a2dp_control_cleanup
 **
 ** Description      Cleanup the A2DP control module
 **
 *******************************************************************************/
void btc_a2dp_control_cleanup(void);

#endif /* #if BTC_AV_INCLUDED */

#endif /* __BTC_A2DP_CONTROL_H__ */
