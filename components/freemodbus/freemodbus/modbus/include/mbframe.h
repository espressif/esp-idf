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
 * File: $Id: mbframe.h,v 1.9 2006/12/07 22:10:34 wolti Exp $
 */

#ifndef _MB_FRAME_H
#define _MB_FRAME_H

#include "port.h"

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/*!
 * Constants which defines the format of a modbus frame. The example is
 * shown for a Modbus RTU/ASCII frame. Note that the Modbus PDU is not
 * dependent on the underlying transport.
 *
 * <code>
 * <------------------------ MODBUS SERIAL LINE PDU (1) ------------------->
 *              <----------- MODBUS PDU (1') ---------------->
 *  +-----------+---------------+----------------------------+-------------+
 *  | Address   | Function Code | Data                       | CRC/LRC     |
 *  +-----------+---------------+----------------------------+-------------+
 *  |           |               |                                   |
 * (2)        (3/2')           (3')                                (4)
 *
 * (1)  ... MB_SER_PDU_SIZE_MAX = 256
 * (2)  ... MB_SER_PDU_ADDR_OFF = 0
 * (3)  ... MB_SER_PDU_PDU_OFF  = 1
 * (4)  ... MB_SER_PDU_SIZE_CRC = 2
 *
 * (1') ... MB_PDU_SIZE_MAX     = 253
 * (2') ... MB_PDU_FUNC_OFF     = 0
 * (3') ... MB_PDU_DATA_OFF     = 1
 * </code>
 */

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_SIZE_MAX         253 /*!< Maximum size of a PDU. */
#define MB_PDU_SIZE_MIN         1   /*!< Function Code */
#define MB_PDU_FUNC_OFF         0   /*!< Offset of function code in PDU. */
#define MB_PDU_DATA_OFF         1   /*!< Offset for response data in PDU. */

#define MB_SER_PDU_SIZE_MAX     MB_SERIAL_BUF_SIZE /*!< Maximum size of a Modbus frame. */
#define MB_SER_PDU_SIZE_LRC     1   /*!< Size of LRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0   /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1   /*!< Offset of Modbus-PDU in Ser-PDU. */
#define MB_SER_PDU_SIZE_CRC     2   /*!< Size of CRC field in PDU. */

#define MB_TCP_TID              0
#define MB_TCP_PID              2
#define MB_TCP_LEN              4
#define MB_TCP_UID              6
#define MB_TCP_FUNC             7

#if MB_MASTER_TCP_ENABLED
#define MB_SEND_BUF_PDU_OFF     MB_TCP_FUNC
#else
#define MB_SEND_BUF_PDU_OFF     MB_SER_PDU_PDU_OFF
#endif

#define MB_TCP_PSEUDO_ADDRESS 255

/* ----------------------- Prototypes  0-------------------------------------*/
typedef void    ( *pvMBFrameStart ) ( void );

typedef void    ( *pvMBFrameStop ) ( void );

typedef eMBErrorCode( *peMBFrameReceive ) ( UCHAR * pucRcvAddress,
                                            UCHAR ** pucFrame,
                                            USHORT * pusLength );

typedef eMBErrorCode( *peMBFrameSend ) ( UCHAR slaveAddress,
                                         const UCHAR * pucFrame,
                                         USHORT usLength );

typedef void( *pvMBFrameClose ) ( void );

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
