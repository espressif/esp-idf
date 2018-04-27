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
 *  Filename:      btc_a2dp.h
 *
 *  Description:   Common definitions for A2DP
 *
 *******************************************************************************/

#ifndef __BTC_A2DP_H__
#define __BTC_A2DP_H__

#include <stdbool.h>
#include "common/bt_target.h"
#include "bta/bta_api.h"
#include "btc_av_api.h"
#include "esp_a2dp_api.h"

#if BTC_AV_INCLUDED

/*******************************************************************************
 **  Constants
 *******************************************************************************/
#define BTC_AV_SUCCESS                             (0)
/**
 * AV (Audio Video source) Errors
 */
#define BTC_ERROR_SRV_AV_NOT_ENABLED               700     /* AV is not enabled */
#define BTC_ERROR_SRV_AV_FEEDING_NOT_SUPPORTED     701     /* Requested Feeding not supported */
#define BTC_ERROR_SRV_AV_BUSY                      702     /* Another operation ongoing */
#define BTC_ERROR_SRV_AV_NOT_OPENED                703     /* No AV link opened */
#define BTC_ERROR_SRV_AV_NOT_STARTED               704     /* AV is not started */
#define BTC_ERROR_SRV_AV_CP_NOT_SUPPORTED          705     /* Content protection is not supported by all headsets */

/* Transcoding definition for TxTranscoding and RxTranscoding */
#define BTC_MEDIA_TRSCD_OFF                        0
#define BTC_MEDIA_TRSCD_PCM_2_SBC                  1       /* Tx */


/*******************************************************************************
 **  Data types
 *******************************************************************************/
typedef int tBTC_AV_STATUS;

/*******************************************************************************
 **  Public functions
 *******************************************************************************/

void btc_a2dp_on_init(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_on_idle
 **
 ** Description      Process 'idle' request from BTC AV state machine during
 **                  initialization
 **
 *******************************************************************************/
void btc_a2dp_on_idle(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_on_started
 **
 ** Description      Process 'start' request from BTC AV state machine to prepare
 **                  for A2DP streaming
 **
 ** Return           TRUE if an ACK for the local command is sent
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_on_started(tBTA_AV_START *p_av, BOOLEAN pending_start);

/*******************************************************************************
 **
 ** Function         btc_a2dp_on_stopped
 **
 ** Description      Process 'stop' request from BTC AV state machine to stop
 **                  A2DP streaming
 **
 *******************************************************************************/
void btc_a2dp_on_stopped(tBTA_AV_SUSPEND *p_av);

/*******************************************************************************
 **
 ** Function         btc_a2dp_on_suspended
 **
 ** Description      Process 'stop' request from BTC AV state machine to suspend
 **                  A2DP streaming
 **
 *******************************************************************************/
void btc_a2dp_on_suspended(tBTA_AV_SUSPEND *p_av);

#endif /* #if BTC_AV_INCLUDED */

#endif /* __BTC_A2DP_H__ */
