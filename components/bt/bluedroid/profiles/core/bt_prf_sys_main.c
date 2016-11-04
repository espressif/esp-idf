/**
 ****************************************************************************************
 *
 * @file bt_prf_sys_main.c
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/10/13
 *
 *
 ****************************************************************************************
 */
 
#include "thread.h"
#include "bt_prf_sys.h"
#include "fixed_queue.h"
#include "bt_prf_task.h"
#include "gki.h"
#include "bt_trace.h"

#include <string.h>


tBT_PRF_SYS_CB bt_prf_sys_cb;
fixed_queue_t *bt_profile_msg_queue;


static const tBT_PRF_SYS_REG bt_prf_sys_reg =
{
    NULL,
    NULL
};


void bt_prf_sys_init(void)
{
	LOG_ERROR("bt_prf_sys_init\n");
	memset(&bt_prf_sys_cb, 0, sizeof(tBT_PRF_SYS_CB));
	bt_prf_StartUp();
}


/*******************************************************************************
**
** Function         bt_prf_sys_event
**
** Description      profile task event handler; called from task event handler.
**
**
** Returns          void
**
*******************************************************************************/
void bt_prf_sys_event(prf_hdr_evt_t *p_msg)
{
    UINT8       id;
    BOOLEAN     freebuf = TRUE;

    LOG_ERROR("profile task got event 0x%x\n", p_msg->event);

    /* get subsystem id from event */
    id = (UINT8) (p_msg->event >> 8);

    /* verify id and call subsystem event handler */
    if ((id < PRF_ID_MAX) && (bt_prf_sys_cb.reg[id] != NULL))
    {
        freebuf = (*bt_prf_sys_cb.reg[id]->evt_hdlr)(p_msg);
    }
    else
    {
        LOG_ERROR("profile task got unregistered event id %d\n", id);
    }

    if (freebuf)
    {
        GKI_freebuf(p_msg);
    }

}


/*******************************************************************************
**
** Function         bt_prf_sys_register
**
** Description      Called by other profile subsystems to register their event
**                  handler.
**
** Parameters		id:the Identifiers index of the profile
**					p_reg:the callback event which has been register to the profile task					
** Returns          void
**
*******************************************************************************/
void bt_prf_sys_register(UINT8 id, const tBT_PRF_SYS_REG *p_reg)
{
    bt_prf_sys_cb.reg[id] = (tBT_PRF_SYS_REG *) p_reg;
    bt_prf_sys_cb.is_reg[id] = TRUE;
}

/*******************************************************************************
**
** Function         bt_prf_sys_deregister
**
** Description      Called by other profile subsystems to de-register
**                  handler.
**
** Parameters		id:Identifiers index of the profile
** Returns          void
**
*******************************************************************************/
void bt_prf_sys_deregister(UINT8 id)
{
	bt_prf_sys_cb.reg[id] = NULL;
    bt_prf_sys_cb.is_reg[id] = FALSE;
}

/*******************************************************************************
**
** Function         bt_prf_sys_is_register
**
** Description      Called by other profile subsystems to get registeration
**                  status.
**
**
** Returns          void
**
*******************************************************************************/
BOOLEAN bt_prf_sys_is_register(UINT8 id)
{
    return bt_prf_sys_cb.is_reg[id];
}

/*******************************************************************************
**
** Function         bt_prf_sys_sendmsg
**
** Description      Send a message to the profile task.
**
**
** Returns          void
**
*******************************************************************************/
void bt_prf_sys_sendmsg(void *p_msg)
{
    // There is a race condition that occurs if the stack is shut down while
    // there is a procedure in progress that can schedule a task via this
    // message queue. This causes |bt_profile_msg_queue| to get cleaned up before
    // it gets used here; hence we check for NULL before using it.
    if (bt_profile_msg_queue) {
        fixed_queue_enqueue(bt_profile_msg_queue, p_msg);

        bt_prf_task_post(SIG_PRF_WORK);
    }
}
