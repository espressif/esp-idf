/*
 * FreeRTOS Kernel V11.0.1
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-FileCopyrightText: 2021 Amazon.com, Inc. or its affiliates
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef MPU_WRAPPERS_H
#define MPU_WRAPPERS_H

/* This file redefines API functions to be called through a wrapper macro, but
 * only for ports that are using the MPU. */
#if ( portUSING_MPU_WRAPPERS == 1 )

/* MPU_WRAPPERS_INCLUDED_FROM_API_FILE will be defined when this file is
 * included from queue.c or task.c to prevent it from having an effect within
 * those files. */
    #ifndef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/*
 * Map standard (non MPU) API functions to equivalents that start
 * "MPU_".  This will cause the application code to call the MPU_
 * version, which wraps the non-MPU version with privilege promoting
 * then demoting code, so the kernel code always runs will full
 * privileges.
 */

/* Map standard task.h API functions to the MPU equivalents. */
        #define vTaskDelay                            MPU_vTaskDelay
        #define xTaskDelayUntil                       MPU_xTaskDelayUntil
        #define xTaskAbortDelay                       MPU_xTaskAbortDelay
        #define uxTaskPriorityGet                     MPU_uxTaskPriorityGet
        #define eTaskGetState                         MPU_eTaskGetState
        #define vTaskGetInfo                          MPU_vTaskGetInfo
        #define vTaskSuspend                          MPU_vTaskSuspend
        #define vTaskResume                           MPU_vTaskResume
        #define xTaskGetTickCount                     MPU_xTaskGetTickCount
        #define uxTaskGetNumberOfTasks                MPU_uxTaskGetNumberOfTasks
        #define uxTaskGetStackHighWaterMark           MPU_uxTaskGetStackHighWaterMark
        #define uxTaskGetStackHighWaterMark2          MPU_uxTaskGetStackHighWaterMark2
        #define vTaskSetApplicationTaskTag            MPU_vTaskSetApplicationTaskTag
        #define xTaskGetApplicationTaskTag            MPU_xTaskGetApplicationTaskTag
        #define vTaskSetThreadLocalStoragePointer     MPU_vTaskSetThreadLocalStoragePointer
        #define pvTaskGetThreadLocalStoragePointer    MPU_pvTaskGetThreadLocalStoragePointer
        #define xTaskGetIdleTaskHandle                MPU_xTaskGetIdleTaskHandle
        #define uxTaskGetSystemState                  MPU_uxTaskGetSystemState
        #define ulTaskGetIdleRunTimeCounter           MPU_ulTaskGetIdleRunTimeCounter
        #define ulTaskGetIdleRunTimePercent           MPU_ulTaskGetIdleRunTimePercent
        #define xTaskGenericNotify                    MPU_xTaskGenericNotify
        #define xTaskGenericNotifyWait                MPU_xTaskGenericNotifyWait
        #define ulTaskGenericNotifyTake               MPU_ulTaskGenericNotifyTake
        #define xTaskGenericNotifyStateClear          MPU_xTaskGenericNotifyStateClear
        #define ulTaskGenericNotifyValueClear         MPU_ulTaskGenericNotifyValueClear
        #define vTaskSetTimeOutState                  MPU_vTaskSetTimeOutState
        #define xTaskCheckForTimeOut                  MPU_xTaskCheckForTimeOut
        #define xTaskGetCurrentTaskHandle             MPU_xTaskGetCurrentTaskHandle
        #define xTaskGetSchedulerState                MPU_xTaskGetSchedulerState

        #if ( configUSE_MPU_WRAPPERS_V1 == 0 )
            #define ulTaskGetRunTimeCounter           MPU_ulTaskGetRunTimeCounter
            #define ulTaskGetRunTimePercent           MPU_ulTaskGetRunTimePercent
        #endif /* #if ( configUSE_MPU_WRAPPERS_V1 == 0 ) */

/* Privileged only wrappers for Task APIs. These are needed so that
 * the application can use opaque handles maintained in mpu_wrappers.c
 * with all the APIs. */
        #define xTaskCreate                              MPU_xTaskCreate
        #define xTaskCreateStatic                        MPU_xTaskCreateStatic
        #define vTaskDelete                              MPU_vTaskDelete
        #define vTaskPrioritySet                         MPU_vTaskPrioritySet
        #define xTaskGetHandle                           MPU_xTaskGetHandle
        #define xTaskCallApplicationTaskHook             MPU_xTaskCallApplicationTaskHook

        #if ( configUSE_MPU_WRAPPERS_V1 == 0 )
            #define pcTaskGetName                        MPU_pcTaskGetName
            #define xTaskCreateRestricted                MPU_xTaskCreateRestricted
            #define xTaskCreateRestrictedStatic          MPU_xTaskCreateRestrictedStatic
            #define vTaskAllocateMPURegions              MPU_vTaskAllocateMPURegions
            #define xTaskGetStaticBuffers                MPU_xTaskGetStaticBuffers
            #define uxTaskPriorityGetFromISR             MPU_uxTaskPriorityGetFromISR
            #define uxTaskBasePriorityGet                MPU_uxTaskBasePriorityGet
            #define uxTaskBasePriorityGetFromISR         MPU_uxTaskBasePriorityGetFromISR
            #define xTaskResumeFromISR                   MPU_xTaskResumeFromISR
            #define xTaskGetApplicationTaskTagFromISR    MPU_xTaskGetApplicationTaskTagFromISR
            #define xTaskGenericNotifyFromISR            MPU_xTaskGenericNotifyFromISR
            #define vTaskGenericNotifyGiveFromISR        MPU_vTaskGenericNotifyGiveFromISR
        #endif /* #if ( configUSE_MPU_WRAPPERS_V1 == 0 ) */

/* Map standard queue.h API functions to the MPU equivalents. */
        #define xQueueGenericSend            MPU_xQueueGenericSend
        #define xQueueReceive                MPU_xQueueReceive
        #define xQueuePeek                   MPU_xQueuePeek
        #define xQueueSemaphoreTake          MPU_xQueueSemaphoreTake
        #define uxQueueMessagesWaiting       MPU_uxQueueMessagesWaiting
        #define uxQueueSpacesAvailable       MPU_uxQueueSpacesAvailable
        #define xQueueGetMutexHolder         MPU_xQueueGetMutexHolder
        #define xQueueTakeMutexRecursive     MPU_xQueueTakeMutexRecursive
        #define xQueueGiveMutexRecursive     MPU_xQueueGiveMutexRecursive
        #define xQueueAddToSet               MPU_xQueueAddToSet
        #define xQueueSelectFromSet          MPU_xQueueSelectFromSet

        #if ( configQUEUE_REGISTRY_SIZE > 0 )
            #define vQueueAddToRegistry      MPU_vQueueAddToRegistry
            #define vQueueUnregisterQueue    MPU_vQueueUnregisterQueue
            #define pcQueueGetName           MPU_pcQueueGetName
        #endif /* #if ( configQUEUE_REGISTRY_SIZE > 0 ) */

/* Privileged only wrappers for Queue APIs. These are needed so that
 * the application can use opaque handles maintained in mpu_wrappers.c
 * with all the APIs. */
        #define vQueueDelete                           MPU_vQueueDelete
        #define xQueueCreateMutex                      MPU_xQueueCreateMutex
        #define xQueueCreateMutexStatic                MPU_xQueueCreateMutexStatic
        #define xQueueCreateCountingSemaphore          MPU_xQueueCreateCountingSemaphore
        #define xQueueCreateCountingSemaphoreStatic    MPU_xQueueCreateCountingSemaphoreStatic
        #define xQueueGenericCreate                    MPU_xQueueGenericCreate
        #define xQueueGenericCreateStatic              MPU_xQueueGenericCreateStatic
        #define xQueueGenericReset                     MPU_xQueueGenericReset
        #define xQueueCreateSet                        MPU_xQueueCreateSet
        #define xQueueRemoveFromSet                    MPU_xQueueRemoveFromSet

        #if ( configUSE_MPU_WRAPPERS_V1 == 0 )
            #define xQueueGenericGetStaticBuffers      MPU_xQueueGenericGetStaticBuffers
            #define xQueueGenericSendFromISR           MPU_xQueueGenericSendFromISR
            #define xQueueGiveFromISR                  MPU_xQueueGiveFromISR
            #define xQueuePeekFromISR                  MPU_xQueuePeekFromISR
            #define xQueueReceiveFromISR               MPU_xQueueReceiveFromISR
            #define xQueueIsQueueEmptyFromISR          MPU_xQueueIsQueueEmptyFromISR
            #define xQueueIsQueueFullFromISR           MPU_xQueueIsQueueFullFromISR
            #define uxQueueMessagesWaitingFromISR      MPU_uxQueueMessagesWaitingFromISR
            #define xQueueGetMutexHolderFromISR        MPU_xQueueGetMutexHolderFromISR
            #define xQueueSelectFromSetFromISR         MPU_xQueueSelectFromSetFromISR
        #endif /* if ( configUSE_MPU_WRAPPERS_V1 == 0 ) */

/* Map standard timer.h API functions to the MPU equivalents. */
        #define pvTimerGetTimerID                 MPU_pvTimerGetTimerID
        #define vTimerSetTimerID                  MPU_vTimerSetTimerID
        #define xTimerIsTimerActive               MPU_xTimerIsTimerActive
        #define xTimerGetTimerDaemonTaskHandle    MPU_xTimerGetTimerDaemonTaskHandle
        #define xTimerGenericCommandFromTask      MPU_xTimerGenericCommandFromTask
        #define pcTimerGetName                    MPU_pcTimerGetName
        #define vTimerSetReloadMode               MPU_vTimerSetReloadMode
        #define uxTimerGetReloadMode              MPU_uxTimerGetReloadMode
        #define xTimerGetPeriod                   MPU_xTimerGetPeriod
        #define xTimerGetExpiryTime               MPU_xTimerGetExpiryTime

/* Privileged only wrappers for Timer APIs. These are needed so that
 * the application can use opaque handles maintained in mpu_wrappers.c
 * with all the APIs. */
        #if ( configUSE_MPU_WRAPPERS_V1 == 0 )
            #define xTimerGetReloadMode            MPU_xTimerGetReloadMode
            #define xTimerCreate                   MPU_xTimerCreate
            #define xTimerCreateStatic             MPU_xTimerCreateStatic
            #define xTimerGetStaticBuffer          MPU_xTimerGetStaticBuffer
            #define xTimerGenericCommandFromISR    MPU_xTimerGenericCommandFromISR
        #endif /* #if ( configUSE_MPU_WRAPPERS_V1 == 0 ) */

/* Map standard event_group.h API functions to the MPU equivalents. */
        #define xEventGroupWaitBits          MPU_xEventGroupWaitBits
        #define xEventGroupClearBits         MPU_xEventGroupClearBits
        #define xEventGroupSetBits           MPU_xEventGroupSetBits
        #define xEventGroupSync              MPU_xEventGroupSync

        #if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_MPU_WRAPPERS_V1 == 0 ) )
            #define uxEventGroupGetNumber    MPU_uxEventGroupGetNumber
            #define vEventGroupSetNumber     MPU_vEventGroupSetNumber
        #endif /* #if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_MPU_WRAPPERS_V1 == 0 ) ) */

/* Privileged only wrappers for Event Group APIs. These are needed so that
 * the application can use opaque handles maintained in mpu_wrappers.c
 * with all the APIs. */
        #define xEventGroupCreate                  MPU_xEventGroupCreate
        #define xEventGroupCreateStatic            MPU_xEventGroupCreateStatic
        #define vEventGroupDelete                  MPU_vEventGroupDelete

        #if ( configUSE_MPU_WRAPPERS_V1 == 0 )
            #define xEventGroupGetStaticBuffer     MPU_xEventGroupGetStaticBuffer
            #define xEventGroupClearBitsFromISR    MPU_xEventGroupClearBitsFromISR
            #define xEventGroupSetBitsFromISR      MPU_xEventGroupSetBitsFromISR
            #define xEventGroupGetBitsFromISR      MPU_xEventGroupGetBitsFromISR
        #endif /* #if ( configUSE_MPU_WRAPPERS_V1 == 0 ) */

/* Map standard message/stream_buffer.h API functions to the MPU
 * equivalents. */
        #define xStreamBufferSend                      MPU_xStreamBufferSend
        #define xStreamBufferReceive                   MPU_xStreamBufferReceive
        #define xStreamBufferIsFull                    MPU_xStreamBufferIsFull
        #define xStreamBufferIsEmpty                   MPU_xStreamBufferIsEmpty
        #define xStreamBufferSpacesAvailable           MPU_xStreamBufferSpacesAvailable
        #define xStreamBufferBytesAvailable            MPU_xStreamBufferBytesAvailable
        #define xStreamBufferSetTriggerLevel           MPU_xStreamBufferSetTriggerLevel
        #define xStreamBufferNextMessageLengthBytes    MPU_xStreamBufferNextMessageLengthBytes

/* Privileged only wrappers for Stream Buffer APIs. These are needed so that
 * the application can use opaque handles maintained in mpu_wrappers.c
 * with all the APIs. */

        #define xStreamBufferGenericCreate                  MPU_xStreamBufferGenericCreate
        #define xStreamBufferGenericCreateStatic            MPU_xStreamBufferGenericCreateStatic
        #define vStreamBufferDelete                         MPU_vStreamBufferDelete
        #define xStreamBufferReset                          MPU_xStreamBufferReset

        #if ( configUSE_MPU_WRAPPERS_V1 == 0 )
            #define xStreamBufferGetStaticBuffers           MPU_xStreamBufferGetStaticBuffers
            #define xStreamBufferSendFromISR                MPU_xStreamBufferSendFromISR
            #define xStreamBufferReceiveFromISR             MPU_xStreamBufferReceiveFromISR
            #define xStreamBufferSendCompletedFromISR       MPU_xStreamBufferSendCompletedFromISR
            #define xStreamBufferReceiveCompletedFromISR    MPU_xStreamBufferReceiveCompletedFromISR
        #endif /* #if ( configUSE_MPU_WRAPPERS_V1 == 0 ) */

/* Remove the privileged function macro, but keep the PRIVILEGED_DATA
 * macro so applications can place data in privileged access sections
 * (useful when using statically allocated objects). */
        #define PRIVILEGED_FUNCTION
        #define PRIVILEGED_DATA    __attribute__( ( section( "privileged_data" ) ) )
        #define FREERTOS_SYSTEM_CALL


        #if ( ( configUSE_MPU_WRAPPERS_V1 == 0 ) && ( configENABLE_ACCESS_CONTROL_LIST == 1 ) )

            #define vGrantAccessToTask( xTask, xTaskToGrantAccess )                        vGrantAccessToKernelObject( ( xTask ), ( int32_t ) ( xTaskToGrantAccess ) )
            #define vRevokeAccessToTask( xTask, xTaskToRevokeAccess )                      vRevokeAccessToKernelObject( ( xTask ), ( int32_t ) ( xTaskToRevokeAccess ) )

            #define vGrantAccessToSemaphore( xTask, xSemaphoreToGrantAccess )              vGrantAccessToKernelObject( ( xTask ), ( int32_t ) ( xSemaphoreToGrantAccess ) )
            #define vRevokeAccessToSemaphore( xTask, xSemaphoreToRevokeAccess )            vRevokeAccessToKernelObject( ( xTask ), ( int32_t ) ( xSemaphoreToRevokeAccess ) )

            #define vGrantAccessToQueue( xTask, xQueueToGrantAccess )                      vGrantAccessToKernelObject( ( xTask ), ( int32_t ) ( xQueueToGrantAccess ) )
            #define vRevokeAccessToQueue( xTask, xQueueToRevokeAccess )                    vRevokeAccessToKernelObject( ( xTask ), ( int32_t ) ( xQueueToRevokeAccess ) )

            #define vGrantAccessToQueueSet( xTask, xQueueSetToGrantAccess )                vGrantAccessToKernelObject( ( xTask ), ( int32_t ) ( xQueueSetToGrantAccess ) )
            #define vRevokeAccessToQueueSet( xTask, xQueueSetToRevokeAccess )              vRevokeAccessToKernelObject( ( xTask ), ( int32_t ) ( xQueueSetToRevokeAccess ) )

            #define vGrantAccessToEventGroup( xTask, xEventGroupToGrantAccess )            vGrantAccessToKernelObject( ( xTask ), ( int32_t ) ( xEventGroupToGrantAccess ) )
            #define vRevokeAccessToEventGroup( xTask, xEventGroupToRevokeAccess )          vRevokeAccessToKernelObject( ( xTask ), ( int32_t ) ( xEventGroupToRevokeAccess ) )

            #define vGrantAccessToStreamBuffer( xTask, xStreamBufferToGrantAccess )        vGrantAccessToKernelObject( ( xTask ), ( int32_t ) ( xStreamBufferToGrantAccess ) )
            #define vRevokeAccessToStreamBuffer( xTask, xStreamBufferToRevokeAccess )      vRevokeAccessToKernelObject( ( xTask ), ( int32_t ) ( xStreamBufferToRevokeAccess ) )

            #define vGrantAccessToMessageBuffer( xTask, xMessageBufferToGrantAccess )      vGrantAccessToKernelObject( ( xTask ), ( int32_t ) ( xMessageBufferToGrantAccess ) )
            #define vRevokeAccessToMessageBuffer( xTask, xMessageBufferToRevokeAccess )    vRevokeAccessToKernelObject( ( xTask ), ( int32_t ) ( xMessageBufferToRevokeAccess ) )

            #define vGrantAccessToTimer( xTask, xTimerToGrantAccess )                      vGrantAccessToKernelObject( ( xTask ), ( int32_t ) ( xTimerToGrantAccess ) )
            #define vRevokeAccessToTimer( xTask, xTimerToRevokeAccess )                    vRevokeAccessToKernelObject( ( xTask ), ( int32_t ) ( xTimerToRevokeAccess ) )

        #endif /* #if ( ( configUSE_MPU_WRAPPERS_V1 == 0 ) && ( configENABLE_ACCESS_CONTROL_LIST == 1 ) ) */

    #else /* MPU_WRAPPERS_INCLUDED_FROM_API_FILE */

/* Ensure API functions go in the privileged execution section. */
        #define PRIVILEGED_FUNCTION     __attribute__( ( section( "privileged_functions" ) ) )
        #define PRIVILEGED_DATA         __attribute__( ( section( "privileged_data" ) ) )
        #define FREERTOS_SYSTEM_CALL    __attribute__( ( section( "freertos_system_calls" ) ) )

    #endif /* MPU_WRAPPERS_INCLUDED_FROM_API_FILE */

#else /* portUSING_MPU_WRAPPERS */

    #define PRIVILEGED_FUNCTION
    #define PRIVILEGED_DATA
    #define FREERTOS_SYSTEM_CALL

#endif /* portUSING_MPU_WRAPPERS */


#endif /* MPU_WRAPPERS_H */
