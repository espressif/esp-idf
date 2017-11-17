/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 2015 - 2017  SEGGER Microcontroller GmbH & Co. KG        *
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
* compatible with the RTT protocol and J-Link.                       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* conditions are met:                                                *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this list of conditions and the following disclaimer.    *
*                                                                    *
* o Redistributions in binary form must reproduce the above          *
*   copyright notice, this list of conditions and the following      *
*   disclaimer in the documentation and/or other materials provided  *
*   with the distribution.                                           *
*                                                                    *
* o Neither the name of SEGGER Microcontroller GmbH & Co. KG         *
*   nor the names of its contributors may be used to endorse or      *
*   promote products derived from this software without specific     *
*   prior written permission.                                        *
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
*       SystemView version: V2.42                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_Conf.h
Purpose : SEGGER SystemView configuration.
Revision: $Rev: 5927 $
*/

#ifndef SEGGER_SYSVIEW_CONF_H
#define SEGGER_SYSVIEW_CONF_H

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/
//
// Constants for known core configuration
//
#define SEGGER_SYSVIEW_CORE_OTHER   0
#define SEGGER_SYSVIEW_CORE_CM0     1 // Cortex-M0/M0+/M1
#define SEGGER_SYSVIEW_CORE_CM3     2 // Cortex-M3/M4/M7
#define SEGGER_SYSVIEW_CORE_RX      3 // Renesas RX

#if (defined __SES_ARM) || (defined __CROSSWORKS_ARM) || (defined __GNUC__)
  #ifdef __ARM_ARCH_6M__
    #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM0
  #elif (defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__))
    #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM3
  #endif
#elif defined(__ICCARM__)
  #if (defined (__ARM6M__) && (__CORE__ == __ARM6M__))
    #define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM0
  #elif ((defined (__ARM7M__) && (__CORE__ == __ARM7M__)) || (defined (__ARM7EM__) && (__CORE__ == __ARM7EM__)))
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

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
/*********************************************************************
*
*       SystemView buffer configuration
*/
#define SEGGER_SYSVIEW_RTT_BUFFER_SIZE      1024                                // Number of bytes that SystemView uses for the buffer.
#define SEGGER_SYSVIEW_RTT_CHANNEL          1                                   // The RTT channel that SystemView will use. 0: Auto selection

#define SEGGER_SYSVIEW_USE_STATIC_BUFFER    1                                   // Use a static buffer to generate events instead of a buffer on the stack

#define SEGGER_SYSVIEW_POST_MORTEM_MODE     0                                   // 1: Enable post mortem analysis mode

/*********************************************************************
*
*       SystemView timestamp configuration
*/
#if SEGGER_SYSVIEW_CORE == SEGGER_SYSVIEW_CORE_CM3
  #define SEGGER_SYSVIEW_GET_TIMESTAMP()      (*(U32 *)(0xE0001004))            // Retrieve a system timestamp. Cortex-M cycle counter.
  #define SEGGER_SYSVIEW_TIMESTAMP_BITS       32                                // Define number of valid bits low-order delivered by clock source
#else
  #define SEGGER_SYSVIEW_GET_TIMESTAMP()      SEGGER_SYSVIEW_X_GetTimestamp()   // Retrieve a system timestamp via user-defined function
  #define SEGGER_SYSVIEW_TIMESTAMP_BITS       32                                // Define number of valid bits low-order delivered by SEGGER_SYSVIEW_X_GetTimestamp()
#endif

/*********************************************************************
*
*       SystemView Id configuration
*/
//TODO: optimise it
#define SEGGER_SYSVIEW_ID_BASE         0x3F400000                               // Default value for the lowest Id reported by the application. Can be overridden by the application via SEGGER_SYSVIEW_SetRAMBase(). (i.e. 0x20000000 when all Ids are an address in this RAM)
#define SEGGER_SYSVIEW_ID_SHIFT        0                                        // Number of bits to shift the Id to save bandwidth. (i.e. 2 when Ids are 4 byte aligned)

/*********************************************************************
*
*       SystemView interrupt configuration
*/
#if SEGGER_SYSVIEW_CORE == SEGGER_SYSVIEW_CORE_CM3
  #define SEGGER_SYSVIEW_GET_INTERRUPT_ID()   ((*(U32 *)(0xE000ED04)) & 0x1FF)  // Get the currently active interrupt Id. (i.e. read Cortex-M ICSR[8:0] = active vector)
#elif SEGGER_SYSVIEW_CORE == SEGGER_SYSVIEW_CORE_CM0
  #if defined(__ICCARM__)
    #define SEGGER_SYSVIEW_GET_INTERRUPT_ID()   (__get_IPSR())                  // Workaround for IAR, which might do a byte-access to 0xE000ED04. Read IPSR instead.
  #else
    #define SEGGER_SYSVIEW_GET_INTERRUPT_ID()   ((*(U32 *)(0xE000ED04)) & 0x3F) // Get the currently active interrupt Id. (i.e. read Cortex-M ICSR[5:0] = active vector)
  #endif
#else
  #define SEGGER_SYSVIEW_GET_INTERRUPT_ID()   SEGGER_SYSVIEW_X_GetInterruptId() // Get the currently active interrupt Id from the user-provided function.
#endif

void SEGGER_SYSVIEW_X_SysView_Lock();
void SEGGER_SYSVIEW_X_SysView_Unlock();
#define SEGGER_SYSVIEW_LOCK()   SEGGER_SYSVIEW_X_SysView_Lock()
#define SEGGER_SYSVIEW_UNLOCK() SEGGER_SYSVIEW_X_SysView_Unlock()

#endif  // SEGGER_SYSVIEW_CONF_H

/*************************** End of file ****************************/
