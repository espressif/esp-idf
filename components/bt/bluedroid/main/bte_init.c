/******************************************************************************
 *
 *  Copyright (C) 2000-2012 Broadcom Corporation
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
 *  This module contains the routines that initialize the stack components.
 *  It must be called before the BTU task is started.
 *
 ******************************************************************************/

#include "bt_target.h"
#include <string.h>


/* Stack Configuation Related Init Definaton
 * TODO: Now Just Unmask these defination until stack layer is OK
 */

#ifndef BTA_INCLUDED
#define BTA_INCLUDED FALSE
#endif

#if (defined(RFCOMM_INCLUDED) && RFCOMM_INCLUDED == TRUE)
// Include initialization functions definitions
#include "port_api.h"
#endif

#if (defined(BNEP_INCLUDED) && BNEP_INCLUDED == TRUE)
#include "bnep_api.h"
#endif

#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE)
#include "gap_api.h"
#endif

#if (defined(PAN_INCLUDED) && PAN_INCLUDED == TRUE)
#include "pan_api.h"
#endif

#if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE)
#include "avrc_api.h"
#endif

#if (defined(A2D_INCLUDED) && A2D_INCLUDED == TRUE)
#include "a2d_api.h"
#endif

#if (defined(AVDT_INCLUDED) && AVDT_INCLUDED == TRUE)
#include "avdt_int.h"
#endif

#if (defined(AVCT_INCLUDED) && AVCT_INCLUDED == TRUE)
#include "avct_int.h"
#endif

#if (defined(HID_HOST_INCLUDED) && HID_HOST_INCLUDED == TRUE)
#include "hidh_api.h"
#endif

#if (defined(MCA_INCLUDED) && MCA_INCLUDED == TRUE)
#include "mca_api.h"
#endif

#if (defined(BLE_INCLUDED) && BLE_INCLUDED == TRUE)
#include "gatt_api.h"
#if (defined(SMP_INCLUDED) && SMP_INCLUDED == TRUE)
#include "smp_api.h"
#endif
#endif

//BTA Modules
#if BTA_INCLUDED == TRUE && BTA_DYNAMIC_MEMORY == TRUE
#include "bta_api.h"
#include "bta_sys.h"
#include "allocator.h"

//#include "bta_ag_int.h"

#if BTA_SDP_INCLUDED == TRUE
#include "bta_sdp_int.h"
#endif

#if BTA_HS_INCLUDED == TRUE
#include "bta_hs_int.h"
#endif

#include "bta_dm_int.h"

#if BTA_AR_INCLUDED==TRUE
#include "bta_ar_int.h"
#endif
#if BTA_AV_INCLUDED==TRUE
#include "bta_av_int.h"
#endif

#if BTA_HH_INCLUDED==TRUE
#include "bta_hh_int.h"
#endif

#if BTA_JV_INCLUDED==TRUE
#include "bta_jv_int.h"
tBTA_JV_CB *bta_jv_cb_ptr = NULL;
#endif

#if BTA_HL_INCLUDED == TRUE
#include "bta_hl_int.h"
#endif

#if BTA_GATT_INCLUDED == TRUE
#include "bta_gattc_int.h"
#include "bta_gatts_int.h"
#endif

#if BTA_PAN_INCLUDED==TRUE
#include "bta_pan_int.h"
#endif

#include "bta_sys_int.h"

// control block for patch ram downloading
//#include "bta_prm_int.h"

#endif // BTA_INCLUDED == TRUE && BTA_DYNAMIC_MEMORY == TRUE


/*****************************************************************************
**                          F U N C T I O N S                                *
******************************************************************************/

/*****************************************************************************
**
** Function         BTE_InitStack
**
** Description      Initialize control block memory for each component.
**
**                  Note: The core stack components must be called
**                      before creating the BTU Task.  The rest of the
**                      components can be initialized at a later time if desired
**                      as long as the component's init function is called
**                      before accessing any of its functions.
**
** Returns          void
**
******************************************************************************/
void BTE_InitStack(void)
{
#if (defined(RFCOMM_INCLUDED) && RFCOMM_INCLUDED == TRUE)
    //Initialize the optional stack components
    RFCOMM_Init();
#endif

    //BNEP and its profiles
#if (defined(BNEP_INCLUDED) && BNEP_INCLUDED == TRUE)
    BNEP_Init();

#if (defined(PAN_INCLUDED) && PAN_INCLUDED == TRUE)
    PAN_Init();
#endif  // PAN
#endif  // BNEP Included


    //AVDT and its profiles
#if (defined(A2D_INCLUDED) && A2D_INCLUDED == TRUE)
    A2D_Init();
#endif  // AADP

#if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE)
    AVRC_Init();
#endif

#if (defined(AVDT_INCLUDED) && AVDT_INCLUDED == TRUE && AVDT_DYNAMIC_MEMORY == TRUE)
    if ((avdt_cb_ptr = (tAVDT_CB *)osi_malloc(sizeof(tAVDT_CB))) == NULL) {
        return;
    }
    memset((void *)avdt_cb_ptr, 0, sizeof(tAVDT_CB));
#endif

#if (defined(AVCT_INCLUDED) && AVCT_INCLUDED == TRUE && AVCT_DYNAMIC_MEMORY == TRUE)
    if ((avct_cb_ptr = (tAVCT_CB *)osi_malloc(sizeof(tAVCT_CB))) == NULL) {
        return;
    }
    memset((void *)avct_cb_ptr, 0, sizeof(tAVCT_CB));
#endif

#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE)
    GAP_Init();
#endif

#if (defined(HID_HOST_INCLUDED) && HID_HOST_INCLUDED == TRUE)
    HID_HostInit();
#endif

#if (defined(MCA_INCLUDED) && MCA_INCLUDED == TRUE)
    MCA_Init();
#endif

    //BTA Modules
#if (BTA_INCLUDED == TRUE && BTA_DYNAMIC_MEMORY == TRUE)
    if ((bta_sys_cb_ptr = (tBTA_SYS_CB *)osi_malloc(sizeof(tBTA_SYS_CB))) == NULL) {
        return;
    }
    if ((bta_dm_cb_ptr = (tBTA_DM_CB *)osi_malloc(sizeof(tBTA_DM_CB))) == NULL) {
        return;
    }
    if ((bta_dm_search_cb_ptr = (tBTA_DM_SEARCH_CB *)osi_malloc(sizeof(tBTA_DM_SEARCH_CB))) == NULL) {
        return;
    }
    if ((bta_dm_di_cb_ptr = (tBTA_DM_DI_CB *)osi_malloc(sizeof(tBTA_DM_DI_CB))) == NULL) {
        return;
    }
    memset((void *)bta_sys_cb_ptr, 0, sizeof(tBTA_SYS_CB));
    memset((void *)bta_dm_cb_ptr, 0, sizeof(tBTA_DM_CB));
    memset((void *)bta_dm_search_cb_ptr, 0, sizeof(tBTA_DM_SEARCH_CB));
    memset((void *)bta_dm_di_cb_ptr, 0, sizeof(tBTA_DM_DI_CB));
    //memset((void *)bta_prm_cb_ptr, 0, sizeof(tBTA_PRM_CB));
    //memset((void *)bta_ag_cb_ptr, 0, sizeof(tBTA_AG_CB));
#if BTA_HS_INCLUDED == TRUE
    memset((void *)bta_hs_cb_ptr, 0, sizeof(tBTA_HS_CB));
#endif
#if BTA_SDP_INCLUDED == TRUE
    if ((bta_sdp_cb_ptr = (tBTA_SDP_CB *)osi_malloc(sizeof(tBTA_SDP_CB))) == NULL) {
        return;
    }
    memset((void *)bta_sdp_cb_ptr, 0, sizeof(tBTA_SDP_CB));
#endif
#if BTA_AR_INCLUDED==TRUE
    if ((bta_ar_cb_ptr = (tBTA_AR_CB *)osi_malloc(sizeof(tBTA_AR_CB))) == NULL) {
        return;
    }
    memset((void *)bta_ar_cb_ptr, 0, sizeof(tBTA_AR_CB));
#endif
#if BTA_AV_INCLUDED==TRUE
    if ((bta_av_cb_ptr = (tBTA_AV_CB *)osi_malloc(sizeof(tBTA_AV_CB))) == NULL) {
        return;
    }
    memset((void *)bta_av_cb_ptr, 0, sizeof(tBTA_AV_CB));
#endif
#if BTA_HH_INCLUDED==TRUE
    if ((bta_hh_cb_ptr = (tBTA_HH_CB *)osi_malloc(sizeof(tBTA_HH_CB))) == NULL) {
        return;
    } 
    memset((void *)bta_hh_cb_ptr, 0, sizeof(tBTA_HH_CB));
#endif
#if BTA_HL_INCLUDED==TRUE
    memset((void *)bta_hl_cb_ptr, 0, sizeof(tBTA_HL_CB));
#endif
#if GATTC_INCLUDED==TRUE
    if ((bta_gattc_cb_ptr = (tBTA_GATTC_CB *)osi_malloc(sizeof(tBTA_GATTC_CB))) == NULL) {
        return;
    }
    memset((void *)bta_gattc_cb_ptr, 0, sizeof(tBTA_GATTC_CB));
#endif
#if GATTS_INCLUDED == TRUE
    if ((bta_gatts_cb_ptr = (tBTA_GATTS_CB *)osi_malloc(sizeof(tBTA_GATTS_CB))) == NULL) {
        return;
    }
    memset((void *)bta_gattc_cb_ptr, 0, sizeof(tBTA_GATTC_CB));
    //
    memset((void *)bta_gatts_cb_ptr, 0, sizeof(tBTA_GATTS_CB));
#endif
#if BTA_PAN_INCLUDED==TRUE
    memset((void *)bta_pan_cb_ptr, 0, sizeof(tBTA_PAN_CB));
#endif

#endif // BTA_INCLUDED == TRUE
}
