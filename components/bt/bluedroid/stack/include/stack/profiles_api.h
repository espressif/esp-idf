/******************************************************************************
 *
 *  Copyright (C) 2009-2013 Broadcom Corporation
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

#ifndef PROFILES_API_H
#define PROFILES_API_H

#include "common/bt_target.h"
#include "stack/btm_api.h"

/*****************************************************************************
**  Constants
*****************************************************************************/
#define BT_PASS         0   /* Used for general successful function returns */

/*** Port entity passes back 8 bit errors; will use upper byte offset ***/
#define PORT_ERR_GRP    0x0000          /* base offset for port entity */
#define GAP_ERR_GRP     0x0100          /* base offset for GAP profile */
#define SPP_ERR_GRP     0x0200          /* base offset for serial port profile */
#define HCRP_ERR_GRP    0x0300          /* base offset for HCRP */
#define HCRPM_ERR_GRP   0x0400          /* base offset for HCRPM */

/* #define HSP2_ERR_GRP 0x0F00 */

/* security level definitions (tBT_SECURITY) */
#define BT_USE_DEF_SECURITY         0
#define BT_SEC_MODE_NONE            BTM_SEC_MODE_NONE
#define BT_SEC_MODE_SERVICE         BTM_SEC_MODE_SERVICE
#define BT_SEC_MODE_LINK            BTM_SEC_MODE_LINK

/* security mask definitions (tBT_SECURITY) */
/* The following definitions are OR'd together to form the security requirements */
#define BT_SEC_IN_AUTHORIZE         BTM_SEC_IN_AUTHORIZE         /* Inbound call requires authorization */
#define BT_SEC_IN_AUTHENTICATE      BTM_SEC_IN_AUTHENTICATE      /* Inbound call requires authentication */
#define BT_SEC_IN_ENCRYPT           BTM_SEC_IN_ENCRYPT           /* Inbound call requires encryption */
#define BT_SEC_OUT_AUTHORIZE        BTM_SEC_OUT_AUTHORIZE        /* Outbound call requires authorization */
#define BT_SEC_OUT_AUTHENTICATE     BTM_SEC_OUT_AUTHENTICATE     /* Outbound call requires authentication */
#define BT_SEC_OUT_ENCRYPT          BTM_SEC_OUT_ENCRYPT          /* Outbound call requires encryption */


/*****************************************************************************
**  Type Definitions
*****************************************************************************/

/*
** Security Definitions
**      This following definitions are used to indicate the security
**      requirements for a service.
*/
typedef struct {
    UINT8   level;
    UINT8   mask;
} tBT_SECURITY;

#endif  /* PROFILES_API_H */

