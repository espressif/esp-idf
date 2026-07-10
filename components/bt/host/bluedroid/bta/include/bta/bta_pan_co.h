/******************************************************************************
 *
 *  Copyright (C) 2004-2012 Broadcom Corporation
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
 *  This is the interface file for data gateway call-out functions.
 *
 ******************************************************************************/
#ifndef BTA_PAN_CO_H
#define BTA_PAN_CO_H

#include "bta_pan_api.h"

/*****************************************************************************
 *  Constants
 ****************************************************************************/

/* BT_HDR buffer offset */
#define BTA_PAN_MIN_OFFSET PAN_MINIMUM_OFFSET

/* Data Flow Mask */
#define BTA_PAN_RX_PUSH 0x00     /* RX push. */
#define BTA_PAN_RX_PUSH_BUF 0x01 /* RX push with zero copy. */
#define BTA_PAN_RX_PULL 0x02     /* RX pull. */
#define BTA_PAN_TX_PUSH 0x00     /* TX push. */
#define BTA_PAN_TX_PUSH_BUF 0x10 /* TX push with zero copy. */
#define BTA_PAN_TX_PULL 0x20     /* TX pull. */

/*****************************************************************************
 *  Function Declarations
 ****************************************************************************/

/*******************************************************************************
 *
 * Function         bta_pan_co_init
 *
 * Description      This callout function is executed by PAN when a server is
 *                  started by calling BTA_PanEnable().  This function can be
 *                  used by the phone to initialize data paths or for other
 *                  initialization purposes.  The function must return the
 *                  data flow mask as described below.
 *
 *
 * Returns          Data flow mask.
 *
 ******************************************************************************/
extern UINT8 bta_pan_co_init(UINT8 *q_level);

/*******************************************************************************
 *
 * Function         bta_pan_co_open
 *
 * Description      This function is executed by PAN when a connection
 *                  is opened.  The phone can use this function to set
 *                  up data paths or perform any required initialization.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_co_open(UINT16 handle, UINT8 app_id,
                            tBTA_PAN_ROLE local_role, tBTA_PAN_ROLE peer_role,
                            BD_ADDR peer_addr);

/*******************************************************************************
 *
 * Function         bta_pan_co_close
 *
 * Description      This function is called by PAN when a connection to a
 *                  server is closed.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_co_close(UINT16 handle, UINT8 app_id);

/*******************************************************************************
 *
 * Function         bta_pan_co_tx_path
 *
 * Description      This function is called by PAN to transfer data on the
 *                  TX path; that is, data being sent from BTA to the phone.
 *                  This function is used when the TX data path is configured
 *                  to use the pull interface.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_co_tx_path(UINT16 handle, UINT8 app_id);

/*******************************************************************************
 *
 * Function         bta_pan_co_rx_path
 *
 * Description      This function is called by PAN to transfer data on the
 *                  RX path; that is, data being sent from the phone to BTA.
 *                  This function is used when the RX data path is configured
 *                  to use the pull interface.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_co_rx_path(UINT16 handle, UINT8 app_id);

/*******************************************************************************
 *
 * Function         bta_pan_co_tx_write
 *
 * Description      This function is called by PAN to send data to the phone
 *                  when the TX path is configured to use a push interface.
 *                  The implementation of this function must copy the data to
 *                  the phone's memory.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_co_tx_write(UINT16 handle, UINT8 app_id,
                                BD_ADDR src, BD_ADDR dst,
                                UINT16 protocol, UINT8 *p_data,
                                UINT16 len, BOOLEAN ext, BOOLEAN forward);

/*******************************************************************************
 *
 * Function         bta_pan_co_tx_writebuf
 *
 * Description      This function is called by PAN to send data to the phone
 *                  when the TX path is configured to use a push interface with
 *                  zero copy.  The phone must free the buffer using function
 *                  osi_free() when it is through processing the buffer.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_co_tx_writebuf(UINT16 handle, UINT8 app_id,
                                   BD_ADDR src, BD_ADDR dst,
                                   UINT16 protocol, BT_HDR *p_buf, BOOLEAN ext,
                                   BOOLEAN forward);

/*******************************************************************************
 *
 * Function         bta_pan_co_rx_flow
 *
 * Description      This function is called by PAN to enable or disable
 *                  data flow on the RX path when it is configured to use
 *                  a push interface.  If data flow is disabled the phone must
 *                  not call bta_pan_ci_rx_write() or bta_pan_ci_rx_writebuf()
 *                  until data flow is enabled again.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_co_rx_flow(UINT16 handle, UINT8 app_id, BOOLEAN enable);

/*******************************************************************************
 *
 * Function         bta_pan_co_filt_ind
 *
 * Description      protocol filter indication from peer device
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_co_pfilt_ind(UINT16 handle, BOOLEAN indication,
                                 tBTA_PAN_STATUS result, UINT16 len,
                                 UINT8 *p_filters);

/*******************************************************************************
 *
 * Function         bta_pan_co_mfilt_ind
 *
 * Description      multicast filter indication from peer device
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_co_mfilt_ind(UINT16 handle, BOOLEAN indication,
                                 tBTA_PAN_STATUS result, UINT16 len,
                                 UINT8 *p_filters);

#endif /* BTA_PAN_CO_H */
