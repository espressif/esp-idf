/******************************************************************************
 *
 *  Copyright (C) 1999-2013 Broadcom Corporation
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

/*******************************************************************************
**
** Header file for profile folder.
**
********************************************************************************/

#ifndef SRVC_DIS_API_H
#define SRVC_DIS_API_H

#include "bt_target.h"
#include "gatt_api.h"
#include "gattdefs.h"
#include "esp_gatts_api.h"

#define DIS_SUCCESS             GATT_SUCCESS
#define DIS_ILLEGAL_PARAM       GATT_ILLEGAL_PARAMETER
#define DIS_NO_RESOURCES        GATT_NO_RESOURCES
typedef UINT8 tDIS_STATUS;


/*****************************************************************************
**  Data structure for DIS
*****************************************************************************/

#define DIS_ATTR_SYS_ID_BIT         0x0001
#define DIS_ATTR_MODEL_NUM_BIT      0x0002
#define DIS_ATTR_SERIAL_NUM_BIT     0x0004
#define DIS_ATTR_FW_NUM_BIT         0x0008
#define DIS_ATTR_HW_NUM_BIT         0x0010
#define DIS_ATTR_SW_NUM_BIT         0x0020
#define DIS_ATTR_MANU_NAME_BIT      0x0040
#define DIS_ATTR_IEEE_DATA_BIT      0x0080
#define DIS_ATTR_PNP_ID_BIT         0x0100
typedef UINT16  tDIS_ATTR_MASK;

#define DIS_ATTR_ALL_MASK           0xffff

typedef tDIS_ATTR_MASK tDIS_ATTR_BIT ;

#define DIS_MAX_NUM_INC_SVR         0
#define DIS_MAX_CHAR_NUM            9
#define DIS_MAX_ATTR_NUM            (DIS_MAX_CHAR_NUM * 2 + DIS_MAX_NUM_INC_SVR + 1)

#ifndef DIS_ATTR_DB_SIZE
#define DIS_ATTR_DB_SIZE            GATT_DB_MEM_SIZE(DIS_MAX_NUM_INC_SVR, DIS_MAX_CHAR_NUM, 0)
#endif

#define DIS_SYSTEM_ID_SIZE          8
#define DIS_PNP_ID_SIZE             7


typedef struct {
    UINT16      uuid;
    UINT16      handle;
} tDIS_DB_ENTRY;

typedef struct {
    UINT16      len;
    UINT8       *p_data;
} tDIS_STRING;

typedef struct {
    UINT16       vendor_id;
    UINT16       product_id;
    UINT16       product_version;
    UINT8        vendor_id_src;

} tDIS_PNP_ID;

typedef union {
    UINT64              system_id;
    tDIS_PNP_ID         pnp_id;
    tDIS_STRING         data_str;
} tDIS_ATTR;

#define DIS_MAX_STRING_DATA     7

typedef struct {
    UINT16                  attr_mask;
    UINT64                  system_id;
    tDIS_PNP_ID             pnp_id;
    UINT8                   *data_string[DIS_MAX_STRING_DATA];
} tDIS_VALUE;

//typedef void (tDIS_READ_CBACK)(BD_ADDR addr, tDIS_VALUE *p_dis_value);

typedef struct {
    tDIS_DB_ENTRY           dis_attr[DIS_MAX_CHAR_NUM];
    tDIS_VALUE              dis_value;

//    tDIS_READ_CBACK         *p_read_dis_cback;

    UINT16                  service_handle;
    UINT16                  max_handle;

    bool                    enabled;

    //  UINT8                   dis_read_uuid_idx;
    //  tDIS_ATTR_MASK          request_mask;
} tDIS_CB;

/*****************************************************************************
**  Data structure used by Battery Service
*****************************************************************************/

#ifndef BA_MAX_INT_NUM
#define BA_MAX_INT_NUM  4
#endif

#define BATTERY_LEVEL_SIZE   1

typedef struct {
    BD_ADDR remote_bda;
    BOOLEAN need_rsp;
    UINT16  clt_cfg;
} tBA_WRITE_DATA;

#define BA_READ_CLT_CFG_REQ     1
#define BA_READ_PRE_FMT_REQ     2
#define BA_READ_RPT_REF_REQ     3
#define BA_READ_LEVEL_REQ       4
#define BA_WRITE_CLT_CFG_REQ    5

typedef void (tBA_CBACK)(UINT32 trans_id, UINT16 conn_id, UINT8 app_id, UINT8 event, tBA_WRITE_DATA *p_data);

#define BA_LEVEL_NOTIFY         0x01
#define BA_LEVEL_PRE_FMT        0x02
#define BA_LEVEL_RPT_REF        0x04
typedef UINT8   tBA_LEVEL_DESCR;

typedef struct {
    BOOLEAN         is_pri;
    tBA_LEVEL_DESCR     ba_level_descr;
    tGATT_TRANSPORT transport;
    tBA_CBACK       *p_cback;

} tBA_REG_INFO;

typedef union {
    UINT8       ba_level;
    UINT16      clt_cfg;
    tGATT_CHAR_RPT_REF  rpt_ref;
    tGATT_CHAR_PRES     pres_fmt;
} tBA_RSP_DATA;

typedef struct {
    UINT8       app_id;
    UINT16      ba_level_hdl;
    UINT16      clt_cfg_hdl;
    UINT16      rpt_ref_hdl;
    UINT16      pres_fmt_hdl;

    tBA_CBACK   *p_cback;

    UINT16      pending_handle;
    //UINT8       pending_clcb_idx;
    UINT8       pending_evt;
} tBA_INST;

typedef struct {
    tBA_INST        battery_inst[BA_MAX_INT_NUM];
    UINT8           inst_id;
    bool            enabled;
} tBATTERY_CB;
/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/*****************************************************************************
**  Service Engine API
*****************************************************************************/
/*******************************************************************************
**
** Function         srvc_eng_init
**
** Description      Initializa the GATT Service engine, register a GATT application
**                  as for a central service management.
**
*******************************************************************************/
//extern tGATT_STATUS srvc_eng_init (void);


/*****************************************************************************
**  DIS Server Function
*****************************************************************************/

extern bool dis_valid_handle_range(UINT16 handle);
/*******************************************************************************
**
** Function         DIS_Init
**
** Description      Initializa the Device Information Service Server.
**
*******************************************************************************/
extern void DIS_Init (tBTA_GATTS_IF gatt_if, tDIS_ATTR_MASK dis_attr_mask);
/*******************************************************************************
**
** Function         DIS_SrUpdate
**
** Description      Update the DIS server attribute values
**
*******************************************************************************/
extern tDIS_STATUS DIS_SrUpdate(tDIS_ATTR_BIT dis_attr_bit, tDIS_ATTR *p_info);
/*******************************************************************************
**
** Function         dis_AddChar
**
** Description      add characteristic for dis
**
*******************************************************************************/
extern void dis_AddChar(UINT16 service_id);
/*******************************************************************************
**   dis_s_read_attr_value
**
**   Process read DIS attribute request.
*******************************************************************************/

extern void dis_s_read_attr_value (tGATTS_DATA *p_data, tGATT_VALUE *p_value,
                                   UINT32 trans_id, UINT16 conn_id);
/*****************************************************************************
**  DIS Client Function
*****************************************************************************/
/*******************************************************************************
**
** Function         DIS_ReadDISInfo
**
** Description      Read remote device DIS information.
**
** Returns          void
**
*******************************************************************************/
//extern BOOLEAN DIS_ReadDISInfo(BD_ADDR peer_bda, tDIS_READ_CBACK *p_cback,
//                              tDIS_ATTR_MASK mask);

/*******************************************************************************
**      BATTERY SERVICE API
*******************************************************************************/
/***************************************************************
**
** Function     bas_register
**
** Description  register app for battery service
**
****************************************************************/
extern void bas_register(void);
/***************************************************************
**
** Function     bas_init
**
** Description  register battery service
**
****************************************************************/
extern void bas_init(tBTA_GATTS_IF gatt_if, UINT16 app_id);

/***************************************************************
**
** Function     bas_AddChar
**
** Description  add characteristic for battery service
**
****************************************************************/
extern void bas_AddChar(UINT16 service_id, tBT_UUID *char_uuid);
/***************************************************************
**
** Function     bas_AddCharDescr
**
** Description  add descriptor for battery service if needed
**
****************************************************************/
extern void bas_AddCharDescr(UINT16 service_id, UINT16 attr_id);
/***************************************************************
**
** Function     bas_service_cmpl
**
** Description  create battery service complete
**
****************************************************************/
extern void bas_service_cmpl(UINT16 service_id, tBTA_GATT_STATUS status);
/*******************************************************************************
**
** Function         Battery_Rsp
**
** Description      Respond to a battery service request
**
*******************************************************************************/
extern void Battery_Rsp (UINT32 trans_id, UINT16 conn_id, UINT8 app_id,
                         tGATT_STATUS st, UINT8 event, tBA_RSP_DATA *p_rsp);
/*******************************************************************************
**
** Function         Battery_Notify
**
** Description      Send battery level notification
**
*******************************************************************************/
extern void Battery_Notify (UINT16 conn_id, UINT8 app_id, BD_ADDR remote_bda, UINT8 battery_level);

/*****************************************************************************
** Function      bas_s_read_attr_value
**
** Description   it will be called when client send a read request
******************************************************************************/
extern void bas_s_read_attr_value(tGATTS_DATA *p_data, UINT32 trans_id, UINT16 conn_id);
/*****************************************************************************
** Function      bas_s_write_attr_value
**
** Description   it will be called when client send a write request
******************************************************************************/
extern void bas_s_write_attr_value(tGATTS_DATA *p_data, UINT32 trans_id,
                                   UINT16 conn_id, BD_ADDR bd_addr);

extern void gatts_server_test(void);
#ifdef __cplusplus

}
#endif

#endif
