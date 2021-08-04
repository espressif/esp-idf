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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if ( configENABLE_TASK_SNAPSHOT == 1 )

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Check `freertos_tasks_c_additions.h` file for more info
 * about these functions declaration.
 */
UBaseType_t pxTCBGetSize ( void );
ListItem_t*	pxTCBGetStateListItem ( void *pxTCB );
StackType_t* pxTCBGetStartOfStack ( void *pxTCB );
StackType_t* pxTCBGetTopOfStack ( void *pxTCB );
StackType_t* pxTCBGetEndOfStack ( void *pxTCB );
List_t* pxListGetReadyTask ( UBaseType_t idx );
List_t* pxListGetReadyPendingTask ( UBaseType_t idx );
List_t* pxGetDelayedTaskList ( void );
List_t* pxGetOverflowDelayedTaskList ( void );
List_t* pxGetTasksWaitingTermination ( void );
List_t* pxGetSuspendedTaskList ( void );

/**
 * Used with the uxTaskGetSnapshotAll() function to save memory snapshot of each task in the system.
 * We need this struct because TCB_t is defined (hidden) in tasks.c.
 */
typedef struct xTASK_SNAPSHOT
{
	void        *pxTCB;         /*!< Address of task control block. */
	StackType_t *pxTopOfStack;  /*!< Points to the location of the last item placed on the tasks stack. */
	StackType_t *pxEndOfStack;  /*!< Points to the end of the stack. pxTopOfStack < pxEndOfStack, stack grows hi2lo
									pxTopOfStack > pxEndOfStack, stack grows lo2hi*/
} TaskSnapshot_t;


/*
 * This function fills array with TaskSnapshot_t structures for every task in the system.
 * Used by panic handling code to get snapshots of all tasks in the system.
 * Only available when configENABLE_TASK_SNAPSHOT is set to 1.
 * @param pxTaskSnapshotArray Pointer to array of TaskSnapshot_t structures to store tasks snapshot data.
 * @param uxArraySize Size of tasks snapshots array.
 * @param pxTcbSz Pointer to store size of TCB.
 * @return Number of elements stored in array.
 */
UBaseType_t uxTaskGetSnapshotAll( TaskSnapshot_t * const pxTaskSnapshotArray, const UBaseType_t uxArraySize, UBaseType_t * const pxTcbSz );

/*
 * This function iterates over all tasks in the system.
 * Used by panic handling code to iterate over tasks in the system.
 * Only available when configENABLE_TASK_SNAPSHOT is set to 1.
 * @note This function should not be used while FreeRTOS is running (as it doesn't acquire any locks).
 * @param pxTask task handle.
 * @return Handle for the next task. If pxTask is NULL, returns hadnle for the first task.
 */
TaskHandle_t pxTaskGetNext( TaskHandle_t pxTask );

/*
 * This function fills TaskSnapshot_t structure for specified task.
 * Used by panic handling code to get snapshot of a task.
 * Only available when configENABLE_TASK_SNAPSHOT is set to 1.
 * @note This function should not be used while FreeRTOS is running (as it doesn't acquire any locks).
 * @param pxTask task handle.
 * @param pxTaskSnapshot address of TaskSnapshot_t structure to fill.
 */
void vTaskGetSnapshot( TaskHandle_t pxTask, TaskSnapshot_t *pxTaskSnapshot );

#ifdef __cplusplus
}
#endif

#endif
