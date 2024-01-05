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

#ifndef MPU_SYSCALL_NUMBERS_H
#define MPU_SYSCALL_NUMBERS_H

/* Numbers assigned to various system calls. */
#define SYSTEM_CALL_xTaskGenericNotify                     0
#define SYSTEM_CALL_xTaskGenericNotifyWait                 1
#define SYSTEM_CALL_xTimerGenericCommandFromTask           2
#define SYSTEM_CALL_xEventGroupWaitBits                    3
#define SYSTEM_CALL_xTaskDelayUntil                        4
#define SYSTEM_CALL_xTaskAbortDelay                        5
#define SYSTEM_CALL_vTaskDelay                             6
#define SYSTEM_CALL_uxTaskPriorityGet                      7
#define SYSTEM_CALL_eTaskGetState                          8
#define SYSTEM_CALL_vTaskGetInfo                           9
#define SYSTEM_CALL_xTaskGetIdleTaskHandle                 10
#define SYSTEM_CALL_vTaskSuspend                           11
#define SYSTEM_CALL_vTaskResume                            12
#define SYSTEM_CALL_xTaskGetTickCount                      13
#define SYSTEM_CALL_uxTaskGetNumberOfTasks                 14
#define SYSTEM_CALL_ulTaskGetRunTimeCounter                15
#define SYSTEM_CALL_ulTaskGetRunTimePercent                16
#define SYSTEM_CALL_ulTaskGetIdleRunTimePercent            17
#define SYSTEM_CALL_ulTaskGetIdleRunTimeCounter            18
#define SYSTEM_CALL_vTaskSetApplicationTaskTag             19
#define SYSTEM_CALL_xTaskGetApplicationTaskTag             20
#define SYSTEM_CALL_vTaskSetThreadLocalStoragePointer      21
#define SYSTEM_CALL_pvTaskGetThreadLocalStoragePointer     22
#define SYSTEM_CALL_uxTaskGetSystemState                   23
#define SYSTEM_CALL_uxTaskGetStackHighWaterMark            24
#define SYSTEM_CALL_uxTaskGetStackHighWaterMark2           25
#define SYSTEM_CALL_xTaskGetCurrentTaskHandle              26
#define SYSTEM_CALL_xTaskGetSchedulerState                 27
#define SYSTEM_CALL_vTaskSetTimeOutState                   28
#define SYSTEM_CALL_xTaskCheckForTimeOut                   29
#define SYSTEM_CALL_ulTaskGenericNotifyTake                30
#define SYSTEM_CALL_xTaskGenericNotifyStateClear           31
#define SYSTEM_CALL_ulTaskGenericNotifyValueClear          32
#define SYSTEM_CALL_xQueueGenericSend                      33
#define SYSTEM_CALL_uxQueueMessagesWaiting                 34
#define SYSTEM_CALL_uxQueueSpacesAvailable                 35
#define SYSTEM_CALL_xQueueReceive                          36
#define SYSTEM_CALL_xQueuePeek                             37
#define SYSTEM_CALL_xQueueSemaphoreTake                    38
#define SYSTEM_CALL_xQueueGetMutexHolder                   39
#define SYSTEM_CALL_xQueueTakeMutexRecursive               40
#define SYSTEM_CALL_xQueueGiveMutexRecursive               41
#define SYSTEM_CALL_xQueueSelectFromSet                    42
#define SYSTEM_CALL_xQueueAddToSet                         43
#define SYSTEM_CALL_vQueueAddToRegistry                    44
#define SYSTEM_CALL_vQueueUnregisterQueue                  45
#define SYSTEM_CALL_pcQueueGetName                         46
#define SYSTEM_CALL_pvTimerGetTimerID                      47
#define SYSTEM_CALL_vTimerSetTimerID                       48
#define SYSTEM_CALL_xTimerIsTimerActive                    49
#define SYSTEM_CALL_xTimerGetTimerDaemonTaskHandle         50
#define SYSTEM_CALL_pcTimerGetName                         51
#define SYSTEM_CALL_vTimerSetReloadMode                    52
#define SYSTEM_CALL_xTimerGetReloadMode                    53
#define SYSTEM_CALL_uxTimerGetReloadMode                   54
#define SYSTEM_CALL_xTimerGetPeriod                        55
#define SYSTEM_CALL_xTimerGetExpiryTime                    56
#define SYSTEM_CALL_xEventGroupClearBits                   57
#define SYSTEM_CALL_xEventGroupSetBits                     58
#define SYSTEM_CALL_xEventGroupSync                        59
#define SYSTEM_CALL_uxEventGroupGetNumber                  60
#define SYSTEM_CALL_vEventGroupSetNumber                   61
#define SYSTEM_CALL_xStreamBufferSend                      62
#define SYSTEM_CALL_xStreamBufferReceive                   63
#define SYSTEM_CALL_xStreamBufferIsFull                    64
#define SYSTEM_CALL_xStreamBufferIsEmpty                   65
#define SYSTEM_CALL_xStreamBufferSpacesAvailable           66
#define SYSTEM_CALL_xStreamBufferBytesAvailable            67
#define SYSTEM_CALL_xStreamBufferSetTriggerLevel           68
#define SYSTEM_CALL_xStreamBufferNextMessageLengthBytes    69
#define NUM_SYSTEM_CALLS                                   70  /* Total number of system calls. */

#endif /* MPU_SYSCALL_NUMBERS_H */
