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

#include "common/bt_target.h"
#include <string.h>


/* Stack Configuration Related Init Definaton
 * TODO: Now Just Unmask these definition until stack layer is OK
 */

#ifndef BTA_INCLUDED
#define BTA_INCLUDED FALSE
#endif

#if (defined(RFCOMM_INCLUDED) && RFCOMM_INCLUDED == TRUE)
// Include initialization functions definitions
#include "stack/port_api.h"
#endif

#if (defined(BNEP_INCLUDED) && BNEP_INCLUDED == TRUE)
#include "bnep_api.h"
#endif

#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE)
#include "stack/gap_api.h"
#endif

#if (defined(PAN_INCLUDED) && PAN_INCLUDED == TRUE)
#include "pan_api.h"
#endif

#if (defined(HID_HOST_INCLUDED) && HID_HOST_INCLUDED == TRUE)
#include "stack/hidh_api.h"
#endif

#if (defined(HID_DEV_INCLUDED) && HID_DEV_INCLUDED == TRUE)
#include "stack/hidd_api.h"
#endif

#if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE)
#include "stack/avrc_api.h"
#endif

#if (defined(A2D_INCLUDED) && A2D_INCLUDED == TRUE)
#include "stack/a2d_api.h"
#endif

#if (defined(AVDT_INCLUDED) && AVDT_INCLUDED == TRUE)
#include "avdt_int.h"
#endif

#if (defined(AVCT_INCLUDED) && AVCT_INCLUDED == TRUE)
#include "avct_int.h"
#endif

#if (defined(HID_HOST_INCLUDED) && HID_HOST_INCLUDED == TRUE)
#include "stack/hidh_api.h"
#endif

#if (defined(MCA_INCLUDED) && MCA_INCLUDED == TRUE)
#include "mca_api.h"
#endif

#if (defined(BLE_INCLUDED) && BLE_INCLUDED == TRUE)
#include "stack/gatt_api.h"
#if (defined(SMP_INCLUDED) && SMP_INCLUDED == TRUE)
#include "stack/smp_api.h"
#endif
#endif

#if (defined(OBEX_INCLUDED) && OBEX_INCLUDED == TRUE)
#include "stack/obex_api.h"
#endif

#if (defined(GOEPC_INCLUDED) && GOEPC_INCLUDED == TRUE)
#include "stack/goep_common.h"
#include "stack/goepc_api.h"
#endif

//BTA Modules
#if BTA_INCLUDED == TRUE && BTA_DYNAMIC_MEMORY == TRUE
#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "osi/allocator.h"

#if BTA_HF_INCLUDED == TRUE
#include "bta_hf_client_int.h"
#endif

#if BTA_AG_INCLUDED == TRUE
#include "bta_ag_int.h"
#endif

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

#if BTA_HD_INCLUDED==TRUE
#include "bta_hd_int.h"
#endif

#if BTA_JV_INCLUDED==TRUE
#include "bta_jv_int.h"
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
** Function         BTE_DeinitStack
**
** Description      Deinitialize control block memory for each component.
**
**                  Note: This API must be called
**                      after freeing the BTU Task.
**
** Returns          void
**
******************************************************************************/
void BTE_DeinitStack(void)
{
    //BTA Modules
#if (BTA_INCLUDED == TRUE && BTA_DYNAMIC_MEMORY == TRUE)
#if GATTS_INCLUDED == TRUE
    if (bta_gatts_cb_ptr){
        osi_free(bta_gatts_cb_ptr);
        bta_gatts_cb_ptr = NULL;
    }
#endif
#if GATTC_INCLUDED==TRUE
    if (bta_gattc_cb_ptr){
        osi_free(bta_gattc_cb_ptr);
        bta_gattc_cb_ptr = NULL;
    }
#endif
#if BTA_HD_INCLUDED==TRUE
    if (bta_hd_cb_ptr){
        osi_free(bta_hd_cb_ptr);
        bta_hd_cb_ptr = NULL;
    }
#endif
#if BTA_HH_INCLUDED==TRUE
    if (bta_hh_cb_ptr){
        osi_free(bta_hh_cb_ptr);
        bta_hh_cb_ptr = NULL;
    }
#endif
#if BTA_AV_INCLUDED==TRUE
    if (bta_av_cb_ptr){
        osi_free(bta_av_cb_ptr);
        bta_av_cb_ptr = NULL;
    }
    if (bta_av_sbc_ups_cb_ptr){
        osi_free(bta_av_sbc_ups_cb_ptr);
        bta_av_sbc_ups_cb_ptr = NULL;
    }
#endif
#if BTA_AR_INCLUDED==TRUE
    if (bta_ar_cb_ptr){
        osi_free(bta_ar_cb_ptr);
        bta_ar_cb_ptr = NULL;
    }
#endif
#if SDP_INCLUDED == TRUE
    if (g_disc_raw_data_buf){
        osi_free(g_disc_raw_data_buf);
        g_disc_raw_data_buf = NULL;
    }
#endif
#if BTA_SDP_INCLUDED == TRUE
    if (bta_sdp_cb_ptr){
        osi_free(bta_sdp_cb_ptr);
        bta_sdp_cb_ptr = NULL;
    }
#endif
#if (defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE)
    if (bta_jv_cb_ptr){
        osi_free(bta_jv_cb_ptr);
        bta_jv_cb_ptr = NULL;
    }
#endif //JV
#if (defined BTA_HF_INCLUDED && BTA_HF_INCLUDED == TRUE)
    if (bta_hf_client_cb_ptr){
        osi_free(bta_hf_client_cb_ptr);
        bta_hf_client_cb_ptr = NULL;
    }
#endif
#if (defined BTA_AG_INCLUDED && BTA_AG_INCLUDED == TRUE)
    if (bta_ag_cb_ptr){
        osi_free(bta_ag_cb_ptr);
        bta_ag_cb_ptr = NULL;
    }
#endif
    if (bta_dm_conn_srvcs_ptr){
        osi_free(bta_dm_conn_srvcs_ptr);
        bta_dm_conn_srvcs_ptr = NULL;
    }
    if (bta_dm_di_cb_ptr){
        osi_free(bta_dm_di_cb_ptr);
        bta_dm_di_cb_ptr = NULL;
    }
    if (bta_dm_search_cb_ptr){
        osi_free(bta_dm_search_cb_ptr);
        bta_dm_search_cb_ptr = NULL;
    }
    if (bta_dm_cb_ptr){
        osi_free(bta_dm_cb_ptr);
        bta_dm_cb_ptr = NULL;
    }
    if (bta_sys_cb_ptr){
        osi_free(bta_sys_cb_ptr);
        bta_sys_cb_ptr = NULL;
    }
#endif // BTA_INCLUDED == TRUE

#if (defined(GOEPC_INCLUDED) && GOEPC_INCLUDED == TRUE)
    GOEPC_Deinit();
#endif

#if (defined(OBEX_INCLUDED) && OBEX_INCLUDED == TRUE)
    OBEX_Deinit();
#endif

#if (defined(HID_DEV_INCLUDED) && HID_DEV_INCLUDED == TRUE)
    HID_DevDeinit();
#endif

#if (defined(HID_HOST_INCLUDED) && HID_HOST_INCLUDED == TRUE)
    HID_HostDeinit();
#endif

#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE)
    GAP_Deinit();
#endif

#if (defined(AVCT_INCLUDED) && AVCT_INCLUDED == TRUE && AVCT_DYNAMIC_MEMORY == TRUE)
    if (avct_cb_ptr){
        osi_free(avct_cb_ptr);
        avct_cb_ptr = NULL;
    }
#endif

#if (defined(AVDT_INCLUDED) && AVDT_INCLUDED == TRUE && AVDT_DYNAMIC_MEMORY == TRUE)
    if (avdt_cb_ptr){
        osi_free(avdt_cb_ptr);
        avdt_cb_ptr = NULL;
    }
#endif

#if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE)
    AVRC_Deinit();
#endif

#if (defined(A2D_INCLUDED) && A2D_INCLUDED == TRUE)
    A2D_Deinit();
#endif

#if (defined(RFCOMM_INCLUDED) && RFCOMM_INCLUDED == TRUE)
    RFCOMM_Deinit();
#endif
}

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
** Returns          status
**
******************************************************************************/
bt_status_t BTE_InitStack(void)
{
#if (defined(RFCOMM_INCLUDED) && RFCOMM_INCLUDED == TRUE)
    //Initialize the optional stack components
    if (RFCOMM_Init() != BT_STATUS_SUCCESS) {
        goto error_exit;
    }
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
    if (A2D_Init() != BT_STATUS_SUCCESS) {
        goto error_exit;
    }
#endif  // AADP

#if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE)
    if (AVRC_Init() != BT_STATUS_SUCCESS) {
        goto error_exit;
    }
#endif

#if (defined(AVDT_INCLUDED) && AVDT_INCLUDED == TRUE && AVDT_DYNAMIC_MEMORY == TRUE)
    if ((avdt_cb_ptr = (tAVDT_CB *)osi_malloc(sizeof(tAVDT_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)avdt_cb_ptr, 0, sizeof(tAVDT_CB));
#endif

#if (defined(AVCT_INCLUDED) && AVCT_INCLUDED == TRUE && AVCT_DYNAMIC_MEMORY == TRUE)
    if ((avct_cb_ptr = (tAVCT_CB *)osi_malloc(sizeof(tAVCT_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)avct_cb_ptr, 0, sizeof(tAVCT_CB));
#endif

#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE)
    if (GAP_Init() != BT_STATUS_SUCCESS) {
        goto error_exit;
    }
#endif

#if (defined(HID_HOST_INCLUDED) && HID_HOST_INCLUDED == TRUE)
    if (HID_HostInit() != HID_SUCCESS) {
        goto error_exit;
    }
#endif

#if (defined(HID_DEV_INCLUDED) && HID_DEV_INCLUDED == TRUE)
    if (HID_DevInit() != HID_SUCCESS) {
        goto error_exit;
    }
#endif

#if (defined(MCA_INCLUDED) && MCA_INCLUDED == TRUE)
    MCA_Init();
#endif

#if (defined(OBEX_INCLUDED) && OBEX_INCLUDED == TRUE)
    if (OBEX_Init() != OBEX_SUCCESS) {
        goto error_exit;
    }
#endif

#if (defined(GOEPC_INCLUDED) && GOEPC_INCLUDED == TRUE)
    if (GOEPC_Init() != GOEP_SUCCESS) {
        goto error_exit;
    }
#endif

    //BTA Modules
#if (BTA_INCLUDED == TRUE && BTA_DYNAMIC_MEMORY == TRUE)
    if ((bta_sys_cb_ptr = (tBTA_SYS_CB *)osi_malloc(sizeof(tBTA_SYS_CB))) == NULL) {
        goto error_exit;
    }
    if ((bta_dm_cb_ptr = (tBTA_DM_CB *)osi_malloc(sizeof(tBTA_DM_CB))) == NULL) {
        goto error_exit;
    }
    if ((bta_dm_search_cb_ptr = (tBTA_DM_SEARCH_CB *)osi_malloc(sizeof(tBTA_DM_SEARCH_CB))) == NULL) {
        goto error_exit;
    }
    if ((bta_dm_di_cb_ptr = (tBTA_DM_DI_CB *)osi_malloc(sizeof(tBTA_DM_DI_CB))) == NULL) {
        goto error_exit;
    }
    if ((bta_dm_conn_srvcs_ptr = (tBTA_DM_CONNECTED_SRVCS *)osi_malloc(sizeof(tBTA_DM_CONNECTED_SRVCS))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_sys_cb_ptr, 0, sizeof(tBTA_SYS_CB));
    memset((void *)bta_dm_cb_ptr, 0, sizeof(tBTA_DM_CB));
    memset((void *)bta_dm_search_cb_ptr, 0, sizeof(tBTA_DM_SEARCH_CB));
    memset((void *)bta_dm_di_cb_ptr, 0, sizeof(tBTA_DM_DI_CB));
    memset((void *)bta_dm_conn_srvcs_ptr, 0, sizeof(tBTA_DM_CONNECTED_SRVCS));
    //memset((void *)bta_prm_cb_ptr, 0, sizeof(tBTA_PRM_CB));

#if (defined BTA_HF_INCLUDED && BTA_HF_INCLUDED == TRUE)
    if ((bta_hf_client_cb_ptr = (tBTA_HF_CLIENT_CB *)osi_malloc(sizeof(tBTA_HF_CLIENT_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_hf_client_cb_ptr, 0, sizeof(tBTA_HF_CLIENT_CB));
#endif
#if (defined BTA_AG_INCLUDED && BTA_AG_INCLUDED == TRUE)
    if ((bta_ag_cb_ptr = (tBTA_AG_CB *)osi_malloc(sizeof(tBTA_AG_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_ag_cb_ptr, 0, sizeof(tBTA_AG_CB));
#endif
#if (defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE)
    if ((bta_jv_cb_ptr = (tBTA_JV_CB *)osi_malloc(sizeof(tBTA_JV_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_jv_cb_ptr, 0, sizeof(tBTA_JV_CB));
#endif //JV
#if BTA_HS_INCLUDED == TRUE
    memset((void *)bta_hs_cb_ptr, 0, sizeof(tBTA_HS_CB));
#endif
#if BTA_SDP_INCLUDED == TRUE
    if ((bta_sdp_cb_ptr = (tBTA_SDP_CB *)osi_malloc(sizeof(tBTA_SDP_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_sdp_cb_ptr, 0, sizeof(tBTA_SDP_CB));
#endif
#if SDP_INCLUDED == TRUE
    if ((g_disc_raw_data_buf = (UINT8 *)osi_malloc(MAX_DISC_RAW_DATA_BUF)) == NULL) {
        goto error_exit;
    }
    memset((void *)g_disc_raw_data_buf, 0, MAX_DISC_RAW_DATA_BUF);
#endif
#if BTA_AR_INCLUDED==TRUE
    if ((bta_ar_cb_ptr = (tBTA_AR_CB *)osi_malloc(sizeof(tBTA_AR_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_ar_cb_ptr, 0, sizeof(tBTA_AR_CB));
#endif
#if BTA_AV_INCLUDED==TRUE
    if ((bta_av_cb_ptr = (tBTA_AV_CB *)osi_malloc(sizeof(tBTA_AV_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_av_cb_ptr, 0, sizeof(tBTA_AV_CB));

    if ((bta_av_sbc_ups_cb_ptr = (tBTA_AV_SBC_UPS_CB *)osi_malloc(sizeof(tBTA_AV_SBC_UPS_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_av_sbc_ups_cb_ptr, 0, sizeof(tBTA_AV_SBC_UPS_CB));
#endif
#if BTA_HH_INCLUDED==TRUE
    if ((bta_hh_cb_ptr = (tBTA_HH_CB *)osi_malloc(sizeof(tBTA_HH_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_hh_cb_ptr, 0, sizeof(tBTA_HH_CB));
#endif
#if BTA_HD_INCLUDED==TRUE
    if ((bta_hd_cb_ptr = (tBTA_HD_CB *)osi_malloc(sizeof(tBTA_HD_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_hd_cb_ptr, 0, sizeof(tBTA_HD_CB));
#endif
#if BTA_HL_INCLUDED==TRUE
    memset((void *)bta_hl_cb_ptr, 0, sizeof(tBTA_HL_CB));
#endif
#if GATTC_INCLUDED==TRUE
    if ((bta_gattc_cb_ptr = (tBTA_GATTC_CB *)osi_malloc(sizeof(tBTA_GATTC_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_gattc_cb_ptr, 0, sizeof(tBTA_GATTC_CB));
#endif
#if GATTS_INCLUDED == TRUE
    if ((bta_gatts_cb_ptr = (tBTA_GATTS_CB *)osi_malloc(sizeof(tBTA_GATTS_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_gatts_cb_ptr, 0, sizeof(tBTA_GATTS_CB));
#endif
#if BTA_PAN_INCLUDED==TRUE
    memset((void *)bta_pan_cb_ptr, 0, sizeof(tBTA_PAN_CB));
#endif

#endif // BTA_INCLUDED == TRUE
    return BT_STATUS_SUCCESS;

error_exit:;
    LOG_ERROR("%s failed due to no memory", __func__);
    BTE_DeinitStack();
    return BT_STATUS_NOMEM;
}
