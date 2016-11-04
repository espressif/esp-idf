/******************************************************************************
 *
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

/*****************************************************************************
 *
 *  Filename:      btif_sm.h
 *
 *  Description:   Generic BTIF state machine API
 *
 *****************************************************************************/

#ifndef BTIF_SM_H
#define BTIF_SM_H

/*****************************************************************************
**  Constants & Macros
******************************************************************************/

/* Generic Enter/Exit state machine events */
#define BTIF_SM_ENTER_EVT 0xFFFF
#define BTIF_SM_EXIT_EVT  0xFFFE


/*****************************************************************************
**  Type definitions and return values
******************************************************************************/
typedef UINT32 btif_sm_state_t;
typedef UINT32 btif_sm_event_t;
typedef void* btif_sm_handle_t;
typedef BOOLEAN(*btif_sm_handler_t)(btif_sm_event_t event, void *data);


/*****************************************************************************
**  Functions
**
**  NOTE: THESE APIs SHOULD BE INVOKED ONLY IN THE BTIF CONTEXT
**
******************************************************************************/

/*****************************************************************************
**
** Function     btif_sm_init
**
** Description  Initializes the state machine with the state handlers
**              The caller should ensure that the table and the corresponding
**              states match. The location that 'p_handlers' points to shall
**              be available until the btif_sm_shutdown API is invoked.
**
** Returns      Returns a pointer to the initialized state machine handle.
**
******************************************************************************/
btif_sm_handle_t btif_sm_init(const btif_sm_handler_t *p_handlers,
                               btif_sm_state_t initial_state);

/*****************************************************************************
**
** Function     btif_sm_shutdown
**
** Description  Tears down the state machine
**
** Returns      None
**
******************************************************************************/
void btif_sm_shutdown(btif_sm_handle_t handle);

/*****************************************************************************
**
** Function     btif_sm_get_state
**
** Description  Fetches the current state of the state machine
**
** Returns      Current state
**
******************************************************************************/
btif_sm_state_t btif_sm_get_state(btif_sm_handle_t handle);

/*****************************************************************************
**
** Function     btif_sm_dispatch
**
** Description  Dispatches the 'event' along with 'data' to the current state handler
**
** Returns      Returns BT_STATUS_OK on success, BT_STATUS_FAIL otherwise
**
******************************************************************************/
bt_status_t btif_sm_dispatch(btif_sm_handle_t handle, btif_sm_event_t event,
                                void *data);

/*****************************************************************************
**
** Function     btif_sm_change_state
**
** Description  Make a transition to the new 'state'. The 'BTIF_SM_EXIT_EVT'
**              shall be invoked before exiting the current state. The
**              'BTIF_SM_ENTER_EVT' shall be invoked before entering the new state
**
**
** Returns      Returns BT_STATUS_OK on success, BT_STATUS_FAIL otherwise
**
******************************************************************************/
bt_status_t btif_sm_change_state(btif_sm_handle_t handle, btif_sm_state_t state);

#endif /* BTIF_SM_H */
