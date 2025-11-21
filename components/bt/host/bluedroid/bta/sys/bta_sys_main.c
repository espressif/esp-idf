/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  This is the main implementation file for the BTA system manager.
 *
 ******************************************************************************/
#define LOG_TAG "bt_bta_sys_main"

#include <assert.h>
#include <string.h>

#include "osi/alarm.h"
#include "osi/thread.h"
#include "stack/btm_api.h"
#include "stack/btu.h"
#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "bta_sys_int.h"

#include "osi/fixed_queue.h"
#include "osi/hash_map.h"
#include "osi/osi.h"
#include "osi/hash_functions.h"
#if( defined BTA_AR_INCLUDED ) && (BTA_AR_INCLUDED == TRUE)
#include "bta/bta_ar_api.h"
#endif
#include "bta/utl.h"
#include "osi/allocator.h"
#include "osi/mutex.h"


/* system manager control block definition */
#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_SYS_CB bta_sys_cb;
#else
tBTA_SYS_CB *bta_sys_cb_ptr;
#endif

static hash_map_t *bta_alarm_hash_map;
static const size_t BTA_ALARM_HASH_MAP_SIZE = 17;
static osi_mutex_t bta_alarm_lock;
// extern thread_t *bt_workqueue_thread;

/* trace level */
/* TODO Bluedroid - Hard-coded trace levels -  Needs to be configurable */
UINT8 appl_trace_level = APPL_INITIAL_TRACE_LEVEL;
UINT8 btif_trace_level = BTIF_INITIAL_TRACE_LEVEL;

void btu_bta_alarm_ready(fixed_queue_t *queue);

static const tBTA_SYS_REG bta_sys_hw_reg = {
    bta_sys_sm_execute,
    NULL
};


/* type for action functions */
typedef void (*tBTA_SYS_ACTION)(tBTA_SYS_HW_MSG *p_data);

/* action function list */
const tBTA_SYS_ACTION bta_sys_action[] = {
    /* device manager local device API events - cf bta/bta_sys.h for events */
    bta_sys_hw_api_enable,             /* 0  BTA_SYS_HW_API_ENABLE_EVT    */
    bta_sys_hw_evt_enabled,           /* 1  BTA_SYS_HW_EVT_ENABLED_EVT */
    bta_sys_hw_evt_stack_enabled,       /* 2  BTA_SYS_HW_EVT_STACK_ENABLED_EVT */
    bta_sys_hw_api_disable,             /* 3  BTA_SYS_HW_API_DISABLE_EVT     */
    bta_sys_hw_evt_disabled,           /* 4  BTA_SYS_HW_EVT_DISABLED_EVT  */
    bta_sys_hw_error                        /* 5   BTA_SYS_HW_ERROR_EVT  */
};

/* state machine action enumeration list */
enum {
    /* device manager local device API events */
    BTA_SYS_HW_API_ENABLE,
    BTA_SYS_HW_EVT_ENABLED,
    BTA_SYS_HW_EVT_STACK_ENABLED,
    BTA_SYS_HW_API_DISABLE,
    BTA_SYS_HW_EVT_DISABLED,
    BTA_SYS_HW_ERROR
};

#define BTA_SYS_NUM_ACTIONS  (BTA_SYS_MAX_EVT & 0x00ff)
#define BTA_SYS_IGNORE       BTA_SYS_NUM_ACTIONS

/* state table information */
#define BTA_SYS_ACTIONS              2       /* number of actions */
#define BTA_SYS_NEXT_STATE           2       /* position of next state */
#define BTA_SYS_NUM_COLS             3       /* number of columns in state tables */


/* state table for OFF state */
const UINT8 bta_sys_hw_off[][BTA_SYS_NUM_COLS] = {
    /* Event                    Action 1               Action 2             Next State */
    /* API_ENABLE    */  {BTA_SYS_HW_API_ENABLE,    BTA_SYS_IGNORE,     BTA_SYS_HW_STARTING},
    /* EVT_ENABLED   */  {BTA_SYS_IGNORE,           BTA_SYS_IGNORE,     BTA_SYS_HW_STARTING},
    /* STACK_ENABLED */  {BTA_SYS_IGNORE,           BTA_SYS_IGNORE,     BTA_SYS_HW_ON},
    /* API_DISABLE   */  {BTA_SYS_HW_EVT_DISABLED,  BTA_SYS_IGNORE,     BTA_SYS_HW_OFF},
    /* EVT_DISABLED  */  {BTA_SYS_IGNORE,           BTA_SYS_IGNORE,     BTA_SYS_HW_OFF},
    /* EVT_ERROR     */  {BTA_SYS_IGNORE,           BTA_SYS_IGNORE,     BTA_SYS_HW_OFF}
};

const UINT8 bta_sys_hw_starting[][BTA_SYS_NUM_COLS] = {
    /* Event                    Action 1                   Action 2               Next State */
    /* API_ENABLE    */  {BTA_SYS_IGNORE,               BTA_SYS_IGNORE,         BTA_SYS_HW_STARTING}, /* wait for completion event */
    /* EVT_ENABLED   */  {BTA_SYS_HW_EVT_ENABLED,       BTA_SYS_IGNORE,         BTA_SYS_HW_STARTING},
    /* STACK_ENABLED */  {BTA_SYS_HW_EVT_STACK_ENABLED, BTA_SYS_IGNORE,         BTA_SYS_HW_ON},
    /* API_DISABLE   */  {BTA_SYS_IGNORE,               BTA_SYS_IGNORE,         BTA_SYS_HW_STOPPING}, /* successive disable/enable: change state wait for completion to disable */
    /* EVT_DISABLED  */  {BTA_SYS_HW_EVT_DISABLED,      BTA_SYS_HW_API_ENABLE,  BTA_SYS_HW_STARTING}, /* successive enable/disable: notify, then restart HW */
    /* EVT_ERROR */      {BTA_SYS_HW_ERROR,             BTA_SYS_IGNORE,         BTA_SYS_HW_ON}
};

const UINT8 bta_sys_hw_on[][BTA_SYS_NUM_COLS] = {
    /* Event                    Action 1                   Action 2               Next State */
    /* API_ENABLE    */  {BTA_SYS_HW_API_ENABLE,        BTA_SYS_IGNORE,         BTA_SYS_HW_ON},
    /* EVT_ENABLED   */  {BTA_SYS_IGNORE,               BTA_SYS_IGNORE,         BTA_SYS_HW_ON},
    /* STACK_ENABLED */  {BTA_SYS_IGNORE,               BTA_SYS_IGNORE,         BTA_SYS_HW_ON},
    /* API_DISABLE   */  {BTA_SYS_HW_API_DISABLE,       BTA_SYS_IGNORE,         BTA_SYS_HW_ON}, /* don't change the state here, as some other modules might be active */
    /* EVT_DISABLED */   {BTA_SYS_HW_ERROR,             BTA_SYS_IGNORE,         BTA_SYS_HW_ON},
    /* EVT_ERROR */      {BTA_SYS_HW_ERROR,             BTA_SYS_IGNORE,         BTA_SYS_HW_ON}
};

const UINT8 bta_sys_hw_stopping[][BTA_SYS_NUM_COLS] = {
    /* Event                    Action 1                   Action 2               Next State */
    /* API_ENABLE    */  {BTA_SYS_IGNORE,               BTA_SYS_IGNORE,         BTA_SYS_HW_STARTING}, /* change state, and wait for completion event to enable */
    /* EVT_ENABLED   */  {BTA_SYS_HW_EVT_ENABLED,       BTA_SYS_IGNORE,         BTA_SYS_HW_STOPPING}, /* successive enable/disable: finish the enable before disabling */
    /* STACK_ENABLED */  {BTA_SYS_HW_EVT_STACK_ENABLED, BTA_SYS_HW_API_DISABLE, BTA_SYS_HW_STOPPING}, /* successive enable/disable: notify, then stop */
    /* API_DISABLE   */  {BTA_SYS_IGNORE,               BTA_SYS_IGNORE,         BTA_SYS_HW_STOPPING}, /* wait for completion event */
    /* EVT_DISABLED  */  {BTA_SYS_HW_EVT_DISABLED,      BTA_SYS_IGNORE,         BTA_SYS_HW_OFF},
    /* EVT_ERROR     */  {BTA_SYS_HW_API_DISABLE,       BTA_SYS_IGNORE,         BTA_SYS_HW_STOPPING}
};

typedef const UINT8 (*tBTA_SYS_ST_TBL)[BTA_SYS_NUM_COLS];

/* state table */
const tBTA_SYS_ST_TBL bta_sys_st_tbl[] = {
    bta_sys_hw_off,
    bta_sys_hw_starting,
    bta_sys_hw_on,
    bta_sys_hw_stopping
};

/*******************************************************************************
**
** Function         bta_sys_init
**
** Description      BTA initialization; called from task initialization.
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_init(void)
{
    memset(&bta_sys_cb, 0, sizeof(tBTA_SYS_CB));

    osi_mutex_new(&bta_alarm_lock);

    bta_alarm_hash_map = hash_map_new(BTA_ALARM_HASH_MAP_SIZE,
                                      hash_function_pointer, NULL, (data_free_fn)osi_alarm_free, NULL);

    appl_trace_level = APPL_INITIAL_TRACE_LEVEL;

    /* register BTA SYS message handler */
    bta_sys_register( BTA_ID_SYS,  &bta_sys_hw_reg);

    /* register for BTM notifications */
    BTM_RegisterForDeviceStatusNotif ((tBTM_DEV_STATUS_CB *)&bta_sys_hw_btm_cback );

#if( defined BTA_AR_INCLUDED ) && (BTA_AR_INCLUDED == TRUE)
    bta_ar_init();
#endif

}

void bta_sys_free(void)
{
    hash_map_free(bta_alarm_hash_map);
    osi_mutex_free(&bta_alarm_lock);
#if BTA_DYNAMIC_MEMORY
    FREE_AND_RESET(bta_sys_cb_ptr);
#endif
}

/*******************************************************************************
**
** Function         bta_sys_sm_execute
**
** Description      State machine event handling function for DM
**
**
** Returns          void
**
*******************************************************************************/
BOOLEAN bta_sys_sm_execute(BT_HDR *p_msg)
{
    BOOLEAN freebuf = TRUE;
    tBTA_SYS_ST_TBL      state_table;
    UINT8               action;
    int                 i;

    APPL_TRACE_EVENT("bta_sys_sm_execute state:%d, event:0x%x\n",  bta_sys_cb.state, p_msg->event);

    /* look up the state table for the current state */
    state_table = bta_sys_st_tbl[bta_sys_cb.state];
    /* update state */
    bta_sys_cb.state = state_table[p_msg->event & 0x00ff][BTA_SYS_NEXT_STATE];

    /* execute action functions */
    for (i = 0; i < BTA_SYS_ACTIONS; i++) {
        if ((action = state_table[p_msg->event & 0x00ff][i]) != BTA_SYS_IGNORE) {
            (*bta_sys_action[action])( (tBTA_SYS_HW_MSG *) p_msg);
        } else {
            break;
        }
    }
    return freebuf;

}


void bta_sys_hw_register( tBTA_SYS_HW_MODULE module, tBTA_SYS_HW_CBACK *cback)
{
    bta_sys_cb.sys_hw_cback[module] = cback;
}


void bta_sys_hw_unregister( tBTA_SYS_HW_MODULE module )
{
    bta_sys_cb.sys_hw_cback[module] = NULL;
}

/*******************************************************************************
**
** Function         bta_sys_hw_btm_cback
**
** Description     This function is registered by BTA SYS to BTM in order to get status notifications
**
**
** Returns
**
*******************************************************************************/
void bta_sys_hw_btm_cback( tBTM_DEV_STATUS status )
{

    tBTA_SYS_HW_MSG *sys_event;

    APPL_TRACE_DEBUG(" bta_sys_hw_btm_cback was called with parameter: %i" , status );

    /* send a message to BTA SYS */
    if ((sys_event = (tBTA_SYS_HW_MSG *) osi_malloc(sizeof(tBTA_SYS_HW_MSG))) != NULL) {
        if (status == BTM_DEV_STATUS_UP) {
            sys_event->hdr.event = BTA_SYS_EVT_STACK_ENABLED_EVT;
        } else if (status == BTM_DEV_STATUS_DOWN) {
            sys_event->hdr.event = BTA_SYS_ERROR_EVT;
        } else {
            /* BTM_DEV_STATUS_CMD_TOUT is ignored for now. */
            osi_free (sys_event);
            sys_event = NULL;
        }

        if (sys_event) {
            bta_sys_sendmsg(sys_event);
        }
    } else {
        APPL_TRACE_DEBUG("ERROR bta_sys_hw_btm_cback couldn't send msg" );
    }
}



/*******************************************************************************
**
** Function         bta_sys_hw_error
**
** Description     In case the HW device stops answering... Try to turn it off, then re-enable all
**                      previously active SW modules.
**
** Returns          success or failure
**
*******************************************************************************/
void bta_sys_hw_error(tBTA_SYS_HW_MSG *p_sys_hw_msg)
{
    UINT8 module_index;
    UNUSED(p_sys_hw_msg);

    APPL_TRACE_DEBUG("%s\n", __FUNCTION__);

    for (module_index = 0; module_index < BTA_SYS_MAX_HW_MODULES; module_index++) {
        if ( bta_sys_cb.sys_hw_module_active &  ((UINT32)1 << module_index )) {
            switch ( module_index) {
            case BTA_SYS_HW_BLUETOOTH:
                /* Send BTA_SYS_HW_ERROR_EVT to DM */
                if (bta_sys_cb.sys_hw_cback[module_index] != NULL) {
                    bta_sys_cb.sys_hw_cback[module_index] (BTA_SYS_HW_ERROR_EVT);
                }
                break;
            default:
                /* not yet supported */
                break;
            }
        }
    }
}



/*******************************************************************************
**
** Function         bta_sys_hw_enable
**
** Description     this function is called after API enable and HW has been turned on
**
**
** Returns          success or failure
**
*******************************************************************************/

void bta_sys_hw_api_enable( tBTA_SYS_HW_MSG *p_sys_hw_msg )
{
    if ((!bta_sys_cb.sys_hw_module_active) && (bta_sys_cb.state != BTA_SYS_HW_ON)) {
        /* register which HW module was turned on */
        bta_sys_cb.sys_hw_module_active |=  ((UINT32)1 << p_sys_hw_msg->hw_module );

        tBTA_SYS_HW_MSG *p_msg;
        if ((p_msg = (tBTA_SYS_HW_MSG *) osi_malloc(sizeof(tBTA_SYS_HW_MSG))) != NULL) {
            p_msg->hdr.event = BTA_SYS_EVT_ENABLED_EVT;
            p_msg->hw_module = p_sys_hw_msg->hw_module;

            bta_sys_sendmsg(p_msg);
        }
    } else {
        /* register which HW module was turned on */
        bta_sys_cb.sys_hw_module_active |=  ((UINT32)1 << p_sys_hw_msg->hw_module );

        /* HW already in use, so directly notify the caller */
        if (bta_sys_cb.sys_hw_cback[p_sys_hw_msg->hw_module ] != NULL ) {
            bta_sys_cb.sys_hw_cback[p_sys_hw_msg->hw_module ](  BTA_SYS_HW_ON_EVT   );
        }
    }

    APPL_TRACE_EVENT ("bta_sys_hw_api_enable for %d, active modules 0x%04X\n",
                      p_sys_hw_msg->hw_module, bta_sys_cb.sys_hw_module_active);

}

/*******************************************************************************
**
** Function         bta_sys_hw_disable
**
** Description     if no other module is using the HW, this function will call ( if defined ) a user-macro to turn off the HW
**
**
** Returns          success or failure
**
*******************************************************************************/
void bta_sys_hw_api_disable(tBTA_SYS_HW_MSG *p_sys_hw_msg)
{
    APPL_TRACE_DEBUG("bta_sys_hw_api_disable for %d, active modules: 0x%04X\n",
                     p_sys_hw_msg->hw_module, bta_sys_cb.sys_hw_module_active );

    /* make sure the related SW blocks were stopped */
    bta_sys_disable( p_sys_hw_msg->hw_module );


    /* register which module we turn off */
    bta_sys_cb.sys_hw_module_active &=  ~((UINT32)1 << p_sys_hw_msg->hw_module );


    /* if there are still some SW modules using the HW, just provide an answer to the calling */
    if ( bta_sys_cb.sys_hw_module_active != 0  ) {
        /*  if there are still some SW modules using the HW,  directly notify the caller */
        if ( bta_sys_cb.sys_hw_cback[p_sys_hw_msg->hw_module ] != NULL ) {
            bta_sys_cb.sys_hw_cback[p_sys_hw_msg->hw_module ](  BTA_SYS_HW_OFF_EVT   );
        }
    } else {
        /* manually update the state of our system */
        bta_sys_cb.state = BTA_SYS_HW_STOPPING;

        tBTA_SYS_HW_MSG *p_msg;
        if ((p_msg = (tBTA_SYS_HW_MSG *) osi_malloc(sizeof(tBTA_SYS_HW_MSG))) != NULL) {
            p_msg->hdr.event = BTA_SYS_EVT_DISABLED_EVT;
            p_msg->hw_module = p_sys_hw_msg->hw_module;

            bta_sys_sendmsg(p_msg);
        }
    }

}


/*******************************************************************************
**
** Function         bta_sys_hw_event_enabled
**
** Description
**
**
** Returns          success or failure
**
*******************************************************************************/
void bta_sys_hw_evt_enabled(tBTA_SYS_HW_MSG *p_sys_hw_msg)
{
    APPL_TRACE_EVENT("bta_sys_hw_evt_enabled for %i\n", p_sys_hw_msg->hw_module);
    BTM_DeviceReset( NULL );
}


/*******************************************************************************
**
** Function         bta_sys_hw_event_disabled
**
** Description
**
**
** Returns          success or failure
**
*******************************************************************************/
void bta_sys_hw_evt_disabled(tBTA_SYS_HW_MSG *p_sys_hw_msg)
{
    UINT8 hw_module_index;

    APPL_TRACE_DEBUG("bta_sys_hw_evt_disabled - module 0x%X\n", p_sys_hw_msg->hw_module);

    for (hw_module_index = 0; hw_module_index < BTA_SYS_MAX_HW_MODULES; hw_module_index++) {
        if (bta_sys_cb.sys_hw_cback[hw_module_index] != NULL) {
            bta_sys_cb.sys_hw_cback[hw_module_index] (BTA_SYS_HW_OFF_EVT);
        }
    }
}

/*******************************************************************************
**
** Function         bta_sys_hw_event_stack_enabled
**
** Description     we receive this event once the SW side is ready ( stack, FW download,... ),
**                       i.e. we can really start using the device. So notify the app.
**
** Returns          success or failure
**
*******************************************************************************/
void bta_sys_hw_evt_stack_enabled(tBTA_SYS_HW_MSG *p_sys_hw_msg)
{
    UINT8 hw_module_index;
    UNUSED(p_sys_hw_msg);

    APPL_TRACE_DEBUG(" bta_sys_hw_evt_stack_enabled!notify the callers\n");

    for (hw_module_index = 0; hw_module_index < BTA_SYS_MAX_HW_MODULES; hw_module_index++ ) {
        if (bta_sys_cb.sys_hw_cback[hw_module_index] != NULL) {
            bta_sys_cb.sys_hw_cback[hw_module_index] (BTA_SYS_HW_ON_EVT);
        }
    }
}




/*******************************************************************************
**
** Function         bta_sys_event
**
** Description      BTA event handler; called from task event handler.
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_event(void * param)
{
    BT_HDR *p_msg = (BT_HDR *)param;

    UINT8       id;
    BOOLEAN     freebuf = TRUE;

    APPL_TRACE_EVENT("BTA got event 0x%x\n", p_msg->event);

    /* get subsystem id from event */
    id = (UINT8) (p_msg->event >> 8);

    /* verify id and call subsystem event handler */
    if ((id < BTA_ID_MAX) && (bta_sys_cb.reg[id] != NULL)) {
        freebuf = (*bta_sys_cb.reg[id]->evt_hdlr)(p_msg);
    } else {
        APPL_TRACE_WARNING("BTA got unregistered event id %d\n", id);
    }

    if (freebuf) {
        osi_free(p_msg);
    }
}

/*******************************************************************************
**
** Function         bta_sys_register
**
** Description      Called by other BTA subsystems to register their event
**                  handler.
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_register(UINT8 id, const tBTA_SYS_REG *p_reg)
{
    bta_sys_cb.reg[id] = (tBTA_SYS_REG *) p_reg;
    bta_sys_cb.is_reg[id] = TRUE;
}

/*******************************************************************************
**
** Function         bta_sys_deregister
**
** Description      Called by other BTA subsystems to de-register
**                  handler.
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_deregister(UINT8 id)
{
    bta_sys_cb.is_reg[id] = FALSE;
}

/*******************************************************************************
**
** Function         bta_sys_is_register
**
** Description      Called by other BTA subsystems to get registration
**                  status.
**
**
** Returns          void
**
*******************************************************************************/
BOOLEAN bta_sys_is_register(UINT8 id)
{
    return bta_sys_cb.is_reg[id];
}

/*******************************************************************************
**
** Function         bta_sys_sendmsg
**
** Description      Send a message to BTA.  This function is designed to
**                  optimize sending of messages to BTA.  It is called by BTA
**                  API functions and call-in functions.
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_sendmsg(void *p_msg)
{
    // There is a race condition that occurs if the stack is shut down while
    // there is a procedure in progress that can schedule a task via this
    // message queue. This causes |btu_bta_msg_queue| to get cleaned up before
    // it gets used here; hence we check for NULL before using it.
    if (btu_task_post(SIG_BTU_BTA_MSG, p_msg, OSI_THREAD_MAX_TIMEOUT) == false) {
        osi_free(p_msg);
    }
}

/*******************************************************************************
**
** Function         bta_sys_start_timer
**
** Description      Start a protocol timer for the specified amount
**                  of time in milliseconds.
**
** Returns          void
**
*******************************************************************************/
void bta_alarm_cb(void *data)
{
    assert(data != NULL);
    TIMER_LIST_ENT *p_tle = (TIMER_LIST_ENT *)data;

    btu_task_post(SIG_BTU_BTA_ALARM, p_tle, OSI_THREAD_MAX_TIMEOUT);
}

void bta_sys_start_timer(TIMER_LIST_ENT *p_tle, UINT16 type, INT32 timeout_ms)
{
    assert(p_tle != NULL);

    // Get the alarm for this p_tle.
    osi_mutex_lock(&bta_alarm_lock, OSI_MUTEX_MAX_TIMEOUT);
    if (!hash_map_has_key(bta_alarm_hash_map, p_tle)) {
        hash_map_set(bta_alarm_hash_map, p_tle, osi_alarm_new("bta_sys", bta_alarm_cb, p_tle, 0));
    }
    osi_mutex_unlock(&bta_alarm_lock);

    osi_alarm_t *alarm = hash_map_get(bta_alarm_hash_map, p_tle);
    if (alarm == NULL) {
        APPL_TRACE_ERROR("%s unable to create alarm.", __func__);
        return;
    }

    p_tle->event = type;
    p_tle->ticks = timeout_ms;
    //osi_alarm_set(alarm, (period_ms_t)timeout_ms, bta_alarm_cb, p_tle);
    osi_alarm_set(alarm, (period_ms_t)timeout_ms);
}

bool hash_iter_ro_cb(hash_map_entry_t *hash_map_entry, void *context)
{
    osi_alarm_t *alarm = (osi_alarm_t *)hash_map_entry->data;
    period_ms_t *p_remaining_ms = (period_ms_t *)context;
    *p_remaining_ms += osi_alarm_get_remaining_ms(alarm);
    return true;
}

UINT32 bta_sys_get_remaining_ticks(TIMER_LIST_ENT *p_target_tle)
{
    period_ms_t remaining_ms = 0;
    osi_mutex_lock(&bta_alarm_lock, OSI_MUTEX_MAX_TIMEOUT);
    // Get the alarm for this p_tle
    hash_map_foreach(bta_alarm_hash_map, hash_iter_ro_cb, &remaining_ms);
    osi_mutex_unlock(&bta_alarm_lock);
    return remaining_ms;
}


/*******************************************************************************
**
** Function         bta_sys_timer_is_active
**
** Description      Get info of timer is active or not.
**
** Returns          true if timer is exist and active, false otherwise.
**
*******************************************************************************/
BOOLEAN bta_sys_timer_is_active(TIMER_LIST_ENT *p_tle)
{
    assert(p_tle != NULL);

    osi_alarm_t *alarm = hash_map_get(bta_alarm_hash_map, p_tle);
    if (alarm != NULL && osi_alarm_is_active(alarm)) {
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         bta_sys_stop_timer
**
** Description      Stop a BTA timer.
**
** Returns          void
**
*******************************************************************************/
void bta_sys_stop_timer(TIMER_LIST_ENT *p_tle)
{
    assert(p_tle != NULL);

    osi_alarm_t *alarm = hash_map_get(bta_alarm_hash_map, p_tle);
    if (alarm == NULL) {
        APPL_TRACE_DEBUG("%s expected alarm was not in bta alarm hash map.", __func__);
        return;
    }
    osi_alarm_cancel(alarm);
}

/*******************************************************************************
**
** Function         bta_sys_free_timer
**
** Description      Stop and free a BTA timer.
**
** Returns          void
**
*******************************************************************************/
void bta_sys_free_timer(TIMER_LIST_ENT *p_tle)
{
    assert(p_tle != NULL);

    osi_alarm_t *alarm = hash_map_get(bta_alarm_hash_map, p_tle);
    if (alarm == NULL) {
        APPL_TRACE_DEBUG("%s expected alarm was not in bta alarm hash map.", __func__);
        return;
    }
    osi_alarm_cancel(alarm);
    hash_map_erase(bta_alarm_hash_map, p_tle);
}

/*******************************************************************************
**
** Function         bta_sys_disable
**
** Description      For each registered subsystem execute its disable function.
**
** Returns          void
**
*******************************************************************************/
void bta_sys_disable(tBTA_SYS_HW_MODULE module)
{
    int bta_id = 0;
    int bta_id_max = 0;

    APPL_TRACE_DEBUG("bta_sys_disable: module %i", module);

    switch ( module ) {
    case BTA_SYS_HW_BLUETOOTH:
        bta_id = BTA_ID_DM;
        bta_id_max = BTA_ID_BLUETOOTH_MAX;
        break;
    default:
        APPL_TRACE_WARNING("bta_sys_disable: unknown module");
        return;
    }

    for ( ; bta_id <= bta_id_max; bta_id++) {
        if (bta_sys_cb.reg[bta_id] != NULL) {
            if (bta_sys_cb.is_reg[bta_id] == TRUE  &&  bta_sys_cb.reg[bta_id]->disable != NULL) {
                (*bta_sys_cb.reg[bta_id]->disable)();
            }
        }
    }
}

/*******************************************************************************
**
** Function         bta_sys_set_trace_level
**
** Description      Set trace level for BTA
**
** Returns          void
**
*******************************************************************************/
void bta_sys_set_trace_level(UINT8 level)
{
    appl_trace_level = level;
}
#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bta_sys_get_sys_features
**
** Description      Returns sys_features to other BTA modules.
**
** Returns          sys_features
**
*******************************************************************************/
UINT16 bta_sys_get_sys_features (void)
{
    return bta_sys_cb.sys_features;
}
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
