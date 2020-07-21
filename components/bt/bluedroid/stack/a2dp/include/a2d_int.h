/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
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
 *  2DP internal header file
 *
 ******************************************************************************/
#ifndef A2D_INT_H
#define A2D_INT_H

#include "stack/a2d_api.h"
#if (A2D_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/
#define A2D_VERSION             0x0102

/* Number of attributes in A2D SDP record. */
#define A2D_NUM_ATTR            6

/* Number of protocol elements in protocol element list. */
#define A2D_NUM_PROTO_ELEMS     2

/*****************************************************************************
**  Type definitions
*****************************************************************************/

/* Control block used by A2D_FindService(). */
typedef struct {
    tA2D_FIND_CBACK     *p_cback;       /* pointer to application callback */
    tSDP_DISCOVERY_DB   *p_db;          /* pointer to discovery database */
    UINT16              service_uuid;   /* service UUID of search */
} tA2D_FIND_CB;

typedef struct {
    tA2D_FIND_CB    find;   /* find service control block */
    UINT8           trace_level;
    UINT16          avdt_sdp_ver;   /* AVDTP version */
} tA2D_CB;


#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
** Main Control Block
*******************************************************************************/
#if A2D_DYNAMIC_MEMORY == FALSE
extern tA2D_CB  a2d_cb;
#else
extern tA2D_CB *a2d_cb_ptr;
#define a2d_cb (*a2d_cb_ptr)
#endif

/* Used only for conformance testing */
extern void a2d_set_avdt_sdp_ver (UINT16 avdt_sdp_ver);

#ifdef __cplusplus
}
#endif
#endif  ///A2D_INCLUDED == TRUE
#endif /* A2D_INT_H */
