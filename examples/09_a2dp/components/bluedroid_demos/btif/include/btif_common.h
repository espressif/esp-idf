/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
 *  Copyright (C) 2009-2012 Broadcom Corporation
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

#ifndef BTIF_COMMON_H
#define BTIF_COMMON_H

#include <stdlib.h>
// #include <hardware/bluetooth.h>

#include "bt_types.h"
#include "bta_api.h"
#include "osi.h"

// #include "osi/include/log.h"

/*******************************************************************************
**  Constants & Macros
********************************************************************************/
#define ASSERTC(cond, msg, val) if (!(cond)) { LOG_ERROR( \
    "### ASSERT : %s line %d %s (%d) ###", __FILE__, __LINE__, msg, val);}

/* Calculate start of event enumeration; id is top 8 bits of event */
#define BTIF_SIG_START(id)       ((id) << 8)

/* For upstream the MSB bit is always SET */
#define BTIF_SIG_CB_BIT   (0x8000)
#define BTIF_SIG_CB_START(id)    (((id) << 8) | BTIF_SIG_CB_BIT)

/* BTIF sub-systems */
#define BTIF_CORE           0
#define BTIF_DM             1
// #define BTIF_HFP            2
// #define BTIF_AV             3
// #define BTIF_PAN            4
// #define BTIF_HF_CLIENT      5

#define HAL_CBACK(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        BTIF_TRACE_API("HAL %s->%s", #P_CB, #P_CBACK); \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        ASSERTC(0, "Callback is NULL", 0);  \
    }

/**
 * BTIF events for requests that require context switch to btif task
 * on downstreams path
 */
enum {
    BTIF_CORE_API_START = BTIF_SIG_START(BTIF_CORE),
    /* add here */

    BTIF_DM_API_START = BTIF_SIG_START(BTIF_DM),
    BTIF_DM_ENABLE_SERVICE,
    BTIF_DM_DISABLE_SERVICE,
    /* add here */

};

enum {
    SIG_BTIF_WORK = 0xff
};

/*******************************************************************************
**  Type definitions for callback functions
********************************************************************************/

typedef void (tBTIF_CBACK) (UINT16 event, char *p_param);
typedef void (tBTIF_COPY_CBACK) (UINT16 event, char *p_dest, char *p_src);


/*******************************************************************************
**  Type definitions and return values
********************************************************************************/

/* this type handles all btif context switches between BTU and HAL */
typedef struct {
    BT_HDR               hdr;
    tBTIF_CBACK         *p_cb;    /* context switch callback */

    /* parameters passed to callback */
    UINT16               event;   /* message event id */
    char                 p_param[0]; /* parameter area needs to be last */
} tBTIF_CONTEXT_SWITCH_CBACK;


/*******************************************************************************
**  Functions
********************************************************************************/

bt_status_t btif_transfer_context (tBTIF_CBACK *p_cback, UINT16 event, char *p_params,
                                   int param_len, tBTIF_COPY_CBACK *p_copy_cback);
tBTA_SERVICE_MASK btif_get_enabled_services_mask(void);
bt_status_t btif_enable_service(tBTA_SERVICE_ID service_id);
bt_status_t btif_disable_service(tBTA_SERVICE_ID service_id);
int btif_is_enabled(void);

/**
 * BTIF_Events
 */
void btif_enable_bluetooth_evt(tBTA_STATUS status);
void btif_disable_bluetooth_evt(void);



#endif /* BTIF_COMMON_H */
