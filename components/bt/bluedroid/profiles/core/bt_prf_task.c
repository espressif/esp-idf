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
#include "bt_prf_sys.h"
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

 void bt_prf_task_post(uint32_t sig)
{
    TaskEvt_t *evt = (TaskEvt_t *)osi_malloc(sizeof(TaskEvt_t));
    if (evt == NULL)
        return;

    evt->sig = sig;
    evt->par = 0;

    if (xQueueSend(xProfileQueue, &evt, 10/portTICK_RATE_MS) != pdTRUE) {
            ets_printf("xProfileQueue failed\n");
    }
}

void bt_profile_msg_ready(fixed_queue_t *queue) {
    prf_hdr_evt_t *p_msg;

    while (!fixed_queue_is_empty(queue)) {
        p_msg = (prf_hdr_evt_t *)fixed_queue_dequeue(queue);
		if(p_msg != NULL)
		{
			bt_prf_sys_event(p_msg);
		}
    }
}


void bt_prf_task_start_up(void)
{
	 fixed_queue_register_dequeue(bt_profile_msg_queue, bt_profile_msg_ready);
}

void bt_prf_task_shut_down(void) 
{
	fixed_queue_unregister_dequeue(bt_profile_msg_queue);
	
  	bt_prf_free_core();
}


void bt_prf_StartUp(void)
{
	bt_profile_msg_queue = fixed_queue_new(SIZE_MAX);
    if (bt_profile_msg_queue == NULL)
        goto error_exit;

	return;

error_exit:;
   LOG_ERROR("%s Unable to allocate resources for bt_workqueue\n", __func__);
   bt_prf_ShutDown();

}

void bt_prf_ShutDown(void)
{
	
	bt_prf_task_shut_down();

	//thread_free(bt_workqueue_thread);
  	vTaskDelete(xProfileTaskHandle);
  	vQueueDelete(xProfileQueue);

	bt_profile_msg_queue = NULL;

	// bt_workqueue_thread = NULL;
  	xProfileTaskHandle = NULL;
 	xProfileQueue = 0;
}


void  bt_prf_free_core(void)
{
	
}



