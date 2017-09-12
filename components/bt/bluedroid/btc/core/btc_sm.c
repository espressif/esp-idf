// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


/*****************************************************************************
 *
 *  Filename:      btc_sm.c
 *
 *  Description:   Generic BTC state machine API
 *
 *****************************************************************************/
#include "bt_target.h"
#include "bt_defs.h"
#include "allocator.h"
#include "btc_common.h"
#include "btc_sm.h"

#if BTC_SM_INCLUDED
/*****************************************************************************
**  Constants & Macros
******************************************************************************/
/*****************************************************************************
**  Local type definitions
******************************************************************************/
typedef struct {
    btc_sm_state_t         state;
    btc_sm_handler_t       *p_handlers;
} btc_sm_cb_t;

/*****************************************************************************
**  Static variables
******************************************************************************/

/*****************************************************************************
**  Static functions
******************************************************************************/

/*****************************************************************************
**  Externs
******************************************************************************/

/*****************************************************************************
**  Functions
******************************************************************************/

/*****************************************************************************
**
** Function     btc_sm_init
**
** Description  Initializes the state machine with the state handlers
**              The caller should ensure that the table and the corresponding
**              states match. The location that 'p_handlers' points to shall
**              be available until the btc_sm_shutdown API is invoked.
**
** Returns      Returns a pointer to the initialized state machine handle.
**
******************************************************************************/

btc_sm_handle_t btc_sm_init(const btc_sm_handler_t *p_handlers, btc_sm_state_t initial_state)
{
    btc_sm_cb_t *p_cb;

    if (p_handlers == NULL) {
        LOG_ERROR("%s : p_handlers is NULL", __FUNCTION__);
        return NULL;
    }

    p_cb = (btc_sm_cb_t *)osi_malloc(sizeof(btc_sm_cb_t));
    p_cb->state = initial_state;
    p_cb->p_handlers = (btc_sm_handler_t *)p_handlers;

    /* Send BTC_SM_ENTER_EVT to the initial state */
    p_cb->p_handlers[initial_state](BTC_SM_ENTER_EVT, NULL);

    return (btc_sm_handle_t)p_cb;
}

/*****************************************************************************
**
** Function     btc_sm_shutdown
**
** Description  Tears down the state machine
**
** Returns      None
**
******************************************************************************/
void btc_sm_shutdown(btc_sm_handle_t handle)
{
    btc_sm_cb_t *p_cb = (btc_sm_cb_t *)handle;

    if (p_cb == NULL) {
        LOG_ERROR("%s : Invalid handle", __FUNCTION__);
        return;
    }
    osi_free(p_cb);
}

/*****************************************************************************
**
** Function     btc_sm_get_state
**
** Description  Fetches the current state of the state machine
**
** Returns      Current state
**
******************************************************************************/
btc_sm_state_t btc_sm_get_state(btc_sm_handle_t handle)
{
    btc_sm_cb_t *p_cb = (btc_sm_cb_t *)handle;

    if (p_cb == NULL) {
        LOG_ERROR("%s : Invalid handle", __FUNCTION__);
        return 0;
    }

    return p_cb->state;
}

/*****************************************************************************
**
** Function     btc_sm_dispatch
**
** Description  Dispatches the 'event' along with 'data' to the current state handler
**
** Returns      BT_STATUS_SUCCESS on success
**              BT_STATUS_UNHANDLED if event was not processed
**              BT_STATUS_FAIL otherwise
**
******************************************************************************/
bt_status_t btc_sm_dispatch(btc_sm_handle_t handle, btc_sm_event_t event,
                            void *data)
{
    bt_status_t status = BT_STATUS_SUCCESS;

    btc_sm_cb_t *p_cb = (btc_sm_cb_t *)handle;

    if (p_cb == NULL) {
        LOG_ERROR("%s : Invalid handle", __FUNCTION__);
        return BT_STATUS_FAIL;
    }

    if (p_cb->p_handlers[p_cb->state](event, data) == FALSE) {
        return BT_STATUS_UNHANDLED;
    }

    return status;
}

/*****************************************************************************
**
** Function     btc_sm_change_state
**
** Description  Make a transition to the new 'state'. The 'BTC_SM_EXIT_EVT'
**              shall be invoked before exiting the current state. The
**              'BTC_SM_ENTER_EVT' shall be invoked before entering the new state
**
** Returns      BT_STATUS_SUCCESS on success
**              BT_STATUS_UNHANDLED if event was not processed
**              BT_STATUS_FAIL otherwise
**
******************************************************************************/
bt_status_t btc_sm_change_state(btc_sm_handle_t handle, btc_sm_state_t state)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    btc_sm_cb_t *p_cb = (btc_sm_cb_t *)handle;

    if (p_cb == NULL) {
        LOG_ERROR("%s : Invalid handle", __FUNCTION__);
        return BT_STATUS_FAIL;
    }

    /* Send exit event to the current state */
    if (p_cb->p_handlers[p_cb->state](BTC_SM_EXIT_EVT, NULL) == FALSE) {
        status = BT_STATUS_UNHANDLED;
    }

    /* Change to the new state */
    p_cb->state = state;

    /* Send enter event to the new state */
    if (p_cb->p_handlers[p_cb->state](BTC_SM_ENTER_EVT, NULL) == FALSE) {
        status = BT_STATUS_UNHANDLED;
    }

    return status;
}

#endif /* #if BTC_SM_INCLUDED */
