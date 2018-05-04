/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef SRVC_BATTERY_INT_H
#define SRVC_BATTERY_INT_H

#include "common/bt_target.h"
#include "srvc_api.h"
#include "stack/gatt_api.h"

#ifndef BA_MAX_INT_NUM
#define BA_MAX_INT_NUM     4
#endif

#define BATTERY_LEVEL_SIZE      1


typedef struct {
    UINT8           app_id;
    UINT16          ba_level_hdl;
    UINT16          clt_cfg_hdl;
    UINT16          rpt_ref_hdl;
    UINT16          pres_fmt_hdl;

    tBA_CBACK       *p_cback;

    UINT16          pending_handle;
    UINT8           pending_clcb_idx;
    UINT8           pending_evt;

} tBA_INST;

typedef struct {
    tBA_INST                battery_inst[BA_MAX_INT_NUM];
    UINT8                   inst_id;
    BOOLEAN                 enabled;

} tBATTERY_CB;

#ifdef __cplusplus
extern "C" {
#endif

/* Global GATT data */
#if GATT_DYNAMIC_MEMORY == FALSE
extern tBATTERY_CB battery_cb;
#else
extern tBATTERY_CB *battery_cb_ptr;
#define battery_cb (*battery_cb_ptr)
#endif


extern BOOLEAN battery_valid_handle_range(UINT16 handle);

extern UINT8 battery_s_write_attr_value(UINT8 clcb_idx, tGATT_WRITE_REQ *p_value,
                                        tGATT_STATUS *p_status);
extern UINT8 battery_s_read_attr_value (UINT8 clcb_idx, UINT16 handle, tGATT_VALUE *p_value, BOOLEAN is_long, tGATT_STATUS *p_status);



#ifdef __cplusplus
}
#endif
#endif
