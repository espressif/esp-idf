/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task_snapshot.h"

#ifndef DIM
#define DIM(t) (sizeof(t)/ sizeof(*(t)))
#endif

#if ( configENABLE_TASK_SNAPSHOT == 1 )

	static void prvTaskGetSnapshot( TaskSnapshot_t *pxTaskSnapshotArray, UBaseType_t *uxTask, void *pxTCB )
	{
		if (pxTCB == NULL) {
			return;
		}
		pxTaskSnapshotArray[ *uxTask ].pxTCB = pxTCB;
		pxTaskSnapshotArray[ *uxTask ].pxTopOfStack = (StackType_t *) pxTCBGetTopOfStack(pxTCB);
		#if( portSTACK_GROWTH < 0 )
		{
			pxTaskSnapshotArray[ *uxTask ].pxEndOfStack = pxTCBGetEndOfStack(pxTCB);
		}
		#else
		{
			pxTaskSnapshotArray[ *uxTask ].pxEndOfStack = pxTCBGetStartOfStack(pxTCB);
		}
		#endif
		(*uxTask)++;
	}

	static void prvTaskGetSnapshotsFromList( TaskSnapshot_t *pxTaskSnapshotArray, UBaseType_t *uxTask, const UBaseType_t uxArraySize, List_t *pxList )
	{
		void *pxNextTCB = NULL;
		void *pxFirstTCB = NULL;

		if( listCURRENT_LIST_LENGTH( pxList ) > ( UBaseType_t ) 0 )
		{
			listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
			do
			{
				if( *uxTask >= uxArraySize ) {
					break;
				}

				listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
				prvTaskGetSnapshot( pxTaskSnapshotArray, uxTask, pxNextTCB );
			} while( pxNextTCB != pxFirstTCB );
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}

	UBaseType_t uxTaskGetSnapshotAll( TaskSnapshot_t * const pxTaskSnapshotArray, const UBaseType_t uxArraySize, UBaseType_t * const pxTcbSz )
	{
		UBaseType_t uxTask = 0;
		UBaseType_t i = 0;


		*pxTcbSz = pxTCBGetSize();
		/* Fill in an TaskStatus_t structure with information on each
		task in the Ready state. */
		i = configMAX_PRIORITIES;
		do
		{
			i--;
			prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, pxListGetReadyTask(i) );
		} while( i > ( UBaseType_t ) tskIDLE_PRIORITY ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */

		/* Fill in an TaskStatus_t structure with information on each
		task in the Blocked state. */
		prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, pxGetDelayedTaskList() );
		prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, pxGetOverflowDelayedTaskList() );
		for (i = 0; i < configNUM_CORES; i++) {
			if( uxTask >= uxArraySize ) {
				break;
			}
			prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, pxListGetReadyPendingTask(i) );
		}

		#if( INCLUDE_vTaskDelete == 1 )
		{
			prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, pxGetTasksWaitingTermination() );
		}
		#endif

		#if ( INCLUDE_vTaskSuspend == 1 )
		{
			prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, pxGetSuspendedTaskList() );
		}
		#endif
		return uxTask;
	}

	static void *prvFirstTaskGet( List_t *pxList )
	{
		ListItem_t *pxListItem = listGET_HEAD_ENTRY( pxList );
		if( pxListItem != listGET_END_MARKER( pxList ) ) {
			return listGET_LIST_ITEM_OWNER( pxListItem );
		}
		return NULL;
	}

	static void *prvNextTaskGet( void *pxTCB )
	{
		List_t *pxList = listLIST_ITEM_CONTAINER( pxTCBGetStateListItem(pxTCB) );
		ListItem_t *pxListItem = listGET_NEXT( pxTCBGetStateListItem(pxTCB) );
		if( pxListItem != listGET_END_MARKER( pxList ) ) {
			return listGET_LIST_ITEM_OWNER( pxListItem );
		}
		return NULL;
	}

	void vTaskGetSnapshot( TaskHandle_t pxTask, TaskSnapshot_t *pxTaskSnapshot )
	{
		configASSERT( portVALID_TCB_MEM(pxTask) );
		configASSERT( pxTaskSnapshot != NULL );
		pxTaskSnapshot->pxTCB = (void*) pxTask;
		pxTaskSnapshot->pxTopOfStack = pxTCBGetTopOfStack((void*) pxTask);
		pxTaskSnapshot->pxEndOfStack = pxTCBGetEndOfStack((void*) pxTask);
	}

	TaskHandle_t pxTaskGetNext( TaskHandle_t pxTask )
	{
		void *pxTCB = pxTask;
		List_t *pxTaskList = NULL;
		UBaseType_t i = configMAX_PRIORITIES;
		UBaseType_t bCurTaskListFound = pdFALSE;
		List_t *task_lists[] = {
			pxGetDelayedTaskList(),
			pxGetOverflowDelayedTaskList(),
		#if( INCLUDE_vTaskDelete == 1 )
			pxGetTasksWaitingTermination(),
		#endif
		#if( INCLUDE_vTaskSuspend == 1 )
			pxGetSuspendedTaskList()
		#endif
		};

		if( pxTask != NULL && !portVALID_TCB_MEM(pxTask) ) {
			return NULL;
		}

		if( pxTCB != NULL ) {
			pxTCB = prvNextTaskGet( pxTCB );
			if( pxTCB != NULL ) {
				// take care not to return garbage
				return portVALID_TCB_MEM(pxTCB) ? pxTCB : NULL;
			}
			pxTaskList = listLIST_ITEM_CONTAINER( pxTCBGetStateListItem(pxTask) );
		}
		/* ready tasks lists */
		do
		{
			i--;
			List_t *pxList = pxListGetReadyTask(i);
			if( bCurTaskListFound == pdFALSE && pxTaskList != NULL ) {
				/* need to find list the current task item from */
				if( pxTaskList == pxList ) {
					bCurTaskListFound = pdTRUE;
				}
				continue; /* go to the next 'ready list' */
			}
			pxTCB = prvFirstTaskGet( pxList );
			if( pxTCB != NULL ) {
				// take care not to return garbage
				return portVALID_TCB_MEM(pxTCB) ? pxTCB : NULL;
		}
		}
		while( i > tskIDLE_PRIORITY );
		/* pending ready tasks lists */
		for (i = 0; i < configNUM_CORES; i++) {
			List_t *pxList = pxListGetReadyPendingTask(i);
			if( bCurTaskListFound == pdFALSE && pxTaskList != NULL ) {
				/* need to find list the current task item from */
				if( pxTaskList == pxList ) {
					bCurTaskListFound = pdTRUE;
				}
				continue; /* go to the next 'ready list' */
			}
			pxTCB = prvFirstTaskGet( pxList );
			if( pxTCB != NULL ) {
				// take care not to return garbage
				return portVALID_TCB_MEM(pxTCB) ? pxTCB : NULL;
			}
		}
		/* other tasks lists */
		for (i = 0; i < DIM(task_lists); i++) {
			List_t *pxList = task_lists[ i ];
			if( bCurTaskListFound == pdFALSE && pxTaskList != NULL ) {
				/* need to find list the current task item from */
				if( pxTaskList == pxList ) {
					bCurTaskListFound = pdTRUE;
				}
				continue; /* go to the next 'ready list' */
			}
			pxTCB = prvFirstTaskGet( pxList );
			if( pxTCB != NULL ) {
				// take care not to return garbage
				return portVALID_TCB_MEM(pxTCB) ? pxTCB : NULL;
			}
		}

		return NULL;
	}

#endif
