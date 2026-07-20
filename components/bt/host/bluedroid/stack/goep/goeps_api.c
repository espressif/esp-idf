/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/osi.h"
#include "osi/allocator.h"
#include "common/bt_target.h"

#include "stack/obex_api.h"
#include "stack/goep_common.h"
#include "stack/goeps_api.h"
#include "goeps_int.h"

#if (GOEPS_INCLUDED == TRUE)

static void goeps_close_ccbs_on_scb(UINT8 scb_idx)
{
    for (int i = 0; i < GOEPS_MAX_CONNECTION; ++i) {
        tGOEPS_CCB *p_ccb = &goeps_cb.ccb[i];
        if (p_ccb->allocated && p_ccb->scb_idx == scb_idx) {
            if (p_ccb->obex_handle) {
                OBEX_RemoveConn(p_ccb->obex_handle);
            }
            goeps_free_ccb(p_ccb);
        }
    }
}

/*******************************************************************************
**
** Function         GOEPS_Init
**
** Description      Initialize GOEP Server role
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPS_Init(void)
{
#if (GOEP_DYNAMIC_MEMORY)
    if (!goeps_cb_ptr) {
        goeps_cb_ptr = (tGOEPS_CB *)osi_malloc(sizeof(tGOEPS_CB));
        if (!goeps_cb_ptr) {
            return GOEP_NO_RESOURCES;
        }
    }
#endif /* #if (GOEP_DYNAMIC_MEMORY) */
    memset(&goeps_cb, 0, sizeof(tGOEPS_CB));
#if defined(GOEPS_INITIAL_TRACE_LEVEL)
    goeps_cb.trace_level = GOEPS_INITIAL_TRACE_LEVEL;
#else
    goeps_cb.trace_level = BT_TRACE_LEVEL_NONE;
#endif
    return GOEP_SUCCESS;
}

/*******************************************************************************
**
** Function         GOEPS_Deinit
**
** Description      Deinit GOEP Server role
**
*******************************************************************************/
void GOEPS_Deinit(void)
{
#if (GOEP_DYNAMIC_MEMORY)
    if (!goeps_cb_ptr) {
        return;
    }
#endif /* #if (GOEP_DYNAMIC_MEMORY) */
    for (int i = 0; i < GOEPS_MAX_SERVER; ++i) {
        if (goeps_cb.scb[i].allocated) {
            goeps_close_ccbs_on_scb(goeps_cb.scb[i].allocated);
            OBEX_DeregisterServer(goeps_cb.scb[i].obex_svr_handle);
            goeps_free_scb(&goeps_cb.scb[i]);
        }
    }
#if (GOEP_DYNAMIC_MEMORY)
    osi_free(goeps_cb_ptr);
    goeps_cb_ptr = NULL;
#endif /* #if (GOEP_DYNAMIC_MEMORY) */
}

/*******************************************************************************
**
** Function         GOEPS_Register
**
** Description      Register a GOEP server and listen for incoming connections
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPS_Register(tOBEX_SVR_INFO *p_listen, tGOEPS_EVT_CBACK callback, UINT16 *out_svr_handle)
{
    UINT16 ret = GOEP_SUCCESS;
    tGOEPS_SCB *p_scb = NULL;

    do {
        if (p_listen == NULL || callback == NULL) {
            ret = GOEP_INVALID_PARAM;
            break;
        }

        p_scb = goeps_allocate_scb();
        if (p_scb == NULL) {
            ret = GOEP_NO_RESOURCES;
            break;
        }

        if (OBEX_RegisterServer(p_listen, goeps_obex_callback, &p_scb->obex_svr_handle) != OBEX_SUCCESS) {
            ret = GOEP_TL_ERROR;
            break;
        }

        p_scb->callback = callback;
        if (out_svr_handle) {
            *out_svr_handle = p_scb->obex_svr_handle;
        }
    } while (0);

    if (ret != GOEP_SUCCESS && p_scb != NULL) {
        goeps_free_scb(p_scb);
    }
    return ret;
}

/*******************************************************************************
**
** Function         GOEPS_Deregister
**
** Description      Deregister a GOEP server
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPS_Deregister(UINT16 svr_handle)
{
    UINT16 scb_idx = (svr_handle >> 8) - 1;
    if (scb_idx >= GOEPS_MAX_SERVER || !goeps_cb.scb[scb_idx].allocated) {
        return GOEP_BAD_HANDLE;
    }

    tGOEPS_SCB *p_scb = &goeps_cb.scb[scb_idx];
    goeps_close_ccbs_on_scb(p_scb->allocated);
    OBEX_DeregisterServer(p_scb->obex_svr_handle);
    goeps_free_scb(p_scb);
    return GOEP_SUCCESS;
}

/*******************************************************************************
**
** Function         GOEPS_CloseConn
**
** Description      Close a GOEP server connection immediately
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPS_CloseConn(UINT16 conn_handle)
{
    UINT16 ccb_idx = conn_handle - 1;
    if (ccb_idx >= GOEPS_MAX_CONNECTION || !goeps_cb.ccb[ccb_idx].allocated) {
        return GOEP_BAD_HANDLE;
    }

    tGOEPS_CCB *p_ccb = &goeps_cb.ccb[ccb_idx];
    if (p_ccb->obex_handle) {
        OBEX_RemoveConn(p_ccb->obex_handle);
    }
    goeps_free_ccb(p_ccb);
    return GOEP_SUCCESS;
}

/*******************************************************************************
**
** Function         GOEPS_SendResponse
**
** Description      Send the prepared response packet to client
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPS_SendResponse(UINT16 conn_handle)
{
    UINT16 ret = GOEP_SUCCESS;
    tGOEPS_CCB *p_ccb = NULL;
    BOOLEAN final = FALSE;

    do {
        UINT16 ccb_idx = conn_handle - 1;
        if (ccb_idx >= GOEPS_MAX_CONNECTION || !goeps_cb.ccb[ccb_idx].allocated) {
            ret = GOEP_BAD_HANDLE;
            break;
        }
        p_ccb = &goeps_cb.ccb[ccb_idx];

        if (p_ccb->pkt == NULL) {
            ret = GOEP_INVALID_STATE;
            break;
        }

        final = OBEX_CheckFinalBit(p_ccb->pkt);
        if (!goeps_check_obex_rsp_allow(p_ccb->state, final)) {
            ret = GOEP_INVALID_STATE;
            break;
        }

        if (p_ccb->congest) {
            ret = GOEP_CONGEST;
            break;
        }

        goeps_srm_sm_execute(p_ccb, TRUE, p_ccb->pkt_srm_en, p_ccb->pkt_srm_wait);

        tGOEPS_DATA data;
        data.pkt = p_ccb->pkt;

        p_ccb->pkt = NULL;
        p_ccb->pkt_srm_en = FALSE;
        p_ccb->pkt_srm_wait = FALSE;

        if (final) {
            goeps_sm_execute(p_ccb, GOEPS_SM_EVENT_RSP_FB, &data);
        } else {
            goeps_sm_execute(p_ccb, GOEPS_SM_EVENT_RSP, &data);
        }
    } while (0);

    return ret;
}

/*******************************************************************************
**
** Function         GOEPS_PrepareResponse
**
** Description      Prepare a response packet, packet will be stored internally
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPS_PrepareResponse(UINT16 conn_handle, tOBEX_PARSE_INFO *info, UINT16 buff_size)
{
    UINT16 ret = GOEP_SUCCESS;
    tGOEPS_CCB *p_ccb = NULL;
    BT_HDR *pkt = NULL;

    do {
        UINT16 ccb_idx = conn_handle - 1;
        if (ccb_idx >= GOEPS_MAX_CONNECTION || !goeps_cb.ccb[ccb_idx].allocated) {
            ret = GOEP_BAD_HANDLE;
            break;
        }
        p_ccb = &goeps_cb.ccb[ccb_idx];

        if (info == NULL || buff_size < OBEX_MIN_PACKET_SIZE) {
            ret = GOEP_INVALID_PARAM;
            break;
        }

        if (p_ccb->pkt != NULL) {
            ret = GOEP_INVALID_STATE;
            break;
        }

        if (!goeps_check_obex_rsp_param(info)) {
            ret = GOEP_INVALID_PARAM;
            break;
        }

        if (OBEX_BuildResponse(info, buff_size, &pkt) != OBEX_SUCCESS) {
            ret = GOEP_NO_RESOURCES;
            break;
        }

        p_ccb->curr_rsp_opcode = info->opcode;
        p_ccb->pkt = pkt;
    } while (0);

    return ret;
}

/*******************************************************************************
**
** Function         GOEPS_DropResponse
**
** Description      Drop the prepared internal response packet
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPS_DropResponse(UINT16 conn_handle)
{
    UINT16 ccb_idx = conn_handle - 1;
    if (ccb_idx >= GOEPS_MAX_CONNECTION || !goeps_cb.ccb[ccb_idx].allocated) {
        return GOEP_BAD_HANDLE;
    }

    tGOEPS_CCB *p_ccb = &goeps_cb.ccb[ccb_idx];
    if (p_ccb->pkt == NULL) {
        return GOEP_INVALID_STATE;
    }

    osi_free(p_ccb->pkt);
    p_ccb->pkt = NULL;
    p_ccb->pkt_srm_en = FALSE;
    p_ccb->pkt_srm_wait = FALSE;
    return GOEP_SUCCESS;
}

/*******************************************************************************
**
** Function         GOEPS_ResponseSetSRM
**
** Description      Append SRM or SRMP header to prepared response packet
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPS_ResponseSetSRM(UINT16 conn_handle, BOOLEAN srm_en, BOOLEAN srm_wait)
{
    UINT16 ret = GOEP_SUCCESS;
    tGOEPS_CCB *p_ccb = NULL;

    do {
        UINT16 ccb_idx = conn_handle - 1;
        if (ccb_idx >= GOEPS_MAX_CONNECTION || !goeps_cb.ccb[ccb_idx].allocated) {
            ret = GOEP_BAD_HANDLE;
            break;
        }
        p_ccb = &goeps_cb.ccb[ccb_idx];

        if (!srm_en && !srm_wait) {
            ret = GOEP_INVALID_PARAM;
            break;
        }

        if (p_ccb->pkt == NULL) {
            ret = GOEP_INVALID_STATE;
            break;
        }

        if (srm_en) {
            if (OBEX_AppendHeaderSRM(p_ccb->pkt, OBEX_SRM_ENABLE) == OBEX_SUCCESS) {
                p_ccb->pkt_srm_en = TRUE;
            } else {
                ret = GOEP_NO_RESOURCES;
                break;
            }
        }
        if (srm_wait) {
            if (OBEX_AppendHeaderSRMP(p_ccb->pkt, OBEX_SRMP_WAIT) == OBEX_SUCCESS) {
                p_ccb->pkt_srm_wait = TRUE;
            } else {
                ret = GOEP_NO_RESOURCES;
                break;
            }
        }
    } while (0);

    return ret;
}

/*******************************************************************************
**
** Function         GOEPS_ResponseAddHeader
**
** Description      Append header to prepared response packet
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPS_ResponseAddHeader(UINT16 conn_handle, UINT8 header_id, const UINT8 *data, UINT16 data_len)
{
    UINT16 ret = GOEP_SUCCESS;
    tGOEPS_CCB *p_ccb = NULL;

    do {
        UINT16 ccb_idx = conn_handle - 1;
        if (ccb_idx >= GOEPS_MAX_CONNECTION || !goeps_cb.ccb[ccb_idx].allocated) {
            ret = GOEP_BAD_HANDLE;
            break;
        }
        p_ccb = &goeps_cb.ccb[ccb_idx];

        if (p_ccb->pkt == NULL) {
            ret = GOEP_INVALID_STATE;
            break;
        }

        if ((data == NULL && data_len != 0) || (data != NULL && data_len == 0)) {
            ret = GOEP_INVALID_PARAM;
            break;
        }

        if (OBEX_AppendHeaderRaw(p_ccb->pkt, header_id, data, data_len) != OBEX_SUCCESS) {
            ret = GOEP_NO_RESOURCES;
            break;
        }
    } while (0);

    return ret;
}

#endif /* #if (GOEPS_INCLUDED == TRUE) */
