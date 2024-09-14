/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/osi.h"
#include "osi/allocator.h"
#include "common/bt_target.h"

#include "stack/obex_api.h"
#include "stack/goep_common.h"
#include "stack/goepc_api.h"
#include "goep_int.h"

#if (GOEPC_INCLUDED == TRUE)

/*******************************************************************************
**
** Function         GOEPC_Init
**
** Description      Initialize GOEP Client role, must call before using any
**                  other GOEPC APIs
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPC_Init(void)
{
#if (GOEP_DYNAMIC_MEMORY)
    if (!goepc_cb_ptr) {
        goepc_cb_ptr = (tGOEPC_CB *)osi_malloc(sizeof(tGOEPC_CB));
        if (!goepc_cb_ptr) {
            return GOEP_NO_RESOURCES;
        }
    }
#endif /* #if (GOEP_DYNAMIC_MEMORY) */
    memset(&goepc_cb, 0, sizeof(tGOEPC_CB));
    goepc_cb.trace_level = BT_TRACE_LEVEL_ERROR;
    return GOEP_SUCCESS;
}

/*******************************************************************************
**
** Function         GOEPC_Deinit
**
** Description      Deinit GOEP Client role, once deinit, can not use any other
**                  GOEPC APIs until call GOEPC_Init again
**
*******************************************************************************/
void GOEPC_Deinit(void)
{
#if (GOEP_DYNAMIC_MEMORY)
    if (goepc_cb_ptr) {
        osi_free(goepc_cb_ptr);
        goepc_cb_ptr = NULL;
    }
#endif /* #if (GOEP_DYNAMIC_MEMORY) */
}

/*******************************************************************************
**
** Function         GOEPC_Open
**
** Description      Start the progress to establish a GOEP connection to server
**
** Returns          GOEP_SUCCESS if successful, otherwise failed, when the
**                  connection is established, GOEPC_OPENED_EVT will come
**
*******************************************************************************/
UINT16 GOEPC_Open(tOBEX_SVR_INFO *svr, tGOEPC_EVT_CBACK callback, UINT16 *out_handle)
{
    UINT16 ret = GOEP_SUCCESS;
    tGOEPC_CCB *p_ccb = NULL;

    do {
        /* check parameter, allow out_handle to be NULL */
        if (svr == NULL || callback == NULL) {
            ret = GOEP_INVALID_PARAM;
            break;
        }

        p_ccb = goepc_allocate_ccb();
        if (p_ccb == NULL) {
            ret =  GOEP_NO_RESOURCES;
            break;
        }

        if (OBEX_CreateConn(svr, goepc_obex_callback, &p_ccb->obex_handle) != OBEX_SUCCESS) {
            ret = GOEP_TL_ERROR;
            break;
        }

        /* success */
        p_ccb->callback = callback;
        p_ccb->state = GOEPC_STATE_OPENING;
        if (out_handle) {
            *out_handle = p_ccb->allocated;
        }
    } while (0);

    if (ret != GOEP_SUCCESS && p_ccb != NULL) {
        goepc_free_ccb(p_ccb);
    }
    return ret;
}

/*******************************************************************************
**
** Function         GOEPC_Close
**
** Description      Close a GOEP connection immediately
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPC_Close(UINT16 handle)
{
    tGOEPC_CCB *p_ccb = NULL;

    UINT16 ccb_idx = handle - 1;
    if (ccb_idx >= GOEPC_MAX_CONNECTION || !goepc_cb.ccb[ccb_idx].allocated) {
        return GOEP_BAD_HANDLE;
    }

    p_ccb = &goepc_cb.ccb[ccb_idx];
    if (p_ccb->obex_handle) {
        OBEX_RemoveConn(p_ccb->obex_handle);
    }
    goepc_free_ccb(p_ccb);

    return GOEP_SUCCESS;
}

/*******************************************************************************
**
** Function         GOEPC_SendRequest
**
** Description      Send the prepared request packet to server
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPC_SendRequest(UINT16 handle)
{
    UINT16 ret = GOEP_SUCCESS;
    tGOEPC_CCB *p_ccb = NULL;
    BOOLEAN final = FALSE;

    do {
        UINT16 ccb_idx = handle - 1;
        if (ccb_idx >= GOEPC_MAX_CONNECTION || !goepc_cb.ccb[ccb_idx].allocated) {
            ret = GOEP_BAD_HANDLE;
            break;
        }
        p_ccb = &goepc_cb.ccb[ccb_idx];

        if (p_ccb->pkt == NULL) {
            ret = GOEP_INVALID_STATE;
            break;
        }

        final = OBEX_CheckFinalBit(p_ccb->pkt);
        /* check whether state machine allow this operation */
        if (!goepc_check_obex_req_allow(p_ccb->state, final)) {
            ret = GOEP_INVALID_STATE;
            break;
        }

        if (p_ccb->congest) {
            ret = GOEP_CONGEST;
            break;
        }

        /* execute srm state machine */
        goepc_srm_sm_execute(p_ccb, TRUE, p_ccb->pkt_srm_en, p_ccb->pkt_srm_wait);

        tGOEPC_DATA data;
        data.pkt = p_ccb->pkt;

        p_ccb->last_pkt_opcode = p_ccb->curr_pkt_opcode;
        p_ccb->pkt = NULL;
        p_ccb->pkt_srm_en = FALSE;
        p_ccb->pkt_srm_wait = FALSE;

        /* execute main state machine */
        if (final) {
            goepc_sm_execute(p_ccb, GOEPC_SM_EVENT_REQ_FB, &data);
        }
        else {
            goepc_sm_execute(p_ccb, GOEPC_SM_EVENT_REQ, &data);
        }
        /* since goepc_sm_execute may free ccb, can not access ccb here */
    } while (0);

    return ret;
}

/*******************************************************************************
**
** Function         GOEPC_PrepareRequest
**
** Description      Prepare a request packet, packet will be store internally
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPC_PrepareRequest(UINT16 handle, tOBEX_PARSE_INFO *info, UINT16 buff_size)
{
    UINT16 ret = GOEP_SUCCESS;
    tGOEPC_CCB *p_ccb = NULL;
    BT_HDR *pkt = NULL;

    do {
        UINT16 ccb_idx = handle - 1;
        if (ccb_idx >= GOEPC_MAX_CONNECTION || !goepc_cb.ccb[ccb_idx].allocated) {
            ret = GOEP_BAD_HANDLE;
            break;
        }
        p_ccb = &goepc_cb.ccb[ccb_idx];

        if (info == NULL || buff_size < OBEX_MIN_PACKET_SIZE) {
            ret = GOEP_INVALID_PARAM;
            break;
        }

        if (p_ccb->pkt != NULL) {
            ret = GOEP_INVALID_STATE;
            break;
        }

        if (!goepc_check_obex_req_param(info)) {
            ret = GOEP_INVALID_PARAM;
            break;
        }

        if (OBEX_BuildRequest(info, buff_size, &pkt) != OBEX_SUCCESS) {
            ret = GOEP_NO_RESOURCES;
            break;
        }

        p_ccb->curr_pkt_opcode = info->opcode;
        p_ccb->pkt = pkt;
    } while (0);

    return ret;
}

/*******************************************************************************
**
** Function         GOEPC_DropRequest
**
** Description      Drop the prepared internal request packet
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPC_DropRequest(UINT16 handle)
{
    UINT16 ccb_idx = handle - 1;
    if (ccb_idx >= GOEPC_MAX_CONNECTION || !goepc_cb.ccb[ccb_idx].allocated) {
        return GOEP_BAD_HANDLE;
    }

    tGOEPC_CCB *p_ccb = &goepc_cb.ccb[ccb_idx];
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
** Function         GOEPC_RequestSetSRM
**
** Description      Modify the prepared internal request packet, append SRM header
**                  or SRMP header
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPC_RequestSetSRM(UINT16 handle, BOOLEAN srm_en, BOOLEAN srm_wait)
{
    UINT16 ret = GOEP_SUCCESS;
    tGOEPC_CCB *p_ccb = NULL;

    do {
        UINT16 ccb_idx = handle - 1;
        if (ccb_idx >= GOEPC_MAX_CONNECTION || !goepc_cb.ccb[ccb_idx].allocated) {
            ret = GOEP_BAD_HANDLE;
            break;
        }
        p_ccb = &goepc_cb.ccb[ccb_idx];

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
            }
            else {
                ret = GOEP_NO_RESOURCES;
                break;
            }
        }
        if (srm_wait) {
            if (OBEX_AppendHeaderSRMP(p_ccb->pkt, OBEX_SRMP_WAIT) == OBEX_SUCCESS) {
                p_ccb->pkt_srm_wait = TRUE;
            }
            else {
                ret = GOEP_NO_RESOURCES;
                break;
            }
        }
    } while (0);

    return ret;
}

/*******************************************************************************
**
** Function         GOEPC_RequestAddHeader
**
** Description      Modify the prepared internal request packet, append header
**
** Returns          GOEP_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 GOEPC_RequestAddHeader(UINT16 handle, UINT8 header_id, const UINT8 *data, UINT16 data_len)
{
    UINT16 ret = GOEP_SUCCESS;
    tGOEPC_CCB *p_ccb = NULL;

    do {
        UINT16 ccb_idx = handle - 1;
        if (ccb_idx >= GOEPC_MAX_CONNECTION || !goepc_cb.ccb[ccb_idx].allocated) {
            ret = GOEP_BAD_HANDLE;
            break;
        }
        p_ccb = &goepc_cb.ccb[ccb_idx];

        if (p_ccb->pkt == NULL) {
            ret = GOEP_INVALID_STATE;
            break;
        }

        if (data == NULL || data_len == 0) {
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

#endif /* #if (GOEPC_INCLUDED == TRUE) */
