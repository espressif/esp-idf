/**
 ****************************************************************************************
 *
 * @file bt_prf_task.h
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/10/11
 *
 *
 ****************************************************************************************
 */

#include <stddef.h> 
#include "fixed_queue.h"

#ifndef BT_PRF_TASK_H__
#define BT_PRF_TASK_H__

/* Functions provided by btu_core.c
************************************
*/


void bt_prf_task_thread_handler(void *arg);

void  bt_prf_init_core(void);
void  bt_prf_free_core(void);

void bt_prf_StartUp(void);
void bt_prf_ShutDown(void);

void bt_prf_task_start_up(void);
void bt_prf_task_shut_down(void);

void bt_profile_msg_ready(fixed_queue_t *queue);








#endif		/// BT_PRF_TASK_H__
