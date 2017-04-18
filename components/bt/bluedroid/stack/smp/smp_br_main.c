/******************************************************************************
 *
 *  Copyright (C) 2014-2015 Broadcom Corporation
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

#include "bt_target.h"

#include <string.h>
#include "smp_int.h"

#if (BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE)

const char *const smp_br_state_name [SMP_BR_STATE_MAX + 1] = {
    "SMP_BR_STATE_IDLE",
    "SMP_BR_STATE_WAIT_APP_RSP",
    "SMP_BR_STATE_PAIR_REQ_RSP",
    "SMP_BR_STATE_BOND_PENDING",
    "SMP_BR_STATE_OUT_OF_RANGE"
};

const char *const smp_br_event_name [SMP_BR_MAX_EVT] = {
    "BR_PAIRING_REQ_EVT",
    "BR_PAIRING_RSP_EVT",
    "BR_CONFIRM_EVT",
    "BR_RAND_EVT",
    "BR_PAIRING_FAILED_EVT",
    "BR_ENCRPTION_INFO_EVT",
    "BR_MASTER_ID_EVT",
    "BR_ID_INFO_EVT",
    "BR_ID_ADDR_EVT",
    "BR_SIGN_INFO_EVT",
    "BR_SECURITY_REQ_EVT",
    "BR_PAIR_PUBLIC_KEY_EVT",
    "BR_PAIR_DHKEY_CHCK_EVT",
    "BR_PAIR_KEYPR_NOTIF_EVT",
    "BR_KEY_READY_EVT",
    "BR_ENCRYPTED_EVT",
    "BR_L2CAP_CONN_EVT",
    "BR_L2CAP_DISCONN_EVT",
    "BR_KEYS_RSP_EVT",
    "BR_API_SEC_GRANT_EVT",
    "BR_TK_REQ_EVT",
    "BR_AUTH_CMPL_EVT",
    "BR_ENC_REQ_EVT",
    "BR_BOND_REQ_EVT",
    "BR_DISCARD_SEC_REQ_EVT",
    "BR_OUT_OF_RANGE_EVT"
};

const char *smp_get_br_event_name(tSMP_BR_EVENT event);
const char *smp_get_br_state_name(tSMP_BR_STATE state);

#define SMP_BR_SM_IGNORE       0
#define SMP_BR_NUM_ACTIONS     2
#define SMP_BR_SME_NEXT_STATE  2
#define SMP_BR_SM_NUM_COLS     3
typedef const UINT8 (*tSMP_BR_SM_TBL)[SMP_BR_SM_NUM_COLS];

enum {
    SMP_SEND_PAIR_REQ,
    SMP_BR_SEND_PAIR_RSP,
    SMP_SEND_PAIR_FAIL,
    SMP_SEND_ID_INFO,
    SMP_BR_PROC_PAIR_CMD,
    SMP_PROC_PAIR_FAIL,
    SMP_PROC_ID_INFO,
    SMP_PROC_ID_ADDR,
    SMP_PROC_SRK_INFO,
    SMP_BR_PROC_SEC_GRANT,
    SMP_BR_PROC_SL_KEYS_RSP,
    SMP_BR_KEY_DISTRIBUTION,
    SMP_BR_PAIRING_COMPLETE,
    SMP_SEND_APP_CBACK,
    SMP_BR_CHECK_AUTH_REQ,
    SMP_PAIR_TERMINATE,
    SMP_IDLE_TERMINATE,
    SMP_BR_SM_NO_ACTION
};

static const tSMP_ACT smp_br_sm_action[] = {
    smp_send_pair_req,
    smp_br_send_pair_response,
    smp_send_pair_fail,
    smp_send_id_info,
    smp_br_process_pairing_command,
    smp_proc_pair_fail,
    smp_proc_id_info,
    smp_proc_id_addr,
    smp_proc_srk_info,
    smp_br_process_security_grant,
    smp_br_process_slave_keys_response,
    smp_br_select_next_key,
    smp_br_pairing_complete,
    smp_send_app_cback,
    smp_br_check_authorization_request,
    smp_pair_terminate,
    smp_idle_terminate
};

static const UINT8 smp_br_all_table[][SMP_BR_SM_NUM_COLS] = {
    /*                               Event                    Action           Next State */
    /* BR_PAIRING_FAILED        */  {SMP_PROC_PAIR_FAIL,  SMP_BR_PAIRING_COMPLETE, SMP_BR_STATE_IDLE},
    /* BR_AUTH_CMPL             */  {SMP_SEND_PAIR_FAIL,  SMP_BR_PAIRING_COMPLETE, SMP_BR_STATE_IDLE},
    /* BR_L2CAP_DISCONN         */  {SMP_PAIR_TERMINATE,  SMP_BR_SM_NO_ACTION, SMP_BR_STATE_IDLE}
};

/************ SMP Master FSM State/Event Indirection Table **************/
static const UINT8 smp_br_master_entry_map[][SMP_BR_STATE_MAX] = {
    /* br_state name:               Idle      WaitApp  Pair    Bond
                                              Rsp      ReqRsp  Pend       */
    /* BR_PAIRING_REQ           */  { 0,       0,       0,      0     },
    /* BR_PAIRING_RSP           */  { 0,       0,       1,      0     },
    /* BR_CONFIRM               */  { 0,       0,       0,      0     },
    /* BR_RAND                  */  { 0,       0,       0,      0     },
    /* BR_PAIRING_FAILED        */  { 0,       0x81,    0x81,   0     },
    /* BR_ENCRPTION_INFO        */  { 0,       0,       0,      0     },
    /* BR_MASTER_ID             */  { 0,       0,       0,      0     },
    /* BR_ID_INFO               */  { 0,       0,       0,      1     },
    /* BR_ID_ADDR               */  { 0,       0,       0,      2     },
    /* BR_SIGN_INFO             */  { 0,       0,       0,      3     },
    /* BR_SECURITY_REQ          */  { 0,       0,       0,      0     },
    /* BR_PAIR_PUBLIC_KEY_EVT   */  { 0,       0,       0,      0     },
    /* BR_PAIR_DHKEY_CHCK_EVT   */  { 0,       0,       0,      0     },
    /* BR_PAIR_KEYPR_NOTIF_EVT  */  { 0,       0,       0,      0     },
    /* BR_KEY_READY             */  { 0,       0,       0,      0     },
    /* BR_ENCRYPTED             */  { 0,       0,       0,      0     },
    /* BR_L2CAP_CONN            */  { 1,       0,       0,      0     },
    /* BR_L2CAP_DISCONN         */  { 2,       0x83,    0x83,   0x83  },
    /* BR_KEYS_RSP              */  { 0,       1,       0,      0     },
    /* BR_API_SEC_GRANT         */  { 0,       0,       0,      0     },
    /* BR_TK_REQ                */  { 0,       0,       0,      0     },
    /* BR_AUTH_CMPL             */  { 0,       0x82,    0x82,   0x82  },
    /* BR_ENC_REQ               */  { 0,       0,       0,      0     },
    /* BR_BOND_REQ              */  { 0,       0,       2,      0     },
    /* BR_DISCARD_SEC_REQ       */  { 0,       0,       0,      0     }
};

static const UINT8 smp_br_master_idle_table[][SMP_BR_SM_NUM_COLS] = {
    /*                                Event               Action               Next State */
    /* BR_L2CAP_CONN        */  {SMP_SEND_APP_CBACK, SMP_BR_SM_NO_ACTION, SMP_BR_STATE_WAIT_APP_RSP},
    /* BR_L2CAP_DISCONN   */  {SMP_IDLE_TERMINATE,  SMP_BR_SM_NO_ACTION, SMP_BR_STATE_IDLE}
};

static const UINT8 smp_br_master_wait_appln_response_table[][SMP_BR_SM_NUM_COLS] = {
    /*                                Event               Action              Next State */
    /* BR_KEYS_RSP           */{SMP_SEND_PAIR_REQ, SMP_BR_SM_NO_ACTION, SMP_BR_STATE_PAIR_REQ_RSP}
};

static const UINT8 smp_br_master_pair_request_response_table [][SMP_BR_SM_NUM_COLS] = {
    /*                        Event               Action                  Next State */
    /* BR_PAIRING_RSP   */  {SMP_BR_PROC_PAIR_CMD, SMP_BR_CHECK_AUTH_REQ, SMP_BR_STATE_PAIR_REQ_RSP},
    /* BR_BOND_REQ      */  {SMP_BR_SM_NO_ACTION, SMP_BR_SM_NO_ACTION, SMP_BR_STATE_BOND_PENDING}
};

static const UINT8 smp_br_master_bond_pending_table[][SMP_BR_SM_NUM_COLS] = {
    /*                                Event               Action              Next State */
    /* BR_ID_INFO               */{SMP_PROC_ID_INFO, SMP_BR_SM_NO_ACTION, SMP_BR_STATE_BOND_PENDING},
    /* BR_ID_ADDR               */{SMP_PROC_ID_ADDR, SMP_BR_SM_NO_ACTION, SMP_BR_STATE_BOND_PENDING},
    /* BR_SIGN_INFO             */{SMP_PROC_SRK_INFO, SMP_BR_SM_NO_ACTION, SMP_BR_STATE_BOND_PENDING}
};

static const UINT8 smp_br_slave_entry_map[][SMP_BR_STATE_MAX] = {
    /* br_state name:               Idle      WaitApp  Pair    Bond
                                              Rsp      ReqRsp  Pend      */
    /* BR_PAIRING_REQ           */  { 1,       0,       0,      0    },
    /* BR_PAIRING_RSP           */  { 0,       0,       0,      0    },
    /* BR_CONFIRM               */  { 0,       0,       0,      0    },
    /* BR_RAND                  */  { 0,       0,       0,      0    },
    /* BR_PAIRING_FAILED        */  { 0,       0x81,    0x81,   0x81 },
    /* BR_ENCRPTION_INFO        */  { 0,       0,       0,      0    },
    /* BR_MASTER_ID             */  { 0,       0,       0,      0    },
    /* BR_ID_INFO               */  { 0,       0,       0,      1    },
    /* BR_ID_ADDR               */  { 0,       0,       0,      2    },
    /* BR_SIGN_INFO             */  { 0,       0,       0,      3    },
    /* BR_SECURITY_REQ          */  { 0,       0,       0,      0    },
    /* BR_PAIR_PUBLIC_KEY_EVT   */  { 0,       0,       0,      0    },
    /* BR_PAIR_DHKEY_CHCK_EVT   */  { 0,       0,       0,      0    },
    /* BR_PAIR_KEYPR_NOTIF_EVT  */  { 0,       0,       0,      0    },
    /* BR_KEY_READY             */  { 0,       0,       0,      0    },
    /* BR_ENCRYPTED             */  { 0,       0,       0,      0    },
    /* BR_L2CAP_CONN            */  { 0,       0,       0,      0    },
    /* BR_L2CAP_DISCONN         */  { 0,       0x83,    0x83,   0x83 },
    /* BR_KEYS_RSP              */  { 0,       2,       0,      0    },
    /* BR_API_SEC_GRANT         */  { 0,       1,       0,      0    },
    /* BR_TK_REQ                */  { 0,       0,       0,      0    },
    /* BR_AUTH_CMPL             */  { 0,       0x82,    0x82,   0x82 },
    /* BR_ENC_REQ               */  { 0,       0,       0,      0    },
    /* BR_BOND_REQ              */  { 0,       3,       0,      0    },
    /* BR_DISCARD_SEC_REQ       */  { 0,       0,       0,      0    }
};

static const UINT8 smp_br_slave_idle_table[][SMP_BR_SM_NUM_COLS] = {
    /*                               Event                Action              Next State */
    /* BR_PAIRING_REQ    */ {SMP_BR_PROC_PAIR_CMD, SMP_SEND_APP_CBACK, SMP_BR_STATE_WAIT_APP_RSP}
};

static const UINT8 smp_br_slave_wait_appln_response_table [][SMP_BR_SM_NUM_COLS] = {
    /*                               Event                 Action             Next State */
    /* BR_API_SEC_GRANT */ {SMP_BR_PROC_SEC_GRANT, SMP_SEND_APP_CBACK, SMP_BR_STATE_WAIT_APP_RSP},
    /* BR_KEYS_RSP     */{SMP_BR_PROC_SL_KEYS_RSP, SMP_BR_CHECK_AUTH_REQ, SMP_BR_STATE_WAIT_APP_RSP},
    /* BR_BOND_REQ        */ {SMP_BR_KEY_DISTRIBUTION, SMP_BR_SM_NO_ACTION, SMP_BR_STATE_BOND_PENDING}
};

static const UINT8 smp_br_slave_bond_pending_table[][SMP_BR_SM_NUM_COLS] = {
    /*                                Event               Action               Next State */
    /* BR_ID_INFO               */  {SMP_PROC_ID_INFO, SMP_BR_SM_NO_ACTION, SMP_BR_STATE_BOND_PENDING},
    /* BR_ID_ADDR               */  {SMP_PROC_ID_ADDR, SMP_BR_SM_NO_ACTION, SMP_BR_STATE_BOND_PENDING},
    /* BR_SIGN_INFO             */  {SMP_PROC_SRK_INFO, SMP_BR_SM_NO_ACTION, SMP_BR_STATE_BOND_PENDING}
};

static const tSMP_BR_SM_TBL smp_br_state_table[][2] = {
    /* SMP_BR_STATE_IDLE */
    {smp_br_master_idle_table, smp_br_slave_idle_table},

    /* SMP_BR_STATE_WAIT_APP_RSP */
    {smp_br_master_wait_appln_response_table, smp_br_slave_wait_appln_response_table},

    /* SMP_BR_STATE_PAIR_REQ_RSP */
    {smp_br_master_pair_request_response_table, NULL},

    /* SMP_BR_STATE_BOND_PENDING */
    {smp_br_master_bond_pending_table, smp_br_slave_bond_pending_table},
};

typedef const UINT8 (*tSMP_BR_ENTRY_TBL)[SMP_BR_STATE_MAX];

static const tSMP_BR_ENTRY_TBL smp_br_entry_table[] = {
    smp_br_master_entry_map,
    smp_br_slave_entry_map
};

#define SMP_BR_ALL_TABLE_MASK  0x80

/*******************************************************************************
** Function     smp_set_br_state
** Returns      None
*******************************************************************************/
void smp_set_br_state(tSMP_BR_STATE br_state)
{
    if (br_state < SMP_BR_STATE_MAX) {
        SMP_TRACE_DEBUG( "BR_State change: %s(%d) ==> %s(%d)",
                         smp_get_br_state_name(smp_cb.br_state), smp_cb.br_state,
                         smp_get_br_state_name(br_state), br_state );
        smp_cb.br_state = br_state;
    } else {
        SMP_TRACE_DEBUG("%s invalid br_state =%d", __FUNCTION__, br_state );
    }
}

/*******************************************************************************
** Function     smp_get_br_state
** Returns      The smp_br state
*******************************************************************************/
tSMP_BR_STATE smp_get_br_state(void)
{
    return smp_cb.br_state;
}

/*******************************************************************************
** Function     smp_get_br_state_name
** Returns      The smp_br state name.
*******************************************************************************/
const char *smp_get_br_state_name(tSMP_BR_STATE br_state)
{
    const char *p_str = smp_br_state_name[SMP_BR_STATE_MAX];

    if (br_state < SMP_BR_STATE_MAX) {
        p_str = smp_br_state_name[br_state];
    }

    return p_str;
}
/*******************************************************************************
** Function     smp_get_br_event_name
** Returns      The smp_br event name.
*******************************************************************************/
const char *smp_get_br_event_name(tSMP_BR_EVENT event)
{
    const char *p_str = smp_br_event_name[SMP_BR_MAX_EVT - 1];

    if (event < SMP_BR_MAX_EVT) {
        p_str = smp_br_event_name[event - 1];
    }
    return p_str;
}

/*******************************************************************************
**
** Function     smp_br_state_machine_event
**
** Description  Handle events to the state machine. It looks up the entry
**              in the smp_br_entry_table array.
**              If it is a valid entry, it gets the state table.Set the next state,
**              if not NULL state. Execute the action function according to the
**              state table. If the state returned by action function is not NULL
**              state, adjust the new state to the returned state.
**
** Returns      void.
**
*******************************************************************************/
void smp_br_state_machine_event(tSMP_CB *p_cb, tSMP_BR_EVENT event, void *p_data)
{
    tSMP_BR_STATE       curr_state = p_cb->br_state;
    tSMP_BR_SM_TBL      state_table;
    UINT8               action, entry;
    tSMP_BR_ENTRY_TBL   entry_table =  smp_br_entry_table[p_cb->role];

    SMP_TRACE_EVENT("main %s", __func__);
    if (curr_state >= SMP_BR_STATE_MAX) {
        SMP_TRACE_DEBUG( "Invalid br_state: %d", curr_state) ;
        return;
    }

    SMP_TRACE_DEBUG( "SMP Role: %s State: [%s (%d)], Event: [%s (%d)]",
                     (p_cb->role == HCI_ROLE_SLAVE) ? "Slave" : "Master",
                     smp_get_br_state_name( p_cb->br_state),
                     p_cb->br_state, smp_get_br_event_name(event), event) ;

    /* look up the state table for the current state */
    /* lookup entry / w event & curr_state */
    /* If entry is ignore, return.
     * Otherwise, get state table (according to curr_state or all_state) */
    if ((event <= SMP_BR_MAX_EVT) && ( (entry = entry_table[event - 1][curr_state])
                                       != SMP_BR_SM_IGNORE )) {
        if (entry & SMP_BR_ALL_TABLE_MASK) {
            entry &= ~SMP_BR_ALL_TABLE_MASK;
            state_table = smp_br_all_table;
        } else {
            state_table = smp_br_state_table[curr_state][p_cb->role];
        }
    } else {
        SMP_TRACE_DEBUG( "Ignore event [%s (%d)] in state [%s (%d)]",
                         smp_get_br_event_name(event), event,
                         smp_get_br_state_name(curr_state), curr_state);
        return;
    }

    /* Get possible next state from state table. */

    smp_set_br_state(state_table[entry - 1][SMP_BR_SME_NEXT_STATE]);

    /* If action is not ignore, clear param, exec action and get next state.
     * The action function may set the Param for cback.
     * Depending on param, call cback or free buffer. */
    /* execute action functions */
    for (UINT8 i = 0; i < SMP_BR_NUM_ACTIONS; i++) {
        if ((action = state_table[entry - 1][i]) != SMP_BR_SM_NO_ACTION) {
            (*smp_br_sm_action[action])(p_cb, (tSMP_INT_DATA *)p_data);
        } else {
            break;
        }
    }
    SMP_TRACE_DEBUG( "result state = %s", smp_get_br_state_name( p_cb->br_state ) ) ;
}

#endif
