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
 *  This is the interface file for pan call-in functions.
 *
 ******************************************************************************/
#ifndef BTA_PAN_CI_H
#define BTA_PAN_CI_H

#include "bta_pan_api.h"

/*****************************************************************************
 *  Function Declarations
 ****************************************************************************/
/*******************************************************************************
 *
 * Function         bta_pan_ci_tx_ready
 *
 * Description      This function sends an event to PAN indicating the phone is
 *                  ready for more data and PAN should call
 *                  bta_pan_co_tx_path().
 *                  This function is used when the TX data path is configured
 *                  to use a pull interface.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_ci_tx_ready(UINT16 handle);

/*******************************************************************************
 *
 * Function         bta_pan_ci_rx_ready
 *
 * Description      This function sends an event to PAN indicating the phone
 *                  has data available to send to PAN and PAN should call
 *                  bta_pan_co_rx_path().  This function is used when the RX
 *                  data path is configured to use a pull interface.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_ci_rx_ready(UINT16 handle);

/*******************************************************************************
 *
 * Function         bta_pan_ci_tx_flow
 *
 * Description      This function is called to enable or disable data flow on
 *                  the TX path.  The phone should call this function to
 *                  disable data flow when it is congested and cannot handle
 *                  any more data sent by bta_pan_co_tx_write() or
 *                  bta_pan_co_tx_writebuf().  This function is used when the
 *                  TX data path is configured to use a push interface.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_ci_tx_flow(UINT16 handle, BOOLEAN enable);

/*******************************************************************************
 *
 * Function         bta_pan_ci_rx_write
 *
 * Description      This function is called to send data to PAN when the RX path
 *                  is configured to use a push interface.
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_ci_rx_write(UINT16 handle, BD_ADDR dst, BD_ADDR src,
                                UINT16 protocol, UINT8 *p_data, UINT16 len,
                                BOOLEAN ext);

/*******************************************************************************
 *
 * Function         bta_pan_ci_rx_writebuf
 *
 * Description      This function is called to send data to the phone when
 *                  the RX path is configured to use a push interface with
 *                  zero copy.  The function sends an event to PAN containing
 *                  the data buffer. The buffer will be freed by BTA; the
 *                  phone must not free the buffer.
 *
 *
 * Returns          true if flow enabled
 *
 ******************************************************************************/
extern void bta_pan_ci_rx_writebuf(UINT16 handle, BD_ADDR dst,
                                   BD_ADDR src, UINT16 protocol,
                                   BT_HDR *p_buf, BOOLEAN ext);

/*******************************************************************************
 *
 * Function         bta_pan_ci_readbuf
 *
 * Description      This function is called by the phone to read data from PAN
 *                  when the TX path is configured to use a pull interface.
 *                  The caller must free the buffer when it is through
 *                  processing the buffer.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern BT_HDR *bta_pan_ci_readbuf(UINT16 handle, BD_ADDR src,
                                  BD_ADDR dst, UINT16 *p_protocol,
                                  BOOLEAN *p_ext, BOOLEAN *p_forward);

/*******************************************************************************
 *
 * Function         bta_pan_ci_set_pfilters
 *
 * Description      This function is called to set protocol filters
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_ci_set_pfilters(UINT16 handle, UINT16 num_filters,
                                    UINT16 *p_start_array,
                                    UINT16 *p_end_array);

/*******************************************************************************
 *
 * Function         bta_pan_ci_set_mfilters
 *
 * Description      This function is called to set multicast filters
 *
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_pan_ci_set_mfilters(UINT16 handle, UINT16 num_mcast_filters,
                                    UINT8 *p_start_array,
                                    UINT8 *p_end_array);

#endif /* BTA_PAN_CI_H */
