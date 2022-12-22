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
 * File: $Id: mbascii.c,v 1.17 2010/06/06 13:47:07 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbconfig.h"
#include "mbascii.h"
#include "mbframe.h"

#include "mbcrc.h"
#include "mbport.h"

#if MB_SLAVE_ASCII_ENABLED > 0

/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
    STATE_RX_IDLE,              /*!< Receiver is in idle state. */
    STATE_RX_RCV,               /*!< Frame is beeing received. */
    STATE_RX_WAIT_EOF           /*!< Wait for End of Frame. */
} eMBRcvState;

typedef enum
{
    STATE_TX_IDLE,              /*!< Transmitter is in idle state. */
    STATE_TX_START,             /*!< Starting transmission (':' sent). */
    STATE_TX_DATA,              /*!< Sending of data (Address, Data, LRC). */
    STATE_TX_END,               /*!< End of transmission. */
    STATE_TX_NOTIFY             /*!< Notify sender that the frame has been sent. */
} eMBSndState;

typedef enum
{
    BYTE_HIGH_NIBBLE,           /*!< Character for high nibble of byte. */
    BYTE_LOW_NIBBLE             /*!< Character for low nibble of byte. */
} eMBBytePos;

/* ----------------------- Shared variables ---------------------------------*/
/* We reuse the Modbus RTU buffer because only one driver is active */
extern volatile UCHAR ucMbSlaveBuf[];

/* ----------------------- Static functions ---------------------------------*/
static UCHAR    prvucMBCHAR2BIN( UCHAR ucCharacter );

static UCHAR    prvucMBBIN2CHAR( UCHAR ucByte );

static UCHAR    prvucMBLRC( UCHAR * pucFrame, USHORT usLen );

/* ----------------------- Static variables ---------------------------------*/
static volatile eMBSndState eSndState;
static volatile eMBRcvState eRcvState;

static volatile UCHAR *ucASCIIBuf = ucMbSlaveBuf;

static volatile USHORT usRcvBufferPos;
static volatile eMBBytePos eBytePos;

static volatile UCHAR *pucSndBufferCur;
static volatile USHORT usSndBufferCount;

static volatile UCHAR ucLRC;
static volatile UCHAR ucMBLFCharacter;

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBASCIIInit( UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    ( void )ucSlaveAddress;

    ENTER_CRITICAL_SECTION(  );
    ucMBLFCharacter = MB_ASCII_DEFAULT_LF;

    if( xMBPortSerialInit( ucPort, ulBaudRate, MB_ASCII_BITS_PER_SYMB, eParity ) != TRUE )
    {
        eStatus = MB_EPORTERR;
    }
    else if( xMBPortTimersInit( MB_ASCII_TIMEOUT_MS * 20UL ) != TRUE )
    {
        eStatus = MB_EPORTERR;
    }

    EXIT_CRITICAL_SECTION(  );

    return eStatus;
}

void
eMBASCIIStart( void )
{
    ENTER_CRITICAL_SECTION(  );
    vMBPortSerialEnable( TRUE, FALSE );
    eRcvState = STATE_RX_IDLE;
    EXIT_CRITICAL_SECTION(  );

    /* No special startup required for ASCII. */
    ( void )xMBPortEventPost( EV_READY );
}

void
eMBASCIIStop( void )
{
    ENTER_CRITICAL_SECTION(  );
    vMBPortSerialEnable( FALSE, FALSE );
    vMBPortTimersDisable(  );
    EXIT_CRITICAL_SECTION(  );
}

eMBErrorCode
eMBASCIIReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    UCHAR          *pucMBASCIIFrame = ( UCHAR* ) ucASCIIBuf;
    USHORT          usFrameLength = usRcvBufferPos;

    if( xMBPortSerialGetRequest( &pucMBASCIIFrame, &usFrameLength ) == FALSE )
    {
        return MB_EIO;
    }

    ENTER_CRITICAL_SECTION(  );
    assert( usFrameLength < MB_SER_PDU_SIZE_MAX );

    /* Length and CRC check */
   if( ( usFrameLength >= MB_ASCII_SER_PDU_SIZE_MIN )
        && ( prvucMBLRC( ( UCHAR * ) pucMBASCIIFrame, usFrameLength ) == 0 ) )
    {
        /* Save the address field. All frames are passed to the upper layed
         * and the decision if a frame is used is done there.
         */
        *pucRcvAddress = pucMBASCIIFrame[MB_SER_PDU_ADDR_OFF];

        /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
         * size of address field and CRC checksum.
         */
        *pusLength = ( USHORT )( usFrameLength - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_LRC );

        /* Return the start of the Modbus PDU to the caller. */
        *pucFrame = ( UCHAR * ) & pucMBASCIIFrame[MB_SER_PDU_PDU_OFF];
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBASCIISend( UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    UCHAR           usLRC;


    /* Check if the receiver is still in idle state. If not we where too
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if( eRcvState == STATE_RX_IDLE )
    {
         ENTER_CRITICAL_SECTION(  );
        /* First byte before the Modbus-PDU is the slave address. */
        pucSndBufferCur = ( UCHAR * ) pucFrame - 1;
        usSndBufferCount = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        usSndBufferCount += usLength;

        /* Calculate LRC checksum for Modbus-Serial-Line-PDU. */
        usLRC = prvucMBLRC( ( UCHAR * ) pucSndBufferCur, usSndBufferCount );
        ucASCIIBuf[usSndBufferCount++] = usLRC;

        /* Activate the transmitter. */
        eSndState = STATE_TX_START;
        EXIT_CRITICAL_SECTION(  );

        if ( xMBPortSerialSendResponse( ( UCHAR * ) pucSndBufferCur, usSndBufferCount ) == FALSE )
        {
            eStatus = MB_EIO;
        }

        vMBPortSerialEnable( FALSE, TRUE );
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

BOOL
xMBASCIIReceiveFSM( void )
{
    BOOL            xNeedPoll = FALSE;
    UCHAR           ucByte;
    UCHAR           ucResult;

    assert( eSndState == STATE_TX_IDLE );

    xNeedPoll = xMBPortSerialGetByte( ( CHAR * ) & ucByte );
    switch ( eRcvState )
    {
        /* A new character is received. If the character is a ':' the input
         * buffer is cleared. A CR-character signals the end of the data
         * block. Other characters are part of the data block and their
         * ASCII value is converted back to a binary representation.
         */
    case STATE_RX_RCV:
        /* Enable timer for character timeout. */
        vMBPortTimersEnable(  );
        if( ucByte == ':' )
        {
            /* Empty receive buffer. */
            eBytePos = BYTE_HIGH_NIBBLE;
            usRcvBufferPos = 0;
        }
        else if( ucByte == MB_ASCII_DEFAULT_CR )
        {
            eRcvState = STATE_RX_WAIT_EOF;
        }
        else
        {
            ucResult = prvucMBCHAR2BIN( ucByte );
            switch ( eBytePos )
            {
                /* High nibble of the byte comes first. We check for
                 * a buffer overflow here. */
            case BYTE_HIGH_NIBBLE:
                if( usRcvBufferPos < MB_SER_PDU_SIZE_MAX )
                {
                    ucASCIIBuf[usRcvBufferPos] = ( UCHAR )( ucResult << 4 );
                    eBytePos = BYTE_LOW_NIBBLE;
                    break;
                }
                else
                {
                    /* not handled in Modbus specification but seems
                     * a resonable implementation. */
                    eRcvState = STATE_RX_IDLE;
                    /* Disable previously activated timer because of error state. */
                    vMBPortTimersDisable(  );
                }
                break;

            case BYTE_LOW_NIBBLE:
                ucASCIIBuf[usRcvBufferPos] |= ucResult;
                usRcvBufferPos++;
                eBytePos = BYTE_HIGH_NIBBLE;
                break;
            }
        }
        break;

    case STATE_RX_WAIT_EOF:
        if( ucByte == ucMBLFCharacter )
        {
            /* Disable character timeout timer because all characters are
             * received. */
            vMBPortTimersDisable(  );
            /* Receiver is again in idle state. */
            eRcvState = STATE_RX_IDLE;

            /* Notify the caller of eMBASCIIReceive that a new frame
             * was received. */
            (void)xMBPortEventPost( EV_FRAME_RECEIVED );
        }
        else if( ucByte == ':' )
        {
            /* Empty receive buffer and back to receive state. */
            eBytePos = BYTE_HIGH_NIBBLE;
            usRcvBufferPos = 0;
            eRcvState = STATE_RX_RCV;

            /* Enable timer for character timeout. */
            vMBPortTimersEnable(  );
        }
        else
        {
            /* Frame is not okay. Delete entire frame. */
            eRcvState = STATE_RX_IDLE;
        }
        break;

    case STATE_RX_IDLE:
        if( ucByte == ':' )
        {
            /* Enable timer for character timeout. */
            vMBPortTimersEnable(  );
            /* Reset the input buffers to store the frame. */
            usRcvBufferPos = 0;
            eBytePos = BYTE_HIGH_NIBBLE;
            eRcvState = STATE_RX_RCV;
        }
        break;
    }

    return xNeedPoll;
}

BOOL
xMBASCIITransmitFSM( void )
{
    BOOL            xNeedPoll = TRUE;
    UCHAR           ucByte;

    assert( eRcvState == STATE_RX_IDLE );
    switch ( eSndState )
    {
        /* Start of transmission. The start of a frame is defined by sending
         * the character ':'. */
    case STATE_TX_START:
        ucByte = ':';
        xMBPortSerialPutByte( ( CHAR )ucByte );
        eSndState = STATE_TX_DATA;
        eBytePos = BYTE_HIGH_NIBBLE;
        break;

        /* Send the data block. Each data byte is encoded as a character hex
         * stream with the high nibble sent first and the low nibble sent
         * last. If all data bytes are exhausted we send a '\r' character
         * to end the transmission. */
    case STATE_TX_DATA:
        if( usSndBufferCount > 0 )
        {
            switch ( eBytePos )
            {
            case BYTE_HIGH_NIBBLE:
                ucByte = prvucMBBIN2CHAR( ( UCHAR )( *pucSndBufferCur >> 4 ) );
                xMBPortSerialPutByte( ( CHAR ) ucByte );
                eBytePos = BYTE_LOW_NIBBLE;
                break;

            case BYTE_LOW_NIBBLE:
                ucByte = prvucMBBIN2CHAR( ( UCHAR )( *pucSndBufferCur & 0x0F ) );
                xMBPortSerialPutByte( ( CHAR )ucByte );
                pucSndBufferCur++;
                eBytePos = BYTE_HIGH_NIBBLE;
                usSndBufferCount--;
                break;
            }
        }
        else
        {
            xMBPortSerialPutByte( MB_ASCII_DEFAULT_CR );
            eSndState = STATE_TX_END;
        }
        break;

        /* Finish the frame by sending a LF character. */
    case STATE_TX_END:
        xMBPortSerialPutByte( ( CHAR )ucMBLFCharacter );
        /* We need another state to make sure that the CR character has
         * been sent. */
        eSndState = STATE_TX_NOTIFY;
        break;

        /* Notify the task which called eMBASCIISend that the frame has
         * been sent. */
    case STATE_TX_NOTIFY:
        eSndState = STATE_TX_IDLE;
        xMBPortEventPost( EV_FRAME_TRANSMIT );
        xNeedPoll = FALSE;
        break;

        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_TX_IDLE:
        break;
    }

    return xNeedPoll;
}

BOOL MB_PORT_ISR_ATTR
xMBASCIITimerT1SExpired( void )
{
    switch ( eRcvState )
    {
        /* If we have a timeout we go back to the idle state and wait for
         * the next frame.
         */
    case STATE_RX_RCV:
    case STATE_RX_WAIT_EOF:
        eRcvState = STATE_RX_IDLE;
        break;

    default:
        assert( ( eRcvState == STATE_RX_RCV ) || ( eRcvState == STATE_RX_WAIT_EOF )
                        || (eRcvState == STATE_RX_IDLE ));
        break;
    }
    vMBPortTimersDisable(  );

    /* no context switch required. */
    return FALSE;
}


static          UCHAR
prvucMBCHAR2BIN( UCHAR ucCharacter )
{
    if( ( ucCharacter >= '0' ) && ( ucCharacter <= '9' ) )
    {
        return ( UCHAR )( ucCharacter - '0' );
    }
    else if( ( ucCharacter >= 'A' ) && ( ucCharacter <= 'F' ) )
    {
        return ( UCHAR )( ucCharacter - 'A' + 0x0A );
    }
    else
    {
        return 0xFF;
    }
}

static          UCHAR
prvucMBBIN2CHAR( UCHAR ucByte )
{
    if( ucByte <= 0x09 )
    {
        return ( UCHAR )( '0' + ucByte );
    }
    else if( ( ucByte >= 0x0A ) && ( ucByte <= 0x0F ) )
    {
        return ( UCHAR )( ucByte - 0x0A + 'A' );
    }
    else
    {
        /* Programming error. */
        assert( 0 );
    }
    return '0';
}


static          UCHAR
prvucMBLRC( UCHAR * pucFrame, USHORT usLen )
{
    UCHAR           ucLRC = 0;  /* LRC char initialized */

    while( usLen-- )
    {
        ucLRC += *pucFrame++;   /* Add buffer byte without carry */
    }

    /* Return twos complement */
    ucLRC = ( UCHAR ) ( -( ( CHAR ) ucLRC ) );
    return ucLRC;
}

#endif
