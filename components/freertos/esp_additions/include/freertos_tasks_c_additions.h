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
		return &( xPendingReadyList[idx] );
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
