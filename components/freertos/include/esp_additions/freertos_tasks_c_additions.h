// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
