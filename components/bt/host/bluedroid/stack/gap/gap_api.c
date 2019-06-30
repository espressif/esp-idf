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
#include "osi/allocator.h"

#if GAP_DYNAMIC_MEMORY == FALSE
tGAP_CB  gap_cb;
#else
tGAP_CB  *gap_cb_ptr;
#endif

/*******************************************************************************
**
** Function         GAP_SetTraceLevel
**
** Description      This function sets the trace level for GAP.  If called with
**                  a value of 0xFF, it simply returns the current trace level.
**
** Returns          The new or current trace level
**
*******************************************************************************/
UINT8 GAP_SetTraceLevel (UINT8 new_level)
{
    if (new_level != 0xFF) {
        gap_cb.trace_level = new_level;
    }

    return (gap_cb.trace_level);
}

/*******************************************************************************
**
** Function         GAP_Init
**
** Description      Initializes the control blocks used by GAP.
**
**                  This routine should not be called except once per
**                      stack invocation.
**
** Returns          status
**
*******************************************************************************/
bt_status_t GAP_Init(void)
{
#if GAP_DYNAMIC_MEMORY == TRUE
    gap_cb_ptr = (tGAP_CB *)osi_malloc(sizeof(tGAP_CB));
    if (!gap_cb_ptr) {
        return BT_STATUS_NOMEM;
    }
#endif

    memset (&gap_cb, 0, sizeof (tGAP_CB));

#if defined(GAP_INITIAL_TRACE_LEVEL)
    gap_cb.trace_level = GAP_INITIAL_TRACE_LEVEL;
#else
    gap_cb.trace_level = BT_TRACE_LEVEL_NONE;    /* No traces */
#endif

#if GAP_CONN_INCLUDED == TRUE
    gap_conn_init();
#endif

#if BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE
    gap_attr_db_init();
#endif

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         GAP_Deinit
**
** Description      This function is called to deinitialize the control block
**                  for this layer.
**
** Returns          void
**
*******************************************************************************/
void GAP_Deinit(void)
{
#if GAP_DYNAMIC_MEMORY == TRUE
    if (gap_cb_ptr) {
        osi_free(gap_cb_ptr);
        gap_cb_ptr = NULL;
    }
#endif
}