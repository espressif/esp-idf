/******************************************************************************
 *
 *  Copyright (C) 2007-2012 Broadcom Corporation
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
 *  This is the interface file for java interface call-out functions.
 *
 ******************************************************************************/
#ifndef BTA_JV_CO_H
#define BTA_JV_CO_H

#include "bta_jv_api.h"

#if (defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE)
/*****************************************************************************
**  Function Declarations
*****************************************************************************/


/*******************************************************************************
**
** Function         bta_jv_co_rfc_data
**
** Description      This function is called by JV to send data to the java glue
**                  code when the RX data path is configured to use a call-out
**
** Returns          void
**
*******************************************************************************/

extern int bta_co_rfc_data_incoming(void *user_data, BT_HDR *p_buf);
extern int bta_co_rfc_data_outgoing_size(void *user_data, int *size);
extern int bta_co_rfc_data_outgoing(void *user_data, UINT8 *buf, UINT16 size);

extern int bta_co_l2cap_data_incoming(void *user_data, BT_HDR *p_buf);
extern int bta_co_l2cap_data_outgoing_size(void *user_data, int *size);
extern int bta_co_l2cap_data_outgoing(void *user_data, UINT8 *buf, UINT16 size);

#endif  ///defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE
#endif /* BTA_DG_CO_H */
