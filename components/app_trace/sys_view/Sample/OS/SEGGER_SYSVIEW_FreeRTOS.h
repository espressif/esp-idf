/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2019 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the SystemView and RTT protocol, and J-Link.       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: 3.10                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_FreeRTOS.h
Purpose : Interface between FreeRTOS and SystemView.
Revision: $Rev: 7745 $

Notes:
  (1) Include this file at the end of FreeRTOSConfig.h
*/

#ifndef SYSVIEW_FREERTOS_H
#define SYSVIEW_FREERTOS_H

#include "SEGGER_SYSVIEW.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#ifndef portSTACK_GROWTH
  #define portSTACK_GROWTH              ( -1 )
#endif

#define SYSVIEW_FREERTOS_MAX_NOF_TASKS  8

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/
#define apiID_OFFSET                              (32u)

#define apiID_VTASKALLOCATEMPUREGIONS             (1u)
#define apiID_VTASKDELETE                         (2u)
#define apiID_VTASKDELAY                          (3u)
#define apiID_VTASKDELAYUNTIL                     (4u)
#define apiID_UXTASKPRIORITYGET                   (5u)
#define apiID_UXTASKPRIORITYGETFROMISR            (6u)
#define apiID_ETASKGETSTATE                       (7u)
#define apiID_VTASKPRIORITYSET                    (8u)
#define apiID_VTASKSUSPEND                        (9u)
#define apiID_VTASKRESUME                         (10u)
#define apiID_XTASKRESUMEFROMISR                  (11u)
#define apiID_VTASKSTARTSCHEDULER                 (12u)
#define apiID_VTASKENDSCHEDULER                   (13u)
#define apiID_VTASKSUSPENDALL                     (14u)
#define apiID_XTASKRESUMEALL                      (15u)
#define apiID_XTASKGETTICKCOUNT                   (16u)
#define apiID_XTASKGETTICKCOUNTFROMISR            (17u)
#define apiID_UXTASKGETNUMBEROFTASKS              (18u)
#define apiID_PCTASKGETTASKNAME                   (19u)
#define apiID_UXTASKGETSTACKHIGHWATERMARK         (20u)
#define apiID_VTASKSETAPPLICATIONTASKTAG          (21u)
#define apiID_XTASKGETAPPLICATIONTASKTAG          (22u)
#define apiID_VTASKSETTHREADLOCALSTORAGEPOINTER   (23u)
#define apiID_PVTASKGETTHREADLOCALSTORAGEPOINTER  (24u)
#define apiID_XTASKCALLAPPLICATIONTASKHOOK        (25u)
#define apiID_XTASKGETIDLETASKHANDLE              (26u)
#define apiID_UXTASKGETSYSTEMSTATE                (27u)
#define apiID_VTASKLIST                           (28u)
#define apiID_VTASKGETRUNTIMESTATS                (29u)
#define apiID_XTASKGENERICNOTIFY                  (30u)
#define apiID_XTASKGENERICNOTIFYFROMISR           (31u)
#define apiID_XTASKNOTIFYWAIT                     (32u)
#define apiID_VTASKNOTIFYGIVEFROMISR              (33u)
#define apiID_ULTASKNOTIFYTAKE                    (34u)
#define apiID_XTASKNOTIFYSTATECLEAR               (35u)
#define apiID_XTASKGETCURRENTTASKHANDLE           (36u)
#define apiID_VTASKSETTIMEOUTSTATE                (37u)
#define apiID_XTASKCHECKFORTIMEOUT                (38u)
#define apiID_VTASKMISSEDYIELD                    (39u)
#define apiID_XTASKGETSCHEDULERSTATE              (40u)
#define apiID_VTASKPRIORITYINHERIT                (41u)
#define apiID_XTASKPRIORITYDISINHERIT             (42u)
#define apiID_XTASKGENERICCREATE                  (43u)
#define apiID_UXTASKGETTASKNUMBER                 (44u)
#define apiID_VTASKSETTASKNUMBER                  (45u)
#define apiID_VTASKSTEPTICK                       (46u)
#define apiID_ETASKCONFIRMSLEEPMODESTATUS         (47u)
#define apiID_XTIMERCREATE                        (48u)
#define apiID_PVTIMERGETTIMERID                   (49u)
#define apiID_VTIMERSETTIMERID                    (50u)
#define apiID_XTIMERISTIMERACTIVE                 (51u)
#define apiID_XTIMERGETTIMERDAEMONTASKHANDLE      (52u)
#define apiID_XTIMERPENDFUNCTIONCALLFROMISR       (53u)
#define apiID_XTIMERPENDFUNCTIONCALL              (54u)
#define apiID_PCTIMERGETTIMERNAME                 (55u)
#define apiID_XTIMERCREATETIMERTASK               (56u)
#define apiID_XTIMERGENERICCOMMAND                (57u)
#define apiID_XQUEUEGENERICSEND                   (58u)
#define apiID_XQUEUEPEEKFROMISR                   (59u)
#define apiID_XQUEUEGENERICRECEIVE                (60u)
#define apiID_UXQUEUEMESSAGESWAITING              (61u)
#define apiID_UXQUEUESPACESAVAILABLE              (62u)
#define apiID_VQUEUEDELETE                        (63u)
#define apiID_XQUEUEGENERICSENDFROMISR            (64u)
#define apiID_XQUEUEGIVEFROMISR                   (65u)
#define apiID_XQUEUERECEIVEFROMISR                (66u)
#define apiID_XQUEUEISQUEUEEMPTYFROMISR           (67u)
#define apiID_XQUEUEISQUEUEFULLFROMISR            (68u)
#define apiID_UXQUEUEMESSAGESWAITINGFROMISR       (69u)
#define apiID_XQUEUEALTGENERICSEND                (70u)
#define apiID_XQUEUEALTGENERICRECEIVE             (71u)
#define apiID_XQUEUECRSENDFROMISR                 (72u)
#define apiID_XQUEUECRRECEIVEFROMISR              (73u)
#define apiID_XQUEUECRSEND                        (74u)
#define apiID_XQUEUECRRECEIVE                     (75u)
#define apiID_XQUEUECREATEMUTEX                   (76u)
#define apiID_XQUEUECREATECOUNTINGSEMAPHORE       (77u)
#define apiID_XQUEUEGETMUTEXHOLDER                (78u)
#define apiID_XQUEUETAKEMUTEXRECURSIVE            (79u)
#define apiID_XQUEUEGIVEMUTEXRECURSIVE            (80u)
#define apiID_VQUEUEADDTOREGISTRY                 (81u)
#define apiID_VQUEUEUNREGISTERQUEUE               (82u)
#define apiID_XQUEUEGENERICCREATE                 (83u)
#define apiID_XQUEUECREATESET                     (84u)
#define apiID_XQUEUEADDTOSET                      (85u)
#define apiID_XQUEUEREMOVEFROMSET                 (86u)
#define apiID_XQUEUESELECTFROMSET                 (87u)
#define apiID_XQUEUESELECTFROMSETFROMISR          (88u)
#define apiID_XQUEUEGENERICRESET                  (89u)
#define apiID_VLISTINITIALISE                     (90u)
#define apiID_VLISTINITIALISEITEM                 (91u)
#define apiID_VLISTINSERT                         (92u)
#define apiID_VLISTINSERTEND                      (93u)
#define apiID_UXLISTREMOVE                        (94u)
#define apiID_XEVENTGROUPCREATE                   (95u)
#define apiID_XEVENTGROUPWAITBITS                 (96u)
#define apiID_XEVENTGROUPCLEARBITS                (97u)
#define apiID_XEVENTGROUPCLEARBITSFROMISR         (98u)
#define apiID_XEVENTGROUPSETBITS                  (99u)
#define apiID_XEVENTGROUPSETBITSFROMISR           (100u)
#define apiID_XEVENTGROUPSYNC                     (101u)
#define apiID_XEVENTGROUPGETBITSFROMISR           (102u)
#define apiID_VEVENTGROUPDELETE                   (103u)
#define apiID_UXEVENTGROUPGETNUMBER               (104u)
#define apiID_XSTREAMBUFFERCREATE                 (105u)
#define apiID_VSTREAMBUFFERDELETE                 (106u)
#define apiID_XSTREAMBUFFERRESET                  (107u)
#define apiID_XSTREAMBUFFERSEND                   (108u)
#define apiID_XSTREAMBUFFERSENDFROMISR            (109u)
#define apiID_XSTREAMBUFFERRECEIVE                (110u)
#define apiID_XSTREAMBUFFERRECEIVEFROMISR         (111u)

#define traceTASK_NOTIFY_TAKE()                                                 SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_ULTASKNOTIFYTAKE, xClearCountOnExit, xTicksToWait)
#define traceTASK_DELAY()                                                       SEGGER_SYSVIEW_RecordU32  (apiID_OFFSET + apiID_VTASKDELAY, xTicksToDelay)
#define traceTASK_DELAY_UNTIL(xTimeToWake)                                      SEGGER_SYSVIEW_RecordVoid (apiID_OFFSET + apiID_VTASKDELAYUNTIL)
#define traceTASK_NOTIFY_GIVE_FROM_ISR()                                        SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_VTASKNOTIFYGIVEFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB), (U32)pxHigherPriorityTaskWoken)
#define traceTASK_PRIORITY_INHERIT( pxTCB, uxPriority )                         SEGGER_SYSVIEW_RecordU32  (apiID_OFFSET + apiID_VTASKPRIORITYINHERIT, (U32)pxMutexHolder)
#define traceTASK_RESUME( pxTCB )                                               SEGGER_SYSVIEW_RecordU32  (apiID_OFFSET + apiID_VTASKRESUME, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB))
#define traceINCREASE_TICK_COUNT( xTicksToJump )                                SEGGER_SYSVIEW_RecordU32  (apiID_OFFSET + apiID_VTASKSTEPTICK, xTicksToJump)
#define traceTASK_SUSPEND( pxTCB )                                              SEGGER_SYSVIEW_RecordU32  (apiID_OFFSET + apiID_VTASKSUSPEND, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB))
#define traceTASK_PRIORITY_DISINHERIT( pxTCB, uxBasePriority )                  SEGGER_SYSVIEW_RecordU32  (apiID_OFFSET + apiID_XTASKPRIORITYDISINHERIT, (U32)pxMutexHolder)
#define traceTASK_RESUME_FROM_ISR( pxTCB )                                      SEGGER_SYSVIEW_RecordU32  (apiID_OFFSET + apiID_XTASKRESUMEFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB))
#define traceTASK_NOTIFY()                                                      SEGGER_SYSVIEW_RecordU32x4(apiID_OFFSET + apiID_XTASKGENERICNOTIFY, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB), ulValue, eAction, (U32)pulPreviousNotificationValue)
#define traceTASK_NOTIFY_FROM_ISR()                                             SEGGER_SYSVIEW_RecordU32x5(apiID_OFFSET + apiID_XTASKGENERICNOTIFYFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB), ulValue, eAction, (U32)pulPreviousNotificationValue, (U32)pxHigherPriorityTaskWoken)
#define traceTASK_NOTIFY_WAIT()                                                 SEGGER_SYSVIEW_RecordU32x4(apiID_OFFSET + apiID_XTASKNOTIFYWAIT, ulBitsToClearOnEntry, ulBitsToClearOnExit, (U32)pulNotificationValue, xTicksToWait)

#define traceQUEUE_CREATE( pxNewQueue )                                         SEGGER_SYSVIEW_RecordU32x3(apiID_OFFSET + apiID_XQUEUEGENERICCREATE, uxQueueLength, uxItemSize, ucQueueType)
#define traceQUEUE_DELETE( pxQueue )                                            SEGGER_SYSVIEW_RecordU32  (apiID_OFFSET + apiID_VQUEUEDELETE, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue))
#define traceQUEUE_PEEK( pxQueue )                                              SEGGER_SYSVIEW_RecordU32x4(apiID_OFFSET + apiID_XQUEUEGENERICRECEIVE, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer), xTicksToWait, 1)
#define traceQUEUE_PEEK_FROM_ISR( pxQueue )                                     SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XQUEUEPEEKFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer))
#define traceQUEUE_PEEK_FROM_ISR_FAILED( pxQueue )                              SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XQUEUEPEEKFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer))
#define traceQUEUE_RECEIVE( pxQueue )                                           SEGGER_SYSVIEW_RecordU32x4(apiID_OFFSET + apiID_XQUEUEGENERICRECEIVE, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)0), xTicksToWait, 1)
#define traceQUEUE_RECEIVE_FAILED( pxQueue )                                    SEGGER_SYSVIEW_RecordU32x4(apiID_OFFSET + apiID_XQUEUEGENERICRECEIVE, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)0), xTicksToWait, 1)
#define traceQUEUE_SEMAPHORE_RECEIVE( pxQueue )                                 SEGGER_SYSVIEW_RecordU32x4(apiID_OFFSET + apiID_XQUEUEGENERICRECEIVE, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)0), xTicksToWait, 0)
#define traceQUEUE_RECEIVE_FROM_ISR( pxQueue )                                  SEGGER_SYSVIEW_RecordU32x3(apiID_OFFSET + apiID_XQUEUERECEIVEFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer), (U32)pxHigherPriorityTaskWoken)
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue )                           SEGGER_SYSVIEW_RecordU32x3(apiID_OFFSET + apiID_XQUEUERECEIVEFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), SEGGER_SYSVIEW_ShrinkId((U32)pvBuffer), (U32)pxHigherPriorityTaskWoken)
#define traceQUEUE_REGISTRY_ADD( xQueue, pcQueueName )                          SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_VQUEUEADDTOREGISTRY, SEGGER_SYSVIEW_ShrinkId((U32)xQueue), (U32)pcQueueName)
#if ( configUSE_QUEUE_SETS != 1 )
  #define traceQUEUE_SEND( pxQueue )                                            SEGGER_SYSVIEW_RecordU32x4(apiID_OFFSET + apiID_XQUEUEGENERICSEND, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), (U32)pvItemToQueue, xTicksToWait, xCopyPosition)
#else
  #define traceQUEUE_SEND( pxQueue )                                            SEGGER_SYSVIEW_RecordU32x4(apiID_OFFSET + apiID_XQUEUEGENERICSEND, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), 0u, 0u, xCopyPosition)
#endif
#define traceQUEUE_SEND_FAILED( pxQueue )                                       SEGGER_SYSVIEW_RecordU32x4(apiID_OFFSET + apiID_XQUEUEGENERICSEND, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), (U32)pvItemToQueue, xTicksToWait, xCopyPosition)
#define traceQUEUE_SEND_FROM_ISR( pxQueue )                                     SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XQUEUEGENERICSENDFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), (U32)pxHigherPriorityTaskWoken)
#define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue )                              SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XQUEUEGENERICSENDFROMISR, SEGGER_SYSVIEW_ShrinkId((U32)pxQueue), (U32)pxHigherPriorityTaskWoken)
#define traceSTREAM_BUFFER_CREATE( pxStreamBuffer, xIsMessageBuffer )           SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XSTREAMBUFFERCREATE, (U32)xIsMessageBuffer, (U32)pxStreamBuffer)
#define traceSTREAM_BUFFER_CREATE_FAILED( xIsMessageBuffer )                    SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XSTREAMBUFFERCREATE, (U32)xIsMessageBuffer, 0u)
#define traceSTREAM_BUFFER_DELETE( xStreamBuffer )                              SEGGER_SYSVIEW_RecordU32  (apiID_OFFSET + apiID_VSTREAMBUFFERDELETE, (U32)xStreamBuffer)
#define traceSTREAM_BUFFER_RESET( xStreamBuffer )                               SEGGER_SYSVIEW_RecordU32  (apiID_OFFSET + apiID_XSTREAMBUFFERRESET, (U32)xStreamBuffer)
#define traceSTREAM_BUFFER_SEND( xStreamBuffer, xBytesSent )                    SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XSTREAMBUFFERSEND, (U32)xStreamBuffer, (U32)xBytesSent)
#define traceSTREAM_BUFFER_SEND_FAILED( xStreamBuffer )                         SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XSTREAMBUFFERSEND, (U32)xStreamBuffer, 0u)
#define traceSTREAM_BUFFER_SEND_FROM_ISR( xStreamBuffer, xBytesSent )           SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XSTREAMBUFFERSENDFROMISR, (U32)xStreamBuffer, (U32)xBytesSent)
#define traceSTREAM_BUFFER_RECEIVE( xStreamBuffer, xReceivedLength )            SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XSTREAMBUFFERRECEIVE, (U32)xStreamBuffer, (U32)xReceivedLength)
#define traceSTREAM_BUFFER_RECEIVE_FAILED( xStreamBuffer )                      SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XSTREAMBUFFERRECEIVE, (U32)xStreamBuffer, 0u)
#define traceSTREAM_BUFFER_RECEIVE_FROM_ISR( xStreamBuffer, xReceivedLength )   SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET + apiID_XSTREAMBUFFERRECEIVEFROMISR, (U32)xStreamBuffer, (U32)xReceivedLength)


#define traceTASK_DELETE( pxTCB )                   {                                                                                                   \
                                                      SEGGER_SYSVIEW_RecordU32(apiID_OFFSET + apiID_VTASKDELETE, SEGGER_SYSVIEW_ShrinkId((U32)pxTCB));  \
                                                      SYSVIEW_DeleteTask((U32)pxTCB);                                                                   \
                                                    }


#if( portSTACK_GROWTH < 0 )
#define traceTASK_CREATE(pxNewTCB)                  if (pxNewTCB != NULL) {                                             \
                                                      SEGGER_SYSVIEW_OnTaskCreate((U32)pxNewTCB);                       \
                                                      SYSVIEW_AddTask((U32)pxNewTCB,                                    \
                                                                      &(pxNewTCB->pcTaskName[0]),                       \
                                                                      pxNewTCB->uxPriority,                             \
                                                                      (U32)pxNewTCB->pxStack,                           \
                                                                      ((U32)pxNewTCB->pxTopOfStack - (U32)pxNewTCB->pxStack) \
                                                                      );                                                \
                                                    }
#else
#define traceTASK_CREATE(pxNewTCB)                  if (pxNewTCB != NULL) {                                             \
                                                      SEGGER_SYSVIEW_OnTaskCreate((U32)pxNewTCB);                       \
                                                      SYSVIEW_AddTask((U32)pxNewTCB,                                    \
                                                                      &(pxNewTCB->pcTaskName[0]),                       \
                                                                      pxNewTCB->uxPriority,                             \
                                                                      (U32)pxNewTCB->pxStack,                           \
                                                                      (U32)(pxNewTCB->pxStack-pxNewTCB->pxTopOfStack)   \
                                                                      );                                                \
                                                    }
#endif
#define traceTASK_PRIORITY_SET(pxTask, uxNewPriority) {                                                                 \
                                                        SEGGER_SYSVIEW_RecordU32x2(apiID_OFFSET+apiID_VTASKPRIORITYSET, \
                                                                                   SEGGER_SYSVIEW_ShrinkId((U32)pxTCB), \
                                                                                   uxNewPriority                        \
                                                                                  );                                    \
                                                        SYSVIEW_UpdateTask((U32)pxTask,                                 \
                                                                           &(pxTask->pcTaskName[0]),                    \
                                                                           uxNewPriority,                               \
                                                                           (U32)pxTask->pxStack,                        \
                                                                           0                                            \
                                                                          );                                            \
                                                      }
//
// Define INCLUDE_xTaskGetIdleTaskHandle as 1 in FreeRTOSConfig.h to allow identification of Idle state.
//
#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )
  #define traceTASK_SWITCHED_IN()                   if(prvGetTCBFromHandle(NULL) == xIdleTaskHandle) {                  \
                                                      SEGGER_SYSVIEW_OnIdle();                                          \
                                                    } else {                                                            \
                                                      SEGGER_SYSVIEW_OnTaskStartExec((U32)pxCurrentTCB);                \
                                                    }
#else
  #define traceTASK_SWITCHED_IN()                   {                                                                   \
                                                      if (memcmp(pxCurrentTCB->pcTaskName, "IDLE", 5) != 0) {           \
                                                        SEGGER_SYSVIEW_OnTaskStartExec((U32)pxCurrentTCB);              \
                                                      } else {                                                          \
                                                        SEGGER_SYSVIEW_OnIdle();                                        \
                                                      }                                                                 \
                                                    }
#endif

#define traceMOVED_TASK_TO_READY_STATE(pxTCB)       SEGGER_SYSVIEW_OnTaskStartReady((U32)pxTCB)
#define traceREADDED_TASK_TO_READY_STATE(pxTCB)     

#define traceMOVED_TASK_TO_DELAYED_LIST()           SEGGER_SYSVIEW_OnTaskStopReady((U32)pxCurrentTCB,  (1u << 2))
#define traceMOVED_TASK_TO_OVERFLOW_DELAYED_LIST()  SEGGER_SYSVIEW_OnTaskStopReady((U32)pxCurrentTCB,  (1u << 2))
#define traceMOVED_TASK_TO_SUSPENDED_LIST(pxTCB)    SEGGER_SYSVIEW_OnTaskStopReady((U32)pxTCB,         ((3u << 3) | 3))


#define traceISR_EXIT_TO_SCHEDULER()                SEGGER_SYSVIEW_RecordExitISRToScheduler()
#define traceISR_EXIT()                             SEGGER_SYSVIEW_RecordExitISR()
#define traceISR_ENTER(_n_)                         SEGGER_SYSVIEW_RecordEnterISR(_n_)

/*********************************************************************
*
*       API functions
*
**********************************************************************
*/
#ifdef __cplusplus
extern "C" {
#endif
void SYSVIEW_AddTask      (U32 xHandle, const char* pcTaskName, unsigned uxCurrentPriority, U32  pxStack, unsigned uStackHighWaterMark);
void SYSVIEW_UpdateTask   (U32 xHandle, const char* pcTaskName, unsigned uxCurrentPriority, U32 pxStack, unsigned uStackHighWaterMark);
void SYSVIEW_DeleteTask   (U32 xHandle);
void SYSVIEW_SendTaskInfo (U32 TaskID, const char* sName, unsigned Prio, U32 StackBase, unsigned StackSize);

#ifdef __cplusplus
}
#endif

#endif

/*************************** End of file ****************************/