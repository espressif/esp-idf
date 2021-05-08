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

#ifndef _GENIE_EVENT_H_
#define _GENIE_EVENT_H_

#ifdef __cplusplus
extern "C" {
#endif /**< __cplusplus */

typedef enum {
    /* START--- Don't add new ID before this one */
    GENIE_EVT_START = 0,

    /* Reset Related Operation */
    GENIE_EVT_SW_RESET = GENIE_EVT_START,   /* triggered from cloud */
    GENIE_EVT_HW_RESET_START,               /* triggered from user */
    GENIE_EVT_HW_RESET_DONE,                /*triggered by reset by repeat module */

    /* SDK triggered event, with prefix of GENIE_EVT_SDK_MESH_ */
    GENIE_EVT_SDK_START,
    GENIE_EVT_SDK_MESH_INIT = GENIE_EVT_SDK_START,
    GENIE_EVT_SDK_MESH_PBADV_START,
    GENIE_EVT_SDK_MESH_PBADV_TIMEOUT,
    GENIE_EVT_SDK_MESH_SILENT_START,

    GENIE_EVT_SDK_MESH_PROV_START,
    GENIE_EVT_SDK_MESH_PROV_TIMEOUT,
    GENIE_EVT_SDK_MESH_PROV_SUCCESS,
    GENIE_EVT_SDK_MESH_PROV_FAIL,

    GENIE_EVT_SDK_APPKEY_ADD,
    GENIE_EVT_SDK_APPKEY_DEL,
    GENIE_EVT_SDK_APPKEY_UPDATE,
    GENIE_EVT_SDK_NETKEY_ADD,
    GENIE_EVT_SDK_NETKEY_DEL,
    GENIE_EVT_SDK_NETKEY_UPDATE,
    GENIE_EVT_SDK_SUB_ADD,
    GENIE_EVT_SDK_SUB_DEL,
    GENIE_EVT_SDK_HB_SET,
    GENIE_EVT_SDK_SEQ_UPDATE,
    GENIE_EVT_SDK_STATE_SYNC,
    GENIE_EVT_SDK_IVI_UPDATE,
    GENIE_EVENT_SUB_SET,
    GENIE_EVENT_HB_SET,

    GENIE_EVT_SDK_ANALYZE_MSG,

    GENIE_EVT_SDK_AIS_DISCON,
    GENIE_EVT_SDK_DELAY_START,
    GENIE_EVT_SDK_DELAY_END,
    GENIE_EVT_SDK_TRANS_START,
    GENIE_EVT_SDK_TRANS_CYCLE,
    GENIE_EVT_SDK_TRANS_END,
    GENIE_EVT_SDK_ACTION_DONE,

    GENIE_EVT_SDK_MESH_PWRON_INDC,
    GENIE_EVT_SDK_INDICATE,
    GENIE_EVT_SDK_VENDOR_MSG,

    /* APP triggered event, with prefix of GENIE_EVT_APP_ */
    GENIE_EVT_APP_START,
    GENIE_EVT_APP_FAC_QUIT = GENIE_EVT_APP_START,
    GENIE_EVT_TIME_OUT,

    /* Board */
    GENIE_EVT_BUTTON_TAP,
    GENIE_EVT_SDK_COLOR_ACTION,
    GENIE_EVT_RESET_BY_REPEAT_NOTIFY, /* triggered by reset */

    /* END --- Don't add new ID after this one */
    GENIE_EVT_END

} genie_event_t;

/**
 * @brief The handler for the underlying events. If necessary
 *      this handler dispatch the user events to applications.
 *
 * @param event refers to the event details.
 * @param args refers to the additional information for the event.
 */
void genie_event(genie_event_t event, void *args);

#ifdef __cplusplus
}
#endif /**< __cplusplus */

#endif /* _GENIE_EVENT_H_ */
