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
 *  Filename:      btc_sm.h
 *
 *  Description:   Generic BTC state machine API
 *
 *****************************************************************************/

#ifndef __BTC_SM_H__
#define __BTC_SM_H__

/*****************************************************************************
**  Constants & Macros
******************************************************************************/

/* Generic Enter/Exit state machine events */
#define BTC_SM_ENTER_EVT 0xFFFF
#define BTC_SM_EXIT_EVT  0xFFFE


/*****************************************************************************
**  Type definitions and return values
******************************************************************************/
typedef UINT32 btc_sm_state_t;
typedef UINT32 btc_sm_event_t;
typedef void *btc_sm_handle_t;
typedef BOOLEAN (* btc_sm_handler_t)(btc_sm_event_t event, void *data);


/*****************************************************************************
**  Functions
**
**  NOTE: THESE APIs SHOULD BE INVOKED ONLY IN THE BTC CONTEXT
**
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
btc_sm_handle_t btc_sm_init(const btc_sm_handler_t *p_handlers,
                            btc_sm_state_t initial_state);

/*****************************************************************************
**
** Function     btc_sm_shutdown
**
** Description  Tears down the state machine
**
** Returns      None
**
******************************************************************************/
void btc_sm_shutdown(btc_sm_handle_t handle);

/*****************************************************************************
**
** Function     btc_sm_get_state
**
** Description  Fetches the current state of the state machine
**
** Returns      Current state
**
******************************************************************************/
btc_sm_state_t btc_sm_get_state(btc_sm_handle_t handle);

/*****************************************************************************
**
** Function     btc_sm_dispatch
**
** Description  Dispatches the 'event' along with 'data' to the current state handler
**
** Returns      Returns BT_STATUS_OK on success, BT_STATUS_FAIL otherwise
**
******************************************************************************/
bt_status_t btc_sm_dispatch(btc_sm_handle_t handle, btc_sm_event_t event,
                            void *data);

/*****************************************************************************
**
** Function     btc_sm_change_state
**
** Description  Make a transition to the new 'state'. The 'BTC_SM_EXIT_EVT'
**              shall be invoked before exiting the current state. The
**              'BTC_SM_ENTER_EVT' shall be invoked before entering the new state
**
**
** Returns      Returns BT_STATUS_OK on success, BT_STATUS_FAIL otherwise
**
******************************************************************************/
bt_status_t btc_sm_change_state(btc_sm_handle_t handle, btc_sm_state_t state);

#endif /* __BTC_SM_H__ */
