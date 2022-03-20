/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * This file will be included in `tasks.c` file, thus, it must NOT be included
 * by any (other) file.
 * The functions below only consist in getters for the static variables in
 * `tasks.c` file.
 * The only source files that should call these functions are the ones in
 * `/additions` directory.
 */

#if ( configENABLE_TASK_SNAPSHOT == 1 )

	UBaseType_t pxTCBGetSize ( void )
	{
		return sizeof(TCB_t);
	}

	ListItem_t*	pxTCBGetStateListItem ( void *pxTCB )
	{
		return &(((TCB_t*)pxTCB)->xStateListItem);
	}

	StackType_t* pxTCBGetStartOfStack ( void *pxTCB )
	{
		return (StackType_t*) ((TCB_t*)pxTCB)->pxStack;
	}

	StackType_t* pxTCBGetTopOfStack ( void *pxTCB )
	{
		return (StackType_t*) ((TCB_t*)pxTCB)->pxTopOfStack;
	}

	StackType_t* pxTCBGetEndOfStack ( void *pxTCB )
	{
		return (StackType_t*) ((TCB_t*)pxTCB)->pxEndOfStack;
	}


	List_t* pxListGetReadyTask ( UBaseType_t idx )
	{
		return &( pxReadyTasksLists[idx] );
	}

	List_t* pxListGetReadyPendingTask ( UBaseType_t idx )
	{
#ifdef CONFIG_FREERTOS_SMP
		return &( xPendingReadyList );
#else
		return &( xPendingReadyList[idx] );
#endif
	}

	List_t* pxGetDelayedTaskList ( void ) {
		return pxDelayedTaskList;
	}

	List_t* pxGetOverflowDelayedTaskList ( void ) {
		return pxOverflowDelayedTaskList;
	}

	List_t* pxGetTasksWaitingTermination ( void ) {
		return &xTasksWaitingTermination;
	}

	List_t* pxGetSuspendedTaskList ( void ) {
		return &xSuspendedTaskList;
	}

#endif

#if ( configENABLE_FREERTOS_DEBUG_OCDAWARE == 1 )

/**
 * Debug param indexes. DO NOT change the order. OpenOCD uses the same indexes
 * Entries in FreeRTOS_openocd_params must match the order of these indexes
 */
enum {
	ESP_FREERTOS_DEBUG_TABLE_SIZE = 0,
	ESP_FREERTOS_DEBUG_TABLE_VERSION,
	ESP_FREERTOS_DEBUG_KERNEL_VER_MAJOR,
	ESP_FREERTOS_DEBUG_KERNEL_VER_MINOR,
	ESP_FREERTOS_DEBUG_KERNEL_VER_BUILD,
	ESP_FREERTOS_DEBUG_UX_TOP_USED_PIORITY,
	ESP_FREERTOS_DEBUG_PX_TOP_OF_STACK,
	ESP_FREERTOS_DEBUG_PC_TASK_NAME,
	/* New entries must be inserted here */
	ESP_FREERTOS_DEBUG_TABLE_END,
};

const DRAM_ATTR uint8_t FreeRTOS_openocd_params[ESP_FREERTOS_DEBUG_TABLE_END]  = {
	ESP_FREERTOS_DEBUG_TABLE_END,	 /* table size */
	1,								 /* table version */
	tskKERNEL_VERSION_MAJOR,
	tskKERNEL_VERSION_MINOR,
	tskKERNEL_VERSION_BUILD,
	configMAX_PRIORITIES - 1, 		 /* uxTopUsedPriority */
	offsetof(TCB_t, pxTopOfStack),	 /* thread_stack_offset; */
	offsetof(TCB_t, pcTaskName),	 /* thread_name_offset; */
};

#endif
