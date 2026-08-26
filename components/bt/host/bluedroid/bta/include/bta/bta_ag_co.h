/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  This is the interface file for audio gateway call-out  and call-in functions.
 *
 ******************************************************************************/
#ifndef BTA_AG_CO_H
#define BTA_AG_CO_H

#include "bta/bta_ag_api.h"
#include "hci/hci_audio.h"

#if (BTA_AG_INCLUDED == TRUE)
#if (BTM_SCO_HCI_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bta_ag_sco_audio_state
**
** Description      This function is called by the AG before the audio connection
**                  is brought up, after it comes up, and after it goes down.
**
** Parameters       handle - handle of the AG instance
**                  state - Audio state
**                  codec - if WBS support is compiled in, codec to going to be used is provided
**                      and when in SCO_STATE_SETUP, BTM_I2SPCMConfig() must be called with
**                      the correct platform parameters.
**                      in the other states codec type should not be ignored
**
** Returns          void
**
*******************************************************************************/
#if (BTM_WBS_INCLUDED == TRUE)
void bta_ag_sco_audio_state(UINT16 handle, UINT8 app_id, UINT8 state, tBTA_AG_PEER_CODEC codec);
#else
void bta_ag_sco_audio_state(UINT16 handle, UINT8 app_id, UINT8 state);
#endif

/*******************************************************************************
**
** Function         bta_ag_sco_co_init
**
** Description      Set default data path for SCO/eSCO.
**                  This callout function is executed by AG when it is
**                  started by calling BTA_AgEnable().  This function can be
**                  used by the phone to initialize audio paths or for other
**                  initialization purposes.
**
**
** Returns          Void.
**
*******************************************************************************/
tBTA_HFP_SCO_ROUTE_TYPE bta_ag_sco_co_init(UINT32 rx_bw, UINT32 tx_bw, tBTA_HFP_CODEC_INFO *p_codec_info, UINT8 app_id);

/*******************************************************************************
**
** Function         bta_ag_sco_co_open
**
** Description      This function is executed by AG when a service level connection
**                  is opened.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_sco_co_open(UINT16 handle, tBTM_SCO_AIR_MODE_TYPE air_mode, UINT8 inout_pkt_size, UINT16 event);

/*******************************************************************************
**
** Function         bta_ag_sco_co_close
**
** Description      This function is called by AG when a service level
**                  connection is closed.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_sco_co_close(void);

/*******************************************************************************
**
** Function         bta_ag_sco_co_out_data
**
** Description      This function is called to send SCO data over HCI.
**
** Returns          number of bytes got from application
**
*******************************************************************************/
uint32_t bta_ag_sco_co_out_data(UINT8 *p_buf);

/*******************************************************************************
**
** Function         bta_hf_client_sco_co_in_data
**
** Description      This function is called to send incoming SCO data to application.
**
** Returns          void
**
*******************************************************************************/
void bta_ag_sco_co_in_data(BT_HDR  *p_buf, tBTM_SCO_DATA_FLAG status);

/*******************************************************************************
**
** Function         bta_ag_co_tx_write
**
** Description      This function is called by the AG to send data to the
**                  phone when the AG is configured for AT command pass-through.
**                  The implementation of this function must copy the data to
**                  the phones memory.
**
** Returns          void
**
*******************************************************************************/
void bta_ag_co_tx_write(UINT16 handle, UINT8 *p_data, UINT16 len);

/*******************************************************************************
**
** Function         bta_ag_ci_rx_write
**
** Description      This function is called to send data to the AG when the AG
**                  is configured for AT command pass-through.  The function
**                  copies data to an event buffer and sends it.
**
** Returns          void
**
*******************************************************************************/
extern void bta_ag_ci_rx_write(UINT16 handle, char *p_data, UINT16 len);

/******************************************************************************
**
** Function         bta_ag_ci_slc_ready
**
** Description      This function is called to notify AG that SLC is up at
**                  the application. This funcion is only used when the app
**                  is running in pass-through mode.
**
** Returns          void
**
******************************************************************************/
extern void bta_ag_ci_slc_ready(UINT16 handle);

#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE) */

#endif /* #if (BTA_AG_INCLUDED == TRUE) */

#endif /* BTA_AG_CO_H */
