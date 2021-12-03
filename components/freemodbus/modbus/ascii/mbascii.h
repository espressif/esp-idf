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
 * File: $Id: mbascii.h,v 1.8 2006/12/07 22:10:34 wolti Exp $
 */

#ifndef _MB_ASCII_H
#define _MB_ASCII_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/* ----------------------- Defines ------------------------------------------*/
#define MB_ASCII_DEFAULT_CR         '\r'    /*!< Default CR character for Modbus ASCII. */
#define MB_ASCII_DEFAULT_LF         '\n'    /*!< Default LF character for Modbus ASCII. */
#define MB_ASCII_SER_PDU_SIZE_MIN   3       /*!< Minimum size of a Modbus ASCII frame. */

/* ----------------------- Function declaration -----------------------------*/

#if MB_SLAVE_ASCII_ENABLED > 0
eMBErrorCode    eMBASCIIInit( UCHAR slaveAddress, UCHAR ucPort,
                              ULONG ulBaudRate, eMBParity eParity );
void            eMBASCIIStart( void );
void            eMBASCIIStop( void );

eMBErrorCode    eMBASCIIReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame,
                                 USHORT * pusLength );
eMBErrorCode    eMBASCIISend( UCHAR slaveAddress, const UCHAR * pucFrame,
                              USHORT usLength );
BOOL            xMBASCIIReceiveFSM( void );
BOOL            xMBASCIITransmitFSM( void );
BOOL            xMBASCIITimerT1SExpired( void );
#endif

#if MB_MASTER_ASCII_ENABLED > 0
eMBErrorCode    eMBMasterASCIIInit( UCHAR ucPort,
                              ULONG ulBaudRate, eMBParity eParity );
void            eMBMasterASCIIStart( void );
void            eMBMasterASCIIStop( void );

eMBErrorCode    eMBMasterASCIIReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame,
                                 USHORT * pusLength );
eMBErrorCode    eMBMasterASCIISend( UCHAR slaveAddress, const UCHAR * pucFrame,
                              USHORT usLength );
BOOL            xMBMasterASCIIReceiveFSM( void );
BOOL            xMBMasterASCIITransmitFSM( void );
BOOL            xMBMasterASCIITimerT1SExpired( void );
#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
