/*
 * SPDX-FileCopyrightText: 1995-2021 SEGGER Microcontroller GmbH
 *
 * SPDX-License-Identifier: BSD-1-Clause
 *
 * SPDX-FileContributor: 2023 Espressif Systems (Shanghai) CO LTD
 */
/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2021 SEGGER Microcontroller GmbH             *
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
*       SystemView version: 3.42                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------
File    : SEGGER_SYSVIEW_ConfDefaults.h
Purpose : Defines defaults for configurable defines used in
          SEGGER SystemView.
Revision: $Rev: 26230 $
*/

#ifndef SEGGER_SYSVIEW_CONFDEFAULTS_H
#define SEGGER_SYSVIEW_CONFDEFAULTS_H

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/

#include "SEGGER_SYSVIEW_Conf.h"
#include "SEGGER_RTT_Conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/
//
// Use auto-detection for SEGGER_SYSVIEW_CORE define
// based on compiler-/toolchain-specific defines
// to define SEGGER_SYSVIEW_GET_INTERRUPT_ID and SEGGER_SYSVIEW_GET_TIMESTAMP
//
#define SEGGER_SYSVIEW_CORE_OTHER   0
#define SEGGER_SYSVIEW_CORE_CM0     1 // Cortex-M0/M0+/M1
#define SEGGER_SYSVIEW_CORE_CM3     2 // Cortex-M3/M4/M7
#define SEGGER_SYSVIEW_CORE_RX      3 // Renesas RX
#ifndef SEGGER_SYSVIEW_CORE
  #if (defined __SES_ARM) || (defined __CROSSWORKS_ARM) || (defined __SEGGER_CC__) || (defined __GNUC__) || (defined __clang__)
    #if (defined __ARM_ARCH_6M__) || (defined __ARM_ARCH_8M_BASE__)
      #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM0
    #elif (defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_8M_MAIN__))
      #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM3
    #endif
  #elif defined(__ICCARM__)
    #if (defined (__ARM6M__)          && (__CORE__ == __ARM6M__))          \
     || (defined (__ARM8M_BASELINE__) && (__CORE__ == __ARM8M_BASELINE__))
      #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM0
    #elif (defined (__ARM7EM__)         && (__CORE__ == __ARM7EM__))         \
       || (defined (__ARM7M__)          && (__CORE__ == __ARM7M__))          \
       || (defined (__ARM8M_MAINLINE__) && (__CORE__ == __ARM8M_MAINLINE__)) \
       || (defined (__ARM8M_MAINLINE__) && (__CORE__ == __ARM8M_MAINLINE__))
      #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM3
    #endif
  #elif defined(__CC_ARM)
    #if (defined(__TARGET_ARCH_6S_M))
      #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM0
    #elif (defined(__TARGET_ARCH_7_M) || defined(__TARGET_ARCH_7E_M))
      #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM3
    #endif
  #elif defined(__TI_ARM__)
    #ifdef __TI_ARM_V6M0__
      #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM0
    #elif (defined(__TI_ARM_V7M3__) || defined(__TI_ARM_V7M4__))
      #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM3
    #endif
  #elif defined(__ICCRX__)
    #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_RX
  #elif defined(__RX)
    #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_RX
  #endif

  #ifndef   SEGGER_SYSVIEW_CORE
    #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_OTHER
  #endif
#endif


/*********************************************************************
*
*       Defines, defaults
*
**********************************************************************
*/
/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_APP_NAME
*
*  Description
*    The application name to be displayed in SystemView.
*  Default
*    "SystemView-enabled Application"
*  Notes
*    Convenience define to be used for SEGGER_SYSVIEW_SendSysDesc().
*/
#ifndef   SEGGER_SYSVIEW_APP_NAME
  #define SEGGER_SYSVIEW_APP_NAME                 "SystemView-enabled Application"
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_DEVICE_NAME
*
*  Description
*    The target device name to be displayed in SystemView.
*  Default
*    "undefined device"
*  Notes
*    Convenience define to be used for SEGGER_SYSVIEW_SendSysDesc().
*/
#ifndef   SEGGER_SYSVIEW_DEVICE_NAME
  #define SEGGER_SYSVIEW_DEVICE_NAME              "undefined device"
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_GET_INTERRUPT_ID()
*
*  Description
*    Function macro to retrieve the Id of the currently active
*    interrupt.
*  Default
*    Call user-supplied function SEGGER_SYSVIEW_X_GetInterruptId().
*  Notes
*    For some known compilers and cores, a ready-to-use, core-specific
*    default is set.
*    ARMv7M: Read ICSR[8:0] (active vector)
*    ARMv6M: Read ICSR[5:0] (active vector)
*/
#ifndef SEGGER_SYSVIEW_GET_INTERRUPT_ID
  #if SEGGER_SYSVIEW_CORE == SEGGER_SYSVIEW_CORE_CM3
    #define SEGGER_SYSVIEW_GET_INTERRUPT_ID()     ((*(U32*)(0xE000ED04)) & 0x1FF)         // Get the currently active interrupt Id. (i.e. read Cortex-M ICSR[8:0] = active vector)
  #elif SEGGER_SYSVIEW_CORE == SEGGER_SYSVIEW_CORE_CM0
    #if defined(__ICCARM__)
      #if (__VER__ > 6010000)
        #define SEGGER_SYSVIEW_GET_INTERRUPT_ID() (__get_IPSR())                          // Workaround for IAR, which might do a byte-access to 0xE000ED04. Read IPSR instead.
      #else
        #define SEGGER_SYSVIEW_GET_INTERRUPT_ID() ((*(U32*)(0xE000ED04)) & 0x3F)          // Older versions of IAR do not include __get_IPSR, but might also not optimize to byte-access.
      #endif
    #else
      #define SEGGER_SYSVIEW_GET_INTERRUPT_ID()   ((*(U32*)(0xE000ED04)) & 0x3F)          // Get the currently active interrupt Id. (i.e. read Cortex-M ICSR[5:0] = active vector)
    #endif
  #else
    #define SEGGER_SYSVIEW_GET_INTERRUPT_ID()     SEGGER_SYSVIEW_X_GetInterruptId()       // Get the currently active interrupt Id from the user-provided function.
  #endif
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_GET_TIMESTAMP()
*
*  Description
*    Function macro to retrieve a system timestamp for SYSVIEW events.
*  Default
*    Call user-supplied function SEGGER_SYSVIEW_X_GetTimestamp().
*  Notes
*    For some known compilers and cores, a ready-to-use, core-specific
*    default is set.
*    ARMv7M: Read Cortex-M Cycle Count register.
*
*    The system timestamp clock frequency has to be passed in
*    SEGGER_SYSVIEW_Init().
*/
#ifndef SEGGER_SYSVIEW_GET_TIMESTAMP
  #if defined (SEGGER_SYSVIEW_CORE) && (SEGGER_SYSVIEW_CORE == SEGGER_SYSVIEW_CORE_CM3)
    #define SEGGER_SYSVIEW_GET_TIMESTAMP()        (*(U32 *)(0xE0001004))                  // Retrieve a system timestamp. Cortex-M cycle counter.
  #else
    #define SEGGER_SYSVIEW_GET_TIMESTAMP()        SEGGER_SYSVIEW_X_GetTimestamp()         // Retrieve a system timestamp via user-defined function
  #endif
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_TIMESTAMP_BITS
*
*  Description
*    Number of valid (low-order) bits delivered in system timestamp.
*  Default
*    32
*  Notes
*    Value has to match system timestamp clock source.
*/
// Define number of valid bits low-order delivered by clock source.
#ifndef   SEGGER_SYSVIEW_TIMESTAMP_BITS
  #define SEGGER_SYSVIEW_TIMESTAMP_BITS           32
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_RTT_CHANNEL
*
*  Description
*    The RTT channel that SystemView will use.
*  Default
*    0: Auto selection.
*  Notes
*    Value has to be lower than SEGGER_RTT_MAX_NUM_UP_BUFFERS.
*/
#ifndef   SEGGER_SYSVIEW_RTT_CHANNEL
  #define SEGGER_SYSVIEW_RTT_CHANNEL              1
#endif
// Sanity check of RTT channel
#if (SEGGER_SYSVIEW_RTT_CHANNEL == 0) && (SEGGER_RTT_MAX_NUM_UP_BUFFERS < 2)
  #error "SEGGER_RTT_MAX_NUM_UP_BUFFERS in SEGGER_RTT_Conf.h has to be > 1!"
#elif (SEGGER_SYSVIEW_RTT_CHANNEL >= SEGGER_RTT_MAX_NUM_UP_BUFFERS)
  #error "SEGGER_RTT_MAX_NUM_UP_BUFFERS  in SEGGER_RTT_Conf.h has to be > SEGGER_SYSVIEW_RTT_CHANNEL!"
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_RTT_BUFFER_SIZE
*
*  Description
*    Number of bytes that SystemView uses for the RTT buffer.
*  Default
*    1024
*/
#ifndef   SEGGER_SYSVIEW_RTT_BUFFER_SIZE
  #define SEGGER_SYSVIEW_RTT_BUFFER_SIZE          1024
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_SECTION
*
*  Description
*    Section to place the SystemView RTT Buffer into.
*  Default
*    undefined: Do not place into a specific section.
*  Notes
*    If SEGGER_RTT_SECTION is defined, the default changes to use
*    this section for the SystemView RTT Buffer, too.
*/
#if !(defined SEGGER_SYSVIEW_SECTION) && (defined SEGGER_RTT_SECTION)
  #define SEGGER_SYSVIEW_SECTION                  SEGGER_RTT_SECTION
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE
*
*  Description
*    Largest cache line size (in bytes) in the target system.
*  Default
*    0
*  Notes
*    Required in systems with caches to make sure that the SystemView
*    RTT buffer can be aligned accordingly.
*/
#ifndef SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE
  #define SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE      0
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_ID_BASE
*
*  Description
*    Lowest Id reported by the application.
*  Default
*    0
*  Notes
*    Value is usually subtracted from mailboxes, semaphores, tasks,
*    .... addresses, to compress event parameters.
*    Should be the lowest RAM address of the system.
*/
#ifndef   SEGGER_SYSVIEW_ID_BASE
  #define SEGGER_SYSVIEW_ID_BASE                  0
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_ID_SHIFT
*
*  Description
*    Number of bits to shift Ids.
*  Default
*    0
*  Notes
*    Ids are shifted to compress event parameters.
*    Should match the alignment of Ids (addresses),
*    e.g. 2 when Ids are 4 byte aligned.
*/
#ifndef   SEGGER_SYSVIEW_ID_SHIFT
  #define SEGGER_SYSVIEW_ID_SHIFT                 0
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_MAX_ARGUMENTS
*
*  Description
*    Maximum number of arguments which are handled with SystemView
*    print routines or may be encoded in one recording function.
*    routines.
*  Default
*    16
*/
#ifndef   SEGGER_SYSVIEW_MAX_ARGUMENTS
  #define SEGGER_SYSVIEW_MAX_ARGUMENTS            16
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_MAX_STRING_LEN
*
*  Description
*    Maximum string length which can be used in SystemView print and
*    system description routines.
*  Default
*    128
*/
#ifndef   SEGGER_SYSVIEW_MAX_STRING_LEN
  #define SEGGER_SYSVIEW_MAX_STRING_LEN           128
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_SUPPORT_LONG_ID
*
*  Description
*    It set, support enconding Evend Ids longer than 14 bit.
*  Default
*    1
*/
#ifndef   SEGGER_SYSVIEW_SUPPORT_LONG_ID
  #define SEGGER_SYSVIEW_SUPPORT_LONG_ID          1
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_SUPPORT_LONG_DATA
*
*  Description
*    It set, support enconding event data longer than 14 bit.
*  Default
*    0
*/
#ifndef   SEGGER_SYSVIEW_SUPPORT_LONG_DATA
  #define SEGGER_SYSVIEW_SUPPORT_LONG_DATA        0
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT
*
*  Description
*    If enabled, on SEGGER_SYSVIEW_PrintHost, check the format string
*    and if it includes unsupported formatters, use formatting on the
*    target instead.
*  Default
*    0: Disabled.
*/
#ifndef   SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT
  #define SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT   0
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_USE_INTERNAL_RECORDER
*
*  Description
*    If set, an internal recorder, such as UART or IP is used.
*  Default
*    0: Disabled.
*  Notes
*    Convenience define to be used by SEGGER_SYSVIEW_Conf(),
*    such as in embOS configuration to enable Cortex-M cycle counter.
*/
#ifndef   SEGGER_SYSVIEW_USE_INTERNAL_RECORDER
  #define SEGGER_SYSVIEW_USE_INTERNAL_RECORDER    0
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_CAN_RESTART
*
*  Description
*    If enabled, send the SystemView start sequence on every start
*    command, not just on the first one.
*    Enables restart when SystemView disconnected unexpectedly.
*  Default
*    1: Enabled
*/
#ifndef   SEGGER_SYSVIEW_CAN_RESTART
  #define SEGGER_SYSVIEW_CAN_RESTART              1
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_START_ON_INIT
*
*  Description
*    Enable calling SEGGER_SYSVIEW_Start() after initialization.
*  Default
*    0: Disabled.
*  Notes
*    Convenience define to be used by SEGGER_SYSVIEW_Conf(),
*    such as in embOS configuration.
*/
#ifndef   SEGGER_SYSVIEW_START_ON_INIT
  #define SEGGER_SYSVIEW_START_ON_INIT            0
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_USE_STATIC_BUFFER
*
*  Description
*    If enabled, use a static buffer instead of a buffer on the stack
*    for SystemView event packets.
*  Default
*    1: Enabled.
*  Notes
*    If enabled, the static memory use by SystemView is increased by
*    the maximum packet size. SystemView is locked on entry of a
*    recording function.
*    If disabled, the stack usage by SystemView recording functions
*    might be increased by up to the maximum packet size. SystemView
*    is locked when writing the packet to the RTT buffer.
*/
#ifndef   SEGGER_SYSVIEW_USE_STATIC_BUFFER
  #define SEGGER_SYSVIEW_USE_STATIC_BUFFER        1
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_MAX_PACKET_SIZE
*
*  Description
*    Maximum packet size for a SystemView event.
*  Default
*    Automatically calculated.
*  Notes
*    The maximum packet size is mainly defined by the maximum string
*    length and the maximum number of arguments.
*/
#ifndef   SEGGER_SYSVIEW_MAX_PACKET_SIZE
  #define SEGGER_SYSVIEW_MAX_PACKET_SIZE          (SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_MAX_STRING_LEN + 2 * SEGGER_SYSVIEW_QUANTA_U32 + SEGGER_SYSVIEW_MAX_ARGUMENTS * SEGGER_SYSVIEW_QUANTA_U32)
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_POST_MORTEM_MODE
*
*  Description
*    If enabled, SystemView records for post-mortem analysis instead
*    of real-time analysis.
*  Default
*    0: Disabled.
*  Notes
*    For more information refer to
*    https://www.segger.com/products/development-tools/systemview/technology/post-mortem-mode
*/
#ifndef   SEGGER_SYSVIEW_POST_MORTEM_MODE
  #define SEGGER_SYSVIEW_POST_MORTEM_MODE         0
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_SYNC_PERIOD_SHIFT
*
*  Description
*    Configure how frequently syncronization is sent in post-mortem
*    mode.
*  Default
*    8: (1 << 8) = Every 256 Events.
*  Notes
*    In post-mortem mode, at least one sync has to be in the RTT buffer.
*    Recommended sync frequency: Buffer Size / 16
*    For more information refer to
*    https://www.segger.com/products/development-tools/systemview/technology/post-mortem-mode
*/
#ifndef   SEGGER_SYSVIEW_SYNC_PERIOD_SHIFT
  #define SEGGER_SYSVIEW_SYNC_PERIOD_SHIFT        8
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_ON_EVENT_RECORDED()
*
*  Description
*    Function macro to notify recorder about a new event in buffer.
*  Default
*    undefined: Do not notify recorder.
*  Notes
*    Used for non-J-Link recorder,
*    such as to enable transmission via UART or notify IP task.
*/
#ifndef   SEGGER_SYSVIEW_ON_EVENT_RECORDED
  #define SEGGER_SYSVIEW_ON_EVENT_RECORDED(NumBytes)
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_LOCK()
*
*  Description
*    Function macro to (nestable) lock SystemView recording.
*  Default
*    Use RTT Locking mechanism (defined by SEGGER_RTT_LOCK()).
*  Notes
*    If SystemView recording is not locked, recording events from
*    interrupts and tasks may lead to unpredictable, undefined, event
*    data.
*/
#ifndef   SEGGER_SYSVIEW_LOCK
  unsigned SEGGER_SYSVIEW_X_SysView_Lock(void);
  #define SEGGER_SYSVIEW_LOCK()                   unsigned _SYSVIEW_int_state = SEGGER_SYSVIEW_X_SysView_Lock()
#endif

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_UNLOCK
*
*  Description
*    Function macro to unlock SystemView recording.
*  Default
*    Use RTT Unlocking mechanism (defined by SEGGER_RTT_UNLOCK()).
*/
#ifndef   SEGGER_SYSVIEW_UNLOCK
  void SEGGER_SYSVIEW_X_SysView_Unlock(unsigned int_state);
  #define SEGGER_SYSVIEW_UNLOCK()                 SEGGER_SYSVIEW_X_SysView_Unlock(_SYSVIEW_int_state)
#endif

#ifdef __cplusplus
}
#endif

#endif

/*************************** End of file ****************************/
