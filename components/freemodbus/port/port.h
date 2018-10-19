/*
 * FreeModbus Libary: ESP32 Port Demo Application
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * IF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: portother.c,v 1.1 2010/06/06 13:07:20 wolti Exp $
 */
#ifndef _PORT_H
#define _PORT_H
/* ----------------------- Platform includes --------------------------------*/
#include "driver/uart.h"
#include "driver/timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/portmacro.h"
#include "sdkconfig.h"

/* ----------------------- Defines ------------------------------------------*/
#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

#define MB_PORT_TAG "MB_PORT"

#define MB_PORT_CHECK(a, ret_val, str, ...) \
    if (!(a)) { \
        ESP_LOGE(MB_PORT_TAG, "%s(%u): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

#define MB_ENTER_CRITICAL_ISR(mux)  portENTER_CRITICAL_ISR(mux)
#define MB_EXIT_CRITICAL_ISR(mux)   portEXIT_CRITICAL_ISR(mux)
#define MB_ENTER_CRITICAL(mux)      portENTER_CRITICAL(mux)
#define MB_EXIT_CRITICAL(mux)       portEXIT_CRITICAL(mux)

#define ENTER_CRITICAL_SECTION( )   ( vMBPortEnterCritical() )
#define EXIT_CRITICAL_SECTION( )    ( vMBPortExitCritical() )

typedef char    BOOL;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef unsigned short USHORT;
typedef short   SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

void vMBPortSetWithinException( BOOL bInException );

void vMBPortEnterCritical(void);

void vMBPortExitCritical(void);

BOOL xMBPortSerialTxPoll();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
