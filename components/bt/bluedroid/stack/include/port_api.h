/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  this file contains the PORT API definitions
 *
 ******************************************************************************/
#ifndef PORT_API_H
#define PORT_API_H

#include "bt_target.h"

/*****************************************************************************
**  Constants and Types
*****************************************************************************/

/*
** Define port settings structure send from the application in the
** set settings request, or to the application in the set settings indication.
*/
typedef struct {

#define PORT_BAUD_RATE_2400       0x00
#define PORT_BAUD_RATE_4800       0x01
#define PORT_BAUD_RATE_7200       0x02
#define PORT_BAUD_RATE_9600       0x03
#define PORT_BAUD_RATE_19200      0x04
#define PORT_BAUD_RATE_38400      0x05
#define PORT_BAUD_RATE_57600      0x06
#define PORT_BAUD_RATE_115200     0x07
#define PORT_BAUD_RATE_230400     0x08

    UINT8  baud_rate;

#define PORT_5_BITS               0x00
#define PORT_6_BITS               0x01
#define PORT_7_BITS               0x02
#define PORT_8_BITS               0x03

    UINT8  byte_size;

#define PORT_ONESTOPBIT           0x00
#define PORT_ONE5STOPBITS         0x01
    UINT8   stop_bits;

#define PORT_PARITY_NO            0x00
#define PORT_PARITY_YES           0x01
    UINT8   parity;

#define PORT_ODD_PARITY           0x00
#define PORT_EVEN_PARITY          0x01
#define PORT_MARK_PARITY          0x02
#define PORT_SPACE_PARITY         0x03

    UINT8   parity_type;

#define PORT_FC_OFF               0x00
#define PORT_FC_XONXOFF_ON_INPUT  0x01
#define PORT_FC_XONXOFF_ON_OUTPUT 0x02
#define PORT_FC_CTS_ON_INPUT      0x04
#define PORT_FC_CTS_ON_OUTPUT     0x08
#define PORT_FC_DSR_ON_INPUT      0x10
#define PORT_FC_DSR_ON_OUTPUT     0x20

    UINT8 fc_type;

    UINT8 rx_char1;

#define PORT_XON_DC1              0x11
    UINT8 xon_char;

#define PORT_XOFF_DC3             0x13
    UINT8 xoff_char;

} tPORT_STATE;


/*
** Define the callback function prototypes.  Parameters are specific
** to each event and are described bellow
*/
typedef int  (tPORT_DATA_CALLBACK) (UINT16 port_handle, void *p_data, UINT16 len);

#define DATA_CO_CALLBACK_TYPE_INCOMING          1
#define DATA_CO_CALLBACK_TYPE_OUTGOING_SIZE     2
#define DATA_CO_CALLBACK_TYPE_OUTGOING          3
typedef int  (tPORT_DATA_CO_CALLBACK) (UINT16 port_handle, UINT8 *p_buf, UINT16 len, int type);

typedef void (tPORT_CALLBACK) (UINT32 code, UINT16 port_handle);

/*
** Define events that registered application can receive in the callback
*/

#define PORT_EV_RXCHAR  0x00000001   /* Any Character received */
#define PORT_EV_RXFLAG  0x00000002   /* Received certain character */
#define PORT_EV_TXEMPTY 0x00000004   /* Transmitt Queue Empty */
#define PORT_EV_CTS     0x00000008   /* CTS changed state */
#define PORT_EV_DSR     0x00000010   /* DSR changed state */
#define PORT_EV_RLSD    0x00000020   /* RLSD changed state */
#define PORT_EV_BREAK   0x00000040   /* BREAK received */
#define PORT_EV_ERR     0x00000080   /* Line status error occurred */
#define PORT_EV_RING    0x00000100   /* Ring signal detected */
#define PORT_EV_CTSS    0x00000400   /* CTS state */
#define PORT_EV_DSRS    0x00000800   /* DSR state */
#define PORT_EV_RLSDS   0x00001000   /* RLSD state */
#define PORT_EV_OVERRUN 0x00002000   /* receiver buffer overrun */
#define PORT_EV_TXCHAR  0x00004000   /* Any character transmitted */

#define PORT_EV_CONNECTED    0x00000200  /* RFCOMM connection established */
#define PORT_EV_CONNECT_ERR  0x00008000  /* Was not able to establish connection */
/* or disconnected */
#define PORT_EV_FC      0x00010000   /* data flow enabled flag changed by remote */
#define PORT_EV_FCS     0x00020000   /* data flow enable status true = enabled */

/*
** To register for events application should provide bitmask with
** corresponding bit set
*/

#define PORT_MASK_ALL             (PORT_EV_RXCHAR | PORT_EV_TXEMPTY | PORT_EV_CTS | \
                                   PORT_EV_DSR | PORT_EV_RLSD | PORT_EV_BREAK | \
                                   PORT_EV_ERR | PORT_EV_RING | PORT_EV_CONNECT_ERR | \
                                   PORT_EV_DSRS | PORT_EV_CTSS | PORT_EV_RLSDS | \
                                   PORT_EV_RXFLAG | PORT_EV_TXCHAR | PORT_EV_OVERRUN | \
                                   PORT_EV_FC | PORT_EV_FCS | PORT_EV_CONNECTED)


/*
** Define port result codes
*/
#define PORT_SUCCESS                0

#define PORT_ERR_BASE               0

#define PORT_UNKNOWN_ERROR          (PORT_ERR_BASE + 1)
#define PORT_ALREADY_OPENED         (PORT_ERR_BASE + 2)
#define PORT_CMD_PENDING            (PORT_ERR_BASE + 3)
#define PORT_APP_NOT_REGISTERED     (PORT_ERR_BASE + 4)
#define PORT_NO_MEM                 (PORT_ERR_BASE + 5)
#define PORT_NO_RESOURCES           (PORT_ERR_BASE + 6)
#define PORT_BAD_BD_ADDR            (PORT_ERR_BASE + 7)
#define PORT_BAD_HANDLE             (PORT_ERR_BASE + 9)
#define PORT_NOT_OPENED             (PORT_ERR_BASE + 10)
#define PORT_LINE_ERR               (PORT_ERR_BASE + 11)
#define PORT_START_FAILED           (PORT_ERR_BASE + 12)
#define PORT_PAR_NEG_FAILED         (PORT_ERR_BASE + 13)
#define PORT_PORT_NEG_FAILED        (PORT_ERR_BASE + 14)
#define PORT_SEC_FAILED             (PORT_ERR_BASE + 15)
#define PORT_PEER_CONNECTION_FAILED (PORT_ERR_BASE + 16)
#define PORT_PEER_FAILED            (PORT_ERR_BASE + 17)
#define PORT_PEER_TIMEOUT           (PORT_ERR_BASE + 18)
#define PORT_CLOSED                 (PORT_ERR_BASE + 19)
#define PORT_TX_FULL                (PORT_ERR_BASE + 20)
#define PORT_LOCAL_CLOSED           (PORT_ERR_BASE + 21)
#define PORT_LOCAL_TIMEOUT          (PORT_ERR_BASE + 22)
#define PORT_TX_QUEUE_DISABLED      (PORT_ERR_BASE + 23)
#define PORT_PAGE_TIMEOUT           (PORT_ERR_BASE + 24)
#define PORT_INVALID_SCN            (PORT_ERR_BASE + 25)

#define PORT_ERR_MAX                (PORT_ERR_BASE + 26)

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         RFCOMM_CreateConnection
**
** Description      RFCOMM_CreateConnection function is used from the application
**                  to establish serial port connection to the peer device,
**                  or allow RFCOMM to accept a connection from the peer
**                  application.
**
** Parameters:      scn          - Service Channel Number as registered with
**                                 the SDP (server) or obtained using SDP from
**                                 the peer device (client).
**                  is_server    - TRUE if requesting application is a server
**                  mtu          - Maximum frame size the application can accept
**                  bd_addr      - BD_ADDR of the peer (client)
**                  mask         - specifies events to be enabled.  A value
**                                 of zero disables all events.
**                  p_handle     - OUT pointer to the handle.
**                  p_mgmt_cb    - pointer to callback function to receive
**                                 connection up/down events.
** Notes:
**
** Server can call this function with the same scn parameter multiple times if
** it is ready to accept multiple simulteneous connections.
**
** DLCI for the connection is (scn * 2 + 1) if client originates connection on
** existing none initiator multiplexer channel.  Otherwise it is (scn * 2).
** For the server DLCI can be changed later if client will be calling it using
** (scn * 2 + 1) dlci.
**
*******************************************************************************/
extern int RFCOMM_CreateConnection (UINT16 uuid, UINT8 scn,
                                    BOOLEAN is_server, UINT16 mtu,
                                    BD_ADDR bd_addr, UINT16 *p_handle,
                                    tPORT_CALLBACK *p_mgmt_cb);


/*******************************************************************************
**
** Function         RFCOMM_RemoveConnection
**
** Description      This function is called to close the specified connection.
**
** Parameters:      handle     - Handle of the port returned in the Open
**
*******************************************************************************/
extern int RFCOMM_RemoveConnection (UINT16 handle);


/*******************************************************************************
**
** Function         RFCOMM_RemoveServer
**
** Description      This function is called to close the server port.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**
*******************************************************************************/
extern int RFCOMM_RemoveServer (UINT16 handle);


/*******************************************************************************
**
** Function         PORT_SetEventCallback
**
** Description      Set event callback the specified connection.
**
** Parameters:      handle       - Handle of the port returned in the Open
**                  p_callback   - address of the callback function which should
**                                 be called from the RFCOMM when an event
**                                 specified in the mask occurs.
**
*******************************************************************************/
extern int PORT_SetEventCallback (UINT16 port_handle,
                                  tPORT_CALLBACK *p_port_cb);

/*******************************************************************************
**
** Function         PORT_ClearKeepHandleFlag
**
** Description      This function is called to clear the keep handle flag
**                  which will cause not to keep the port handle open when closed
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**
*******************************************************************************/
int PORT_ClearKeepHandleFlag (UINT16 port_handle);

/*******************************************************************************
**
** Function         PORT_SetEventCallback
**
** Description      Set event data callback the specified connection.
**
** Parameters:      handle       - Handle of the port returned in the Open
**                  p_callback   - address of the callback function which should
**                                 be called from the RFCOMM when a data
**                                 packet is received.
**
*******************************************************************************/
extern int PORT_SetDataCallback (UINT16 port_handle,
                                 tPORT_DATA_CALLBACK *p_cb);

extern int PORT_SetDataCOCallback (UINT16 port_handle, tPORT_DATA_CO_CALLBACK *p_port_cb);
/*******************************************************************************
**
** Function         PORT_SetEventMask
**
** Description      This function is called to close the specified connection.
**
** Parameters:      handle - Handle of the port returned in the Open
**                  mask   - specifies events to be enabled.  A value
**                           of zero disables all events.
**
*******************************************************************************/
extern int PORT_SetEventMask (UINT16 port_handle, UINT32 mask);


/*******************************************************************************
**
** Function         PORT_CheckConnection
**
** Description      This function returns PORT_SUCCESS if connection referenced
**                  by handle is up and running
**
** Parameters:      handle     - Handle of the port returned in the Open
**                  bd_addr    - OUT bd_addr of the peer
**                  p_lcid     - OUT L2CAP's LCID
**
*******************************************************************************/
extern int PORT_CheckConnection (UINT16 handle, BD_ADDR bd_addr,
                                 UINT16 *p_lcid);

/*******************************************************************************
**
** Function         PORT_IsOpening
**
** Description      This function returns TRUE if there is any RFCOMM connection
**                  opening in process.
**
** Parameters:      TRUE if any connection opening is found
**                  bd_addr    - bd_addr of the peer
**
*******************************************************************************/
extern BOOLEAN PORT_IsOpening (BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         PORT_SetState
**
** Description      This function configures connection according to the
**                  specifications in the tPORT_STATE structure.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_settings - Pointer to a tPORT_STATE structure containing
**                               configuration information for the connection.
**
*******************************************************************************/
extern int PORT_SetState (UINT16 handle, tPORT_STATE *p_settings);

/*******************************************************************************
**
** Function         PORT_GetRxQueueCnt
**
** Description      This function return number of buffers on the rx queue.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_rx_queue_count - Pointer to return queue count in.
**
*******************************************************************************/
extern int PORT_GetRxQueueCnt (UINT16 handle, UINT16 *p_rx_queue_count);

/*******************************************************************************
**
** Function         PORT_GetState
**
** Description      This function is called to fill tPORT_STATE structure
**                  with the current control settings for the port
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_settings - Pointer to a tPORT_STATE structure in which
**                               configuration information is returned.
**
*******************************************************************************/
extern int PORT_GetState (UINT16 handle, tPORT_STATE *p_settings);


/*******************************************************************************
**
** Function         PORT_Control
**
** Description      This function directs a specified connection to pass control
**                  control information to the peer device.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  signal     - specify the function to be passed
**
*******************************************************************************/
#define PORT_SET_DTRDSR         0x01
#define PORT_CLR_DTRDSR         0x02
#define PORT_SET_CTSRTS         0x03
#define PORT_CLR_CTSRTS         0x04
#define PORT_SET_RI             0x05        /* DCE only */
#define PORT_CLR_RI             0x06        /* DCE only */
#define PORT_SET_DCD            0x07        /* DCE only */
#define PORT_CLR_DCD            0x08        /* DCE only */
#define PORT_BREAK              0x09        /* Break event */

extern int PORT_Control (UINT16 handle, UINT8 signal);


/*******************************************************************************
**
** Function         PORT_FlowControl
**
** Description      This function directs a specified connection to pass
**                  flow control message to the peer device.  Enable flag passed
**                  shows if port can accept more data.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  enable     - enables data flow
**
*******************************************************************************/
extern int PORT_FlowControl (UINT16 handle, BOOLEAN enable);


/*******************************************************************************
**
** Function         PORT_GetModemStatus
**
** Description      This function retrieves modem control signals.  Normally
**                  application will call this function after a callback
**                  function is called with notification that one of signals
**                  has been changed.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                               callback.
**                  p_signal   - specify the pointer to control signals info
**
*******************************************************************************/
#define PORT_DTRDSR_ON          0x01
#define PORT_CTSRTS_ON          0x02
#define PORT_RING_ON            0x04
#define PORT_DCD_ON             0x08

/*
** Define default initial local modem signals state set after connection established
*/
#define PORT_OBEX_DEFAULT_SIGNAL_STATE  (PORT_DTRDSR_ON | PORT_CTSRTS_ON | PORT_DCD_ON)
#define PORT_SPP_DEFAULT_SIGNAL_STATE   (PORT_DTRDSR_ON | PORT_CTSRTS_ON | PORT_DCD_ON)
#define PORT_PPP_DEFAULT_SIGNAL_STATE   (PORT_DTRDSR_ON | PORT_CTSRTS_ON | PORT_DCD_ON)
#define PORT_DUN_DEFAULT_SIGNAL_STATE   (PORT_DTRDSR_ON | PORT_CTSRTS_ON)

extern int PORT_GetModemStatus (UINT16 handle, UINT8 *p_control_signal);


/*******************************************************************************
**
** Function         PORT_ClearError
**
** Description      This function retreives information about a communications
**                  error and reports current status of a connection.  The
**                  function should be called when an error occures to clear
**                  the connection error flag and to enable additional read
**                  and write operations.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_errors   - pointer of the variable to receive error codes
**                  p_status   - pointer to the tPORT_STATUS structur to receive
**                               connection status
**
*******************************************************************************/

#define PORT_ERR_BREAK      0x01    /* Break condition occured on the peer device */
#define PORT_ERR_OVERRUN    0x02    /* Overrun is reported by peer device */
#define PORT_ERR_FRAME      0x04    /* Framing error reported by peer device */
#define PORT_ERR_RXOVER     0x08    /* Input queue overflow occured */
#define PORT_ERR_TXFULL     0x10    /* Output queue overflow occured */

typedef struct {
#define PORT_FLAG_CTS_HOLD  0x01    /* Tx is waiting for CTS signal */
#define PORT_FLAG_DSR_HOLD  0x02    /* Tx is waiting for DSR signal */
#define PORT_FLAG_RLSD_HOLD 0x04    /* Tx is waiting for RLSD signal */

    UINT16  flags;
    UINT16  in_queue_size;          /* Number of bytes in the input queue */
    UINT16  out_queue_size;         /* Number of bytes in the output queue */
    UINT16  mtu_size;               /* peer MTU size */
} tPORT_STATUS;


extern int PORT_ClearError (UINT16 handle, UINT16 *p_errors,
                            tPORT_STATUS *p_status);


/*******************************************************************************
**
** Function         PORT_SendError
**
** Description      This function send a communications error to the peer device
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  errors     - receive error codes
**
*******************************************************************************/
extern int PORT_SendError (UINT16 handle, UINT8 errors);


/*******************************************************************************
**
** Function         PORT_GetQueueStatus
**
** Description      This function reports current status of a connection.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_status   - pointer to the tPORT_STATUS structur to receive
**                               connection status
**
*******************************************************************************/
extern int PORT_GetQueueStatus (UINT16 handle, tPORT_STATUS *p_status);


/*******************************************************************************
**
** Function         PORT_Purge
**
** Description      This function discards all the data from the output or
**                  input queues of the specified connection.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  purge_flags - specify the action to take.
**
*******************************************************************************/
#define PORT_PURGE_TXCLEAR  0x01
#define PORT_PURGE_RXCLEAR  0x02

extern int PORT_Purge (UINT16 handle, UINT8 purge_flags);


/*******************************************************************************
**
** Function         PORT_Read
**
** Description      This function returns the pointer to the buffer received
**                  from the peer device.  Normally application will call this
**                  function after receiving PORT_EVT_RXCHAR event.
**                  Application calling this function is responsible to free
**                  buffer returned.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                                callback.
**                  pp_buf      - pointer to address of buffer with data,
**
*******************************************************************************/
extern int PORT_Read (UINT16 handle, BT_HDR **pp_buf);


/*******************************************************************************
**
** Function         PORT_ReadData
**
** Description      Normally application will call this function after receiving
**                  PORT_EVT_RXCHAR event.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                                callback.
**                  p_data      - Data area
**                  max_len     - Byte count requested
**                  p_len       - Byte count received
**
*******************************************************************************/
extern int PORT_ReadData (UINT16 handle, char *p_data, UINT16 max_len,
                          UINT16 *p_len);


/*******************************************************************************
**
** Function         PORT_Write
**
** Description      This function to send BT buffer to the peer device.
**                  Application should not free the buffer.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_buf       - pointer to the buffer with data,
**
*******************************************************************************/
extern int PORT_Write (UINT16 handle, BT_HDR *p_buf);


/*******************************************************************************
**
** Function         PORT_WriteData
**
** Description      This function is called from the legacy application to
**                  send data.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**                  p_data      - Data area
**                  max_len     - Byte count to write
**                  p_len       - Bytes written
**
*******************************************************************************/
extern int PORT_WriteData (UINT16 handle, char *p_data, UINT16 max_len,
                           UINT16 *p_len);

/*******************************************************************************
**
** Function         PORT_WriteDataCO
**
** Description      Normally not GKI aware application will call this function
**                  to send data to the port by callout functions.
**
** Parameters:      handle     - Handle returned in the RFCOMM_CreateConnection
**
*******************************************************************************/
extern int PORT_WriteDataCO (UINT16 handle, int *p_len);

/*******************************************************************************
**
** Function         PORT_Test
**
** Description      Application can call this function to send RFCOMM Test frame
**
** Parameters:      handle      - Handle returned in the RFCOMM_CreateConnection
**                  p_data      - Data area
**                  max_len     - Byte count requested
**
*******************************************************************************/
extern int PORT_Test (UINT16 handle, UINT8 *p_data, UINT16 len);


/*******************************************************************************
**
** Function         RFCOMM_Init
**
** Description      This function is called to initialize RFCOMM layer
**
*******************************************************************************/
extern void RFCOMM_Init (void);


/*******************************************************************************
**
** Function         PORT_SetTraceLevel
**
** Description      This function sets the trace level for RFCOMM. If called with
**                  a value of 0xFF, it simply reads the current trace level.
**
** Returns          the new (current) trace level
**
*******************************************************************************/
extern UINT8 PORT_SetTraceLevel (UINT8 new_level);


/*******************************************************************************
**
** Function         PORT_GetResultString
**
** Description      This function returns the human-readable string for a given
**                  result code.
**
** Returns          a pointer to the human-readable string for the given
**                  result. Note that the string returned must not be freed.
**
*******************************************************************************/
extern const char *PORT_GetResultString (const uint8_t result_code);

#ifdef __cplusplus
}
#endif

#endif  /* PORT_API_H */
