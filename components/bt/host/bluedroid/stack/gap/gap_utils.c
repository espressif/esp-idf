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

#include <string.h>
#include "common/bt_target.h"
//#include "bt_utils.h"
#include "gap_int.h"

#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         gap_allocate_cb
**
** Description      Look through the GAP Control Blocks for a free one.
**
** Returns          Pointer to the control block or NULL if not found
**
*******************************************************************************/
tGAP_INFO *gap_allocate_cb (void)
{
    tGAP_INFO     *p_cb = &gap_cb.blk[0];
    UINT8        x;

    for (x = 0; x < GAP_MAX_BLOCKS; x++, p_cb++) {
        if (!p_cb->in_use) {
            memset (p_cb, 0, sizeof (tGAP_INFO));

            p_cb->in_use = TRUE;
            p_cb->index = x;
            p_cb->p_data = (void *)NULL;
            return (p_cb);
        }
    }

    /* If here, no free control blocks found */
    return (NULL);
}


/*******************************************************************************
**
** Function         gap_free_cb
**
** Description      Release GAP control block.
**
** Returns          Pointer to the control block or NULL if not found
**
*******************************************************************************/
void gap_free_cb (tGAP_INFO *p_cb)
{
    if (p_cb) {
        p_cb->gap_cback = NULL;
        p_cb->in_use = FALSE;
    }
}


/*******************************************************************************
**
** Function         gap_is_service_busy
**
** Description      Look through the GAP Control Blocks that are in use
**                  and check to see if the event waiting for is the command
**                  requested.
**
** Returns          TRUE if already in use
**                  FALSE if not busy
**
*******************************************************************************/
BOOLEAN gap_is_service_busy (UINT16 request)
{
    tGAP_INFO   *p_cb = &gap_cb.blk[0];
    UINT8        x;

    for (x = 0; x < GAP_MAX_BLOCKS; x++, p_cb++) {
        if (p_cb->in_use && p_cb->event == request) {
            return (TRUE);
        }
    }

    /* If here, service is not busy */
    return (FALSE);
}


/*******************************************************************************
**
** Function         gap_convert_btm_status
**
** Description      Converts a BTM error status into a GAP error status
**
**
** Returns          GAP_UNKNOWN_BTM_STATUS is returned if not recognized
**
*******************************************************************************/
UINT16 gap_convert_btm_status (tBTM_STATUS btm_status)
{
    switch (btm_status) {
    case BTM_SUCCESS:
        return (BT_PASS);

    case BTM_CMD_STARTED:
        return (GAP_CMD_INITIATED);

    case BTM_BUSY:
        return (GAP_ERR_BUSY);

    case BTM_MODE_UNSUPPORTED:
    case BTM_ILLEGAL_VALUE:
        return (GAP_ERR_ILL_PARM);

    case BTM_WRONG_MODE:
        return (GAP_DEVICE_NOT_UP);

    case BTM_UNKNOWN_ADDR:
        return (GAP_BAD_BD_ADDR);

    case BTM_DEVICE_TIMEOUT:
        return (GAP_ERR_TIMEOUT);

    default:
        return (GAP_ERR_PROCESSING);
    }
}

#endif  ///CLASSIC_BT_INCLUDED == TRUE
