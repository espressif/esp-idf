/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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
#include "mb_m.h"
#include "mbconfig.h"
#include "mbascii.h"
#include "mbframe.h"

#include "mbcrc.h"
#include "mbport.h"

#if MB_MASTER_ASCII_ENABLED > 0

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
    STATE_M_RX_INIT,            /*!< Receiver is in initial state. */
    STATE_M_RX_IDLE,            /*!< Receiver is in idle state. */
    STATE_M_RX_RCV,             /*!< Frame is beeing received. */
    STATE_M_RX_WAIT_EOF,        /*!< Wait for End of Frame. */
    STATE_M_RX_ERROR,           /*!< If the frame is invalid. */
} eMBMasterAsciiRcvState;

typedef enum
{
    STATE_M_TX_IDLE,            /*!< Transmitter is in idle state. */
    STATE_M_TX_START,           /*!< Starting transmission (':' sent). */
    STATE_M_TX_DATA,            /*!< Sending of data (Address, Data, LRC). */
    STATE_M_TX_END,             /*!< End of transmission. */
    STATE_M_TX_NOTIFY,          /*!< Notify sender that the frame has been sent. */
    STATE_M_TX_XFWR,            /*!< Transmitter is in transfer finish and wait receive state. */
} eMBMasterAsciiSndState;

typedef enum
{
    BYTE_HIGH_NIBBLE,           /*!< Character for high nibble of byte. */
    BYTE_LOW_NIBBLE             /*!< Character for low nibble of byte. */
} eMBBytePos;

/* ----------------------- Shared values  -----------------------------------*/
/* These Modbus values are shared in ASCII mode*/
extern volatile UCHAR   ucMasterRcvBuf[];
extern volatile UCHAR   ucMasterSndBuf[];
extern volatile eMBMasterTimerMode eMasterCurTimerMode;

/* ----------------------- Static functions ---------------------------------*/
static UCHAR    prvucMBCHAR2BIN( UCHAR ucCharacter );

static UCHAR    prvucMBBIN2CHAR( UCHAR ucByte );

static UCHAR    prvucMBLRC( UCHAR * pucFrame, USHORT usLen );

/* ----------------------- Static variables ---------------------------------*/
static volatile eMBMasterAsciiSndState eSndState;
static volatile eMBMasterAsciiRcvState eRcvState;

static volatile UCHAR *ucMasterASCIIRcvBuf = ucMasterRcvBuf;
static volatile UCHAR *ucMasterASCIISndBuf = ucMasterSndBuf;

static volatile USHORT usMasterRcvBufferPos;
static volatile eMBBytePos eBytePos;

static volatile UCHAR *pucMasterSndBufferCur;
static volatile USHORT usMasterSndBufferCount;

static volatile UCHAR ucLRC;
static volatile UCHAR ucMBLFCharacter;

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBMasterASCIIInit( UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    ENTER_CRITICAL_SECTION(  );
    ucMBLFCharacter = MB_ASCII_DEFAULT_LF;

    if( xMBMasterPortSerialInit( ucPort, ulBaudRate, MB_ASCII_BITS_PER_SYMB, eParity ) != TRUE )
    {
        eStatus = MB_EPORTERR;
    }
    else if( xMBMasterPortTimersInit( MB_ASCII_TIMEOUT_MS * MB_TIMER_TICS_PER_MS ) != TRUE )
    {
        eStatus = MB_EPORTERR;
    }

    EXIT_CRITICAL_SECTION(  );

    return eStatus;
}

void
eMBMasterASCIIStart( void )
{
    ENTER_CRITICAL_SECTION(  );
    eRcvState = STATE_M_RX_IDLE;
    vMBMasterPortSerialEnable( TRUE, FALSE );
    vMBMasterPortTimersT35Enable(  );
    EXIT_CRITICAL_SECTION(  );
}

void
eMBMasterASCIIStop( void )
{
    ENTER_CRITICAL_SECTION(  );
    vMBMasterPortSerialEnable( FALSE, FALSE );
    vMBMasterPortTimersDisable(  );
    EXIT_CRITICAL_SECTION(  );
}

eMBErrorCode
eMBMasterASCIIReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    UCHAR          *pucMBASCIIFrame = ( UCHAR* ) ucMasterASCIIRcvBuf;
    USHORT          usFrameLength = usMasterRcvBufferPos;

    if( xMBMasterPortSerialGetResponse( &pucMBASCIIFrame, &usFrameLength ) == FALSE )
    {
        return MB_EIO;
    }
    ENTER_CRITICAL_SECTION(  );
    assert( usFrameLength < MB_SER_PDU_SIZE_MAX );

    assert( pucMBASCIIFrame );
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
    } else {
      eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBMasterASCIISend( UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    UCHAR           usLRC;

    if ( ucSlaveAddress > MB_MASTER_TOTAL_SLAVE_NUM ) return MB_EINVAL;

    /* Check if the receiver is still in idle state. If not we where too
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if(eRcvState == STATE_M_RX_IDLE)
    {
        ENTER_CRITICAL_SECTION(  );
        /* First byte before the Modbus-PDU is the slave address. */
        pucMasterSndBufferCur = ( UCHAR * ) pucFrame - 1;
        usMasterSndBufferCount = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pucMasterSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        usMasterSndBufferCount += usLength;

        /* Calculate LRC checksum for Modbus-Serial-Line-PDU. */
        usLRC = prvucMBLRC( ( UCHAR * ) pucMasterSndBufferCur, usMasterSndBufferCount );
        pucMasterSndBufferCur[usMasterSndBufferCount++] = usLRC;

        /* Activate the transmitter. */
        eSndState = STATE_M_TX_START;
        EXIT_CRITICAL_SECTION(  );

        if ( xMBMasterPortSerialSendRequest( ( UCHAR * ) pucMasterSndBufferCur, usMasterSndBufferCount ) == FALSE )
        {
            eStatus = MB_EIO;
        }
        vMBMasterPortSerialEnable( FALSE, TRUE );
    }
    else
    {
        eStatus = MB_EIO;
    }
    return eStatus;
}

BOOL
xMBMasterASCIIReceiveFSM( void )
{
    BOOL            xNeedPoll = FALSE;
    UCHAR           ucByte;
    UCHAR           ucResult;

    assert(( eSndState == STATE_M_TX_IDLE ) || ( eSndState == STATE_M_TX_XFWR ));

    /* Always read the character. */
    xNeedPoll = xMBMasterPortSerialGetByte( ( CHAR * ) & ucByte );

    switch ( eRcvState )
    {
        /* If we have received a character in the init state we have to
        * wait until the frame is finished.
        */
    case STATE_M_RX_INIT:
        vMBMasterPortTimersT35Enable( );
        break;

         /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_M_RX_ERROR:
        vMBMasterPortTimersRespondTimeoutEnable( );
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVE and disable early
         * the timer of respond timeout .
         */
    case STATE_M_RX_IDLE:
        /* Waiting for the start of frame character during respond timeout */
        vMBMasterPortTimersRespondTimeoutEnable(  );
        if( ucByte == ':' )
        {
            /* Reset the input buffers to store the frame in receive state. */
            usMasterRcvBufferPos = 0;
            eBytePos = BYTE_HIGH_NIBBLE;
            eRcvState = STATE_M_RX_RCV;
        }
        eSndState = STATE_M_TX_IDLE;
        break;

        /* A new character is received. If the character is a ':' the input
        * buffer is cleared. A CR-character signals the end of the data
        * block. Other characters are part of the data block and their
        * ASCII value is converted back to a binary representation.
        */
    case STATE_M_RX_RCV:
        /* Enable timer timeout. */
        vMBMasterPortTimersT35Enable(  );
        if( ucByte == ':' )
        {
            /* Empty receive buffer. */
            eBytePos = BYTE_HIGH_NIBBLE;
            usMasterRcvBufferPos = 0;
        }
        else if( ucByte == MB_ASCII_DEFAULT_CR )
        {
            eRcvState = STATE_M_RX_WAIT_EOF;
        }
        else
        {
            ucResult = prvucMBCHAR2BIN( ucByte );
            switch ( eBytePos )
            {
                /* High nibble of the byte comes first. We check for
                 * a buffer overflow here. */
            case BYTE_HIGH_NIBBLE:
                if( usMasterRcvBufferPos < MB_SER_PDU_SIZE_MAX )
                {
                    ucMasterASCIIRcvBuf[usMasterRcvBufferPos] = ( UCHAR )( ucResult << 4 );
                    eBytePos = BYTE_LOW_NIBBLE;
                    break;
                }
                else
                {
                    /* not handled in Modbus specification but seems
                     * a resonable implementation. */
                    eRcvState = STATE_M_RX_ERROR;
                    /* Disable previously activated timer because of error state. */
                    vMBPortTimersDisable(  );
                }
                break;

            case BYTE_LOW_NIBBLE:
                ucMasterASCIIRcvBuf[usMasterRcvBufferPos] |= ucResult;
                usMasterRcvBufferPos++;
                eBytePos = BYTE_HIGH_NIBBLE;
                break;
            }
        }
        break;

    case STATE_M_RX_WAIT_EOF:
        if( ucByte == ucMBLFCharacter )
        {
            /* Disable character timeout timer because all characters are
             * received. */
            vMBMasterPortTimersDisable(  );
            /* Receiver is again in idle state. */
            eRcvState = STATE_M_RX_IDLE;

            /* Notify the caller of eMBMasterASCIIReceive that a new frame
             * was received. */
            (void)xMBMasterPortEventPost( EV_MASTER_FRAME_RECEIVED );
            xNeedPoll = FALSE;
        }
        else if( ucByte == ':' )
        {
            /* Start of frame character received but last message is not completed.
             * Empty receive buffer and back to receive state. */
            eBytePos = BYTE_HIGH_NIBBLE;
            usMasterRcvBufferPos = 0;
            eRcvState = STATE_M_RX_IDLE;

            /* Enable timer for respond timeout and wait for next frame. */
            vMBMasterPortTimersRespondTimeoutEnable(  );
        }
        else
        {
            /* Frame is not okay. Delete entire frame. */
            eRcvState = STATE_M_RX_IDLE;
        }
        break;
    }

    return xNeedPoll;
}

BOOL
xMBMasterASCIITransmitFSM( void )
{
    BOOL            xNeedPoll = TRUE;
    UCHAR           ucByte;
    BOOL            xFrameIsBroadcast = FALSE;

    assert( eRcvState == STATE_M_RX_IDLE );

    switch ( eSndState )
    {
         /* We should not get a transmitter event if the transmitter is in
          * idle state.  */
    case STATE_M_TX_XFWR:
        break;

         /* We should not get a transmitter event if the transmitter is in
          * idle state.  */
    case STATE_M_TX_IDLE:
        break;

        /* Start of transmission. The start of a frame is defined by sending
         * the character ':'. */
    case STATE_M_TX_START:
        ucByte = ':';
        xMBMasterPortSerialPutByte( ( CHAR )ucByte );
        eSndState = STATE_M_TX_DATA;
        eBytePos = BYTE_HIGH_NIBBLE;
        break;

        /* Send the data block. Each data byte is encoded as a character hex
         * stream with the high nibble sent first and the low nibble sent
         * last. If all data bytes are exhausted we send a '\r' character
         * to end the transmission. */
    case STATE_M_TX_DATA:
        if( usMasterSndBufferCount > 0 )
        {
            switch ( eBytePos )
            {
            case BYTE_HIGH_NIBBLE:
                ucByte = prvucMBBIN2CHAR( ( UCHAR )( *pucMasterSndBufferCur >> 4 ) );
                xMBMasterPortSerialPutByte( ( CHAR ) ucByte );
                eBytePos = BYTE_LOW_NIBBLE;
                break;

            case BYTE_LOW_NIBBLE:
                ucByte = prvucMBBIN2CHAR( ( UCHAR )( *pucMasterSndBufferCur & 0x0F ) );
                xMBMasterPortSerialPutByte( ( CHAR )ucByte );
                pucMasterSndBufferCur++;
                eBytePos = BYTE_HIGH_NIBBLE;
                usMasterSndBufferCount--;
                break;
            }
        }
        else
        {
            xMBMasterPortSerialPutByte( MB_ASCII_DEFAULT_CR );
            eSndState = STATE_M_TX_END;
        }
        break;

        /* Finish the frame by sending a LF character. */
    case STATE_M_TX_END:
        xMBMasterPortSerialPutByte( ( CHAR )ucMBLFCharacter );
        /* We need another state to make sure that the CR character has
         * been sent. */
        eSndState = STATE_M_TX_NOTIFY;
        break;

        /* Notify the task which called eMBMasterASCIISend that the frame has
         * been sent. */
    case STATE_M_TX_NOTIFY:
        xFrameIsBroadcast = ( ucMasterASCIISndBuf[MB_SEND_BUF_PDU_OFF - MB_SER_PDU_PDU_OFF]
                                                    == MB_ADDRESS_BROADCAST ) ? TRUE : FALSE;
        vMBMasterRequestSetType( xFrameIsBroadcast );
        eSndState = STATE_M_TX_XFWR;
        /* If the frame is broadcast ,master will enable timer of convert delay,
         * else master will enable timer of respond timeout. */
        if ( xFrameIsBroadcast == TRUE )
        {
             vMBMasterPortTimersConvertDelayEnable( );
        }
        else
        {
            vMBMasterPortTimersRespondTimeoutEnable( );
        }
        xNeedPoll = FALSE;
        break;
    }

    return xNeedPoll;
}

BOOL MB_PORT_ISR_ATTR
xMBMasterASCIITimerT1SExpired( void )
{
    BOOL xNeedPoll = FALSE;

    switch ( eRcvState )
    {
        /* Timer t35 expired. Startup phase is finished. */
    case STATE_M_RX_INIT:
        xNeedPoll = xMBMasterPortEventPost(EV_MASTER_READY);
        break;

        /* Start of message is not received during respond timeout.
         * Process error. */
    case STATE_M_RX_IDLE:
        eRcvState = STATE_M_RX_ERROR;
        break;

        /* A recieve timeout expired and no any new character received.
         * Wait for respond time and go to error state to inform listener about error */
    case STATE_M_RX_RCV:
        eRcvState = STATE_M_RX_ERROR;
        break;

        /* An error occured while receiving the frame. */
    case STATE_M_RX_ERROR:
        vMBMasterSetErrorType(EV_ERROR_RECEIVE_DATA);
        xNeedPoll = xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
        break;

        /* If we have a timeout we go back to the idle state and wait for
         * the next frame.
         */
    case STATE_M_RX_WAIT_EOF:
        eRcvState = STATE_M_RX_IDLE;
        break;

    default:
        assert( 0 );
        break;
    }
    eRcvState = STATE_M_RX_IDLE;

    switch (eSndState)
    {
        /* A frame was send finish and convert delay or respond timeout expired.
         * If the frame is broadcast,The master will idle,and if the frame is not
         * broadcast.*/
    case STATE_M_TX_XFWR:
        if ( xMBMasterRequestIsBroadcast( ) == FALSE ) {
            vMBMasterSetErrorType(EV_ERROR_RESPOND_TIMEOUT);
            xNeedPoll = xMBMasterPortEventPost(EV_MASTER_ERROR_PROCESS);
        }
        break;

        /* Function called in an illegal state. */
    default:
        assert( ( eSndState == STATE_M_TX_START ) || ( eSndState == STATE_M_TX_IDLE )
                || ( eSndState == STATE_M_TX_DATA ) || ( eSndState == STATE_M_TX_END )
                || ( eSndState == STATE_M_TX_NOTIFY ) );
        break;
    }
    eSndState = STATE_M_TX_IDLE;

    vMBMasterPortTimersDisable( );
    /* If timer mode is convert delay, the master event then turns EV_MASTER_EXECUTE status. */
    if (xMBMasterGetCurTimerMode() == MB_TMODE_CONVERT_DELAY) {
        xNeedPoll = xMBMasterPortEventPost( EV_MASTER_EXECUTE );
    }

    vMBMasterPortTimersDisable(  );

    /* no context switch required. */
    return xNeedPoll;
}

static  UCHAR
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

static  UCHAR
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

static  UCHAR
prvucMBLRC( UCHAR * pucFrame, USHORT usLen )
{
    UCHAR   ucLRC = 0;  /* LRC char initialized */

    while( usLen-- )
    {
        ucLRC += *pucFrame++;   /* Add buffer byte without carry */
    }

    /* Return twos complement */
    ucLRC = ( UCHAR ) ( -( ( CHAR ) ucLRC ) );
    return ucLRC;
}

#endif
