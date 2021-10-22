/*
 * SPDX-FileCopyrightText: 2006 Christian Walter
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2016-2021 Espressif Systems (Shanghai) CO LTD
 */
/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbrtu.h,v 1.9 2006/12/07 22:10:34 wolti Exp $
 */
#include "mbconfig.h"

#ifndef _MB_RTU_H
#define _MB_RTU_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */

#if MB_SLAVE_RTU_ENABLED
eMBErrorCode eMBRTUInit( UCHAR slaveAddress, UCHAR ucPort, ULONG ulBaudRate,
                             eMBParity eParity );
void            eMBRTUStart( void );
void            eMBRTUStop( void );
eMBErrorCode    eMBRTUReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength );
eMBErrorCode    eMBRTUSend( UCHAR slaveAddress, const UCHAR * pucFrame, USHORT usLength );
BOOL            xMBRTUReceiveFSM( void );
BOOL            xMBRTUTransmitFSM( void );
BOOL            xMBRTUTimerT15Expired( void );
BOOL            xMBRTUTimerT35Expired( void );
#endif

#if MB_MASTER_RTU_ENABLED
eMBErrorCode    eMBMasterRTUInit( UCHAR ucPort, ULONG ulBaudRate,eMBParity eParity );
void            eMBMasterRTUStart( void );
void            eMBMasterRTUStop( void );
eMBErrorCode    eMBMasterRTUReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength );
eMBErrorCode    eMBMasterRTUSend( UCHAR slaveAddress, const UCHAR * pucFrame, USHORT usLength );
BOOL            xMBMasterRTUReceiveFSM( void );
BOOL            xMBMasterRTUTransmitFSM( void );
BOOL            xMBMasterRTUTimerExpired( void );
#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
