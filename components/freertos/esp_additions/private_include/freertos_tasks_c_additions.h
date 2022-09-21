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

#include "idf_additions_inc.h"

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

#if ( INCLUDE_vTaskPrioritySet == 1 )

void prvTaskPriorityRaise( prvTaskSavedPriority_t * pxSavedPriority, UBaseType_t uxNewPriority )
{
    TCB_t * pxTCB;
    UBaseType_t uxPriorityUsedOnEntry;

    configASSERT( ( uxNewPriority < configMAX_PRIORITIES ) );

    /* Ensure the new priority is valid. */
    if( uxNewPriority >= ( UBaseType_t ) configMAX_PRIORITIES )
    {
        uxNewPriority = ( UBaseType_t ) configMAX_PRIORITIES - ( UBaseType_t ) 1U;
    }

    taskENTER_CRITICAL();
    {
        pxTCB = prvGetTCBFromHandle( NULL );

        #if ( configUSE_MUTEXES == 1 )
        {
            pxSavedPriority->uxPriority = pxTCB->uxPriority;
            pxSavedPriority->uxBasePriority = pxTCB->uxBasePriority;

            /* If uxNewPriority < uxBasePriority, then there is nothing else to
             * do, as uxBasePriority is always <= uxPriority. */
            if( uxNewPriority > pxTCB->uxBasePriority )
            {
                pxTCB->uxBasePriority = uxNewPriority;

                /* Remember the task's current priority before attempting to
                 * change it. If the task's current priority is changed, it must
                 * be done so before moving the task between task lists) in order
                 * for the taskRESET_READY_PRIORITY() macro to function correctly. */
                uxPriorityUsedOnEntry = pxTCB->uxPriority;

                if( uxNewPriority > pxTCB->uxPriority )
                {
                    pxTCB->uxPriority = uxNewPriority;

                    /* Only reset the event list item value if the value is not
                     * being used for anything else. */
                    if( ( listGET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ) ) & taskEVENT_LIST_ITEM_VALUE_IN_USE ) == 0UL )
                    {
                        listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), ( ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) uxNewPriority ) ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
                    }

                    /* If the task is in the blocked or suspended list we need do
                     * nothing more than change its priority variable. However, if
                     * the task is in a ready list it needs to be removed and placed
                     * in the list appropriate to its new priority. */
                    if( listIS_CONTAINED_WITHIN( &( pxReadyTasksLists[ uxPriorityUsedOnEntry ] ), &( pxTCB->xStateListItem ) ) != pdFALSE )
                    {
                        /* The task is currently in its ready list - remove before
                         * adding it to its new ready list.  As we are in a critical
                         * section we can do this even if the scheduler is suspended. */
                        if( uxListRemove( &( pxTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
                        {
                            /* It is known that the task is in its ready list so
                             * there is no need to check again and the port level
                             * reset macro can be called directly. */
                            portRESET_READY_PRIORITY( uxPriorityUsedOnEntry, uxTopReadyPriority );
                        }
                        prvAddTaskToReadyList( pxTCB );
                    }
                }
            }
        }
        #else /* if ( configUSE_MUTEXES == 1 ) */
        {
            pxSavedPriority->uxPriority = pxTCB->uxPriority;
            if ( uxNewPriority > pxTCB->uxPriority)
            {
                vTaskPrioritySet( NULL, uxNewPriority );
            }
        }
        #endif
    }
    taskEXIT_CRITICAL();
}

void prvTaskPriorityRestore( prvTaskSavedPriority_t * pxSavedPriority )
{
    TCB_t * pxTCB;
    UBaseType_t uxNewPriority;
    UBaseType_t uxPriorityUsedOnEntry;
    UBaseType_t uxBasePriorityUsedOnEntry;
    BaseType_t xYieldRequired = pdFALSE;

    taskENTER_CRITICAL();
    {
        pxTCB = prvGetTCBFromHandle( NULL );

        #if ( configUSE_MUTEXES == 1 )
        {
            /* If the saved uxBasePriority == the task's uxBasePriority, it means
             * that prvTaskPriorityRaise() never raised the task's uxBasePriority.
             * In that case, there is nothing else to do. */
            if( pxSavedPriority->uxBasePriority != pxTCB->uxBasePriority )
            {
                uxBasePriorityUsedOnEntry = pxTCB->uxBasePriority;
                pxTCB->uxBasePriority = pxSavedPriority->uxBasePriority;

                /* Remember the task's current priority before attempting to
                 * change it. If the task's current priority is changed, it must
                 * be done so before moving the task between task lists in order
                 * for the taskRESET_READY_PRIORITY() macro to function correctly. */
                uxPriorityUsedOnEntry = pxTCB->uxPriority;

                /* Check if the task inherited a priority after prvTaskPriorityRaise().
                 * If this is the case, there is nothing else to do. The priority
                 * will be restored when the task disinherits its priority. */
                if( pxTCB->uxPriority == uxBasePriorityUsedOnEntry )
                {
                    if( pxTCB->uxMutexesHeld == 0 )
                    {
                        /* The task may have inherited a priority before prvTaskPriorityRaise()
                         * then disinherited a priority after prvTaskPriorityRaise().
                         * Thus we need set the uxPriority to the saved base priority
                         * so that the task's priority gets restored to the priority
                         * before any inheritance or raising. */
                        pxTCB->uxPriority = pxSavedPriority->uxBasePriority;
                    }
                    else
                    {
                        /* The task may have inherited a priority before prvTaskPriorityRaise()
                         * was called. Thus, we need to restore uxPriority to the
                         * "saved uxPriority" so that the task still retains that
                         * inherited priority. */
                        pxTCB->uxPriority = pxSavedPriority->uxPriority;
                    }
                    uxNewPriority = pxTCB->uxPriority;

                    if( uxNewPriority < uxPriorityUsedOnEntry )
                    {
                        /* Setting the priority of the running task down means
                        * there may now be another task of higher priority that
                        * is ready to execute. */
                        xYieldRequired = pdTRUE;
                    }

                    /* Only reset the event list item value if the value is not
                     * being used for anything else. */
                    if( ( listGET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ) ) & taskEVENT_LIST_ITEM_VALUE_IN_USE ) == 0UL )
                    {
                        listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), ( ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) uxNewPriority ) ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
                    }

                    /* If the task is in the blocked or suspended list we need do
                     * nothing more than change its priority variable. However, if
                     * the task is in a ready list it needs to be removed and placed
                     * in the list appropriate to its new priority. */
                    if( listIS_CONTAINED_WITHIN( &( pxReadyTasksLists[ uxPriorityUsedOnEntry ] ), &( pxTCB->xStateListItem ) ) != pdFALSE )
                    {
                        /* The task is currently in its ready list - remove before
                         * adding it to its new ready list.  As we are in a critical
                         * section we can do this even if the scheduler is suspended. */
                        if( uxListRemove( &( pxTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
                        {
                            /* It is known that the task is in its ready list so
                             * there is no need to check again and the port level
                             * reset macro can be called directly. */
                            portRESET_READY_PRIORITY( uxPriorityUsedOnEntry, uxTopReadyPriority );
                        }
                        prvAddTaskToReadyList( pxTCB );
                    }

                    if( xYieldRequired != pdFALSE )
                    {
                        taskYIELD_IF_USING_PREEMPTION();
                    }
                }
            }
        }
        #else /* if ( configUSE_MUTEXES == 1 ) */
        {
            vTaskPrioritySet( NULL, pxSavedPriority->uxPriority );
        }
        #endif
    }
    taskEXIT_CRITICAL();
}

#endif // ( INCLUDE_vTaskPrioritySet == 1 )
