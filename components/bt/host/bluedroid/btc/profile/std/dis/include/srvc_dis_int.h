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

#ifndef SRVC_DIS_INT_H
#define SRVC_DIS_INT_H

#include "common/bt_target.h"
#include "srvc_api.h"
#include "stack/gatt_api.h"

#define DIS_MAX_CHAR_NUM            9


typedef struct {
    UINT16      uuid;
    UINT16      handle;
} tDIS_DB_ENTRY;

#define DIS_SYSTEM_ID_SIZE      8
#define DIS_PNP_ID_SIZE         7



typedef struct {
    tDIS_DB_ENTRY           dis_attr[DIS_MAX_CHAR_NUM];
    tDIS_VALUE              dis_value;

    tDIS_READ_CBACK         *p_read_dis_cback;

    UINT16                  service_handle;
    UINT16                  max_handle;

    BOOLEAN                 enabled;

    UINT8                   dis_read_uuid_idx;

    tDIS_ATTR_MASK          request_mask;
} tDIS_CB;



#ifdef __cplusplus
extern "C" {
#endif

/* Global GATT data */
#if GATT_DYNAMIC_MEMORY == FALSE
extern tDIS_CB dis_cb;
#else
extern tDIS_CB *dis_cb_ptr;
#define dis_cb (*dis_cb_ptr)
#endif


extern BOOLEAN dis_valid_handle_range(UINT16 handle);
extern UINT8 dis_read_attr_value (UINT8 clcb_idx, UINT16 handle, tGATT_VALUE *p_value,
                                  BOOLEAN is_long, tGATT_STATUS *p_status);
extern UINT8 dis_write_attr_value(tGATT_WRITE_REQ *p_data, tGATT_STATUS *p_status);

extern void dis_c_cmpl_cback (tSRVC_CLCB *p_clcb, tGATTC_OPTYPE op,
                              tGATT_STATUS status, tGATT_CL_COMPLETE *p_data);


#ifdef __cplusplus
}
#endif
#endif
