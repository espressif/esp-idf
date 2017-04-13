/******************************************************************************
 *
 *  Copyright (C) 2006-2012 Broadcom Corporation
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

/******************************************************************************
 *
 *  This is the interface file for device mananger callout functions.
 *
 ******************************************************************************/
#ifndef BTA_DM_CO_H
#define BTA_DM_CO_H

#include "bta_sys.h"


#ifndef BTA_SCO_OUT_PKT_SIZE
#define BTA_SCO_OUT_PKT_SIZE    BTM_SCO_DATA_SIZE_MAX
#endif

#define BTA_SCO_CODEC_PCM       0       /* used for regular SCO */
#define BTA_SCO_CODEC_SBC       1       /* used for WBS */
typedef UINT8   tBTA_SCO_CODEC_TYPE;

#define BTA_DM_SCO_SAMP_RATE_8K     8000
#define BTA_DM_SCO_SAMP_RATE_16K    16000

/* SCO codec information */
typedef struct {
    tBTA_SCO_CODEC_TYPE   codec_type;
} tBTA_CODEC_INFO;

#define BTA_DM_SCO_ROUTE_PCM    BTM_SCO_ROUTE_PCM
#define BTA_DM_SCO_ROUTE_HCI    BTM_SCO_ROUTE_HCI

typedef tBTM_SCO_ROUTE_TYPE tBTA_DM_SCO_ROUTE_TYPE;


/*****************************************************************************
**  Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         bta_dm_co_io_req
**
** Description      This callout function is executed by DM to get IO capabilities
**                  of the local device for the Simple Pairing process
**
** Parameters       bd_addr  - The peer device
**                  *p_io_cap - The local Input/Output capabilities
**                  *p_oob_data - TRUE, if OOB data is available for the peer device.
**                  *p_auth_req - TRUE, if MITM protection is required.
**
** Returns          void.
**
*******************************************************************************/
extern void bta_dm_co_io_req(BD_ADDR bd_addr, tBTA_IO_CAP *p_io_cap,
                             tBTA_OOB_DATA *p_oob_data, tBTA_AUTH_REQ *p_auth_req,
                             BOOLEAN is_orig);

/*******************************************************************************
**
** Function         bta_dm_co_io_rsp
**
** Description      This callout function is executed by DM to report IO capabilities
**                  of the peer device for the Simple Pairing process
**
** Parameters       bd_addr  - The peer device
**                  io_cap - The remote Input/Output capabilities
**                  oob_data - TRUE, if OOB data is available for the peer device.
**                  auth_req - TRUE, if MITM protection is required.
**
** Returns          void.
**
*******************************************************************************/
extern void bta_dm_co_io_rsp(BD_ADDR bd_addr, tBTA_IO_CAP io_cap,
                             tBTA_OOB_DATA oob_data, tBTA_AUTH_REQ auth_req);

/*******************************************************************************
**
** Function         bta_dm_co_lk_upgrade
**
** Description      This callout function is executed by DM to check if the
**                  platform wants allow link key upgrade
**
** Parameters       bd_addr  - The peer device
**                  *p_upgrade - TRUE, if link key upgrade is desired.
**
** Returns          void.
**
*******************************************************************************/
extern void  bta_dm_co_lk_upgrade(BD_ADDR bd_addr, BOOLEAN *p_upgrade );

/*******************************************************************************
**
** Function         bta_dm_co_loc_oob
**
** Description      This callout function is executed by DM to report the OOB
**                  data of the local device for the Simple Pairing process
**
** Parameters       valid - TRUE, if the local OOB data is retrieved from LM
**                  c     - Simple Pairing Hash C
**                  r     - Simple Pairing Randomnizer R
**
** Returns          void.
**
*******************************************************************************/
extern void bta_dm_co_loc_oob(BOOLEAN valid, BT_OCTET16 c, BT_OCTET16 r);

/*******************************************************************************
**
** Function         bta_dm_co_rmt_oob
**
** Description      This callout function is executed by DM to request the OOB
**                  data for the remote device for the Simple Pairing process
**
** Parameters       bd_addr  - The peer device
**
** Returns          void.
**
*******************************************************************************/
extern void bta_dm_co_rmt_oob(BD_ADDR bd_addr);

/*****************************************************************************
**  SCO over HCI Function Declarations
*****************************************************************************/
/*******************************************************************************
**
** Function         bta_dm_sco_co_init
**
** Description      This function can be used by the phone to initialize audio
**                  codec or for other initialization purposes before SCO connection
**                  is opened.
**
**
** Returns          Void.
**
*******************************************************************************/
extern tBTA_DM_SCO_ROUTE_TYPE bta_dm_sco_co_init(UINT32 rx_bw, UINT32 tx_bw,
        tBTA_CODEC_INFO *p_codec_info, UINT8 app_id);


/*******************************************************************************
**
** Function         bta_dm_sco_co_open
**
** Description      This function is executed when a SCO connection is open.
**
**
** Returns          void
**
*******************************************************************************/
extern void bta_dm_sco_co_open(UINT16 handle, UINT8 pkt_size, UINT16 event);

/*******************************************************************************
**
** Function         bta_dm_sco_co_close
**
** Description      This function is called when a SCO connection is closed
**
**
** Returns          void
**
*******************************************************************************/
extern void bta_dm_sco_co_close(void);

/*******************************************************************************
**
** Function         bta_dm_sco_co_out_data
**
** Description      This function is called to send SCO data over HCI.
**
** Returns          void
**
*******************************************************************************/
extern void bta_dm_sco_co_out_data(BT_HDR  **p_buf);

/*******************************************************************************
**
** Function         bta_dm_sco_co_in_data
**
** Description      This function is called to send incoming SCO data to application.
**
** Returns          void
**
*******************************************************************************/
extern void bta_dm_sco_co_in_data(BT_HDR  *p_buf, tBTM_SCO_DATA_FLAG status);



/*******************************************************************************
**
** Function         bta_dm_co_ble_io_req
**
** Description      This callout function is executed by DM to get BLE IO capabilities
**                  before SMP pairing gets going.
**
** Parameters       bd_addr  - The peer device
**                  *p_io_cap - The local Input/Output capabilities
**                  *p_oob_data - TRUE, if OOB data is available for the peer device.
**                  *p_auth_req -  Auth request setting (Bonding and MITM required or not)
**                  *p_max_key_size - max key size local device supported.
**                  *p_init_key - initiator keys.
**                  *p_resp_key - responder keys.
**
** Returns          void.
**
*******************************************************************************/
extern void bta_dm_co_ble_io_req(BD_ADDR bd_addr,  tBTA_IO_CAP *p_io_cap,
                                 tBTA_OOB_DATA *p_oob_data,
                                 tBTA_LE_AUTH_REQ *p_auth_req,
                                 UINT8 *p_max_key_size,
                                 tBTA_LE_KEY_TYPE *p_init_key,
                                 tBTA_LE_KEY_TYPE  *p_resp_key );


/*******************************************************************************
**
** Function         bta_dm_co_ble_local_key_reload
**
** Description      This callout function is to load the local BLE keys if available
**                  on the device.
**
** Parameters       none
**
** Returns          void.
**
*******************************************************************************/
extern void bta_dm_co_ble_load_local_keys (tBTA_DM_BLE_LOCAL_KEY_MASK *p_key_mask, BT_OCTET16 er,
        tBTA_BLE_LOCAL_ID_KEYS *p_id_keys);

// btla-specific ++
/*******************************************************************************
**
** Function         bta_dm_co_ble_io_req
**
** Description      This callout function is executed by DM to get BLE IO capabilities
**                  before SMP pairing gets going.
**
** Parameters       bd_addr  - The peer device
**                  *p_io_cap - The local Input/Output capabilities
**                  *p_oob_data - TRUE, if OOB data is available for the peer device.
**                  *p_auth_req -  Auth request setting (Bonding and MITM required or not)
**                  *p_max_key_size - max key size local device supported.
**                  *p_init_key - initiator keys.
**                  *p_resp_key - responder keys.
**
** Returns          void.
**
*******************************************************************************/
extern void bta_dm_co_ble_io_req(BD_ADDR bd_addr,  tBTA_IO_CAP *p_io_cap,
                                 tBTA_OOB_DATA *p_oob_data,
                                 tBTA_LE_AUTH_REQ *p_auth_req,
                                 UINT8 *p_max_key_size,
                                 tBTA_LE_KEY_TYPE *p_init_key,
                                 tBTA_LE_KEY_TYPE  *p_resp_key );
// btla-specific --

extern void bta_dm_co_ble_set_io_cap(UINT8 ble_io_cap);

extern void bta_dm_co_ble_set_auth_req(UINT8 ble_auth_req);

extern void bta_dm_co_ble_set_init_key_req(UINT8 init_key);

extern void bta_dm_co_ble_set_rsp_key_req(UINT8 rsp_key);

extern void bta_dm_co_ble_set_max_key_size(UINT8 ble_key_size);
#endif
