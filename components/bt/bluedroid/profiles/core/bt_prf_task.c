/**
 ****************************************************************************************
 *
 * @file bt_prf_task.c
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/10/11
 *
 *
 ****************************************************************************************
 */

#include "bt_prf_task.h"
#include "allocator.h"
#include "thread.h"
#include "gki.h"

 //thread_t *bt_workqueue_thread;
 //static const char *BT_WORKQUEUE_NAME = "bt_workqueue";
 xTaskHandle  xProfileTaskHandle = NULL;
 xQueueHandle xProfileQueue = 0;

 // Communication queue between bt_proflie_task and app.
 extern fixed_queue_t *bt_profile_msg_queue;

 
 /*****************************************************************************
 **
 ** Function		 bt_prf_task_thread_handler
 **
 ** Description 	 Process profile Task Thread.
 ******************************************************************************/
 void bt_prf_task_thread_handler(void *arg)
 {
	 //ke_event_clear(KE_EVENT_BTU_TASK_THREAD);
 
	 TaskEvt_t *e;
 
	 for (;;) {
		 if (pdTRUE == xQueueReceive(xProfileQueue, &e, (portTickType)portMAX_DELAY)) {
 
			 if (e->sig == SIG_BTU_WORK) {
				 fixed_queue_process(bt_profile_msg_queue);
			 }
			 else if (e->sig == SIG_BTU_START_UP) {
				 bt_prf_task_start_up();
			 }
			 osi_free(e); 
		 }
	 }
 }

void bt_profile_msg_ready(fixed_queue_t *queue) {
    BT_HDR *p_msg;

    while (!fixed_queue_is_empty(queue)) {
        p_msg = (BT_HDR *)fixed_queue_dequeue(queue);
    }
}


void bt_prf_task_start_up(void)
{
	 fixed_queue_register_dequeue(bt_profile_msg_queue, bt_profile_msg_ready);
}





