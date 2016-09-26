/******************************************************************************
 *
 *  Copyright (C) 2001-2012 Broadcom Corporation
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
 *  This interface file contains the interface to the Bluetooth Network
 *  Encapsilation Protocol (BNEP).
 *
 ******************************************************************************/
#ifndef BNEP_API_H
#define BNEP_API_H

#include "l2c_api.h"

/*****************************************************************************
**  Constants
*****************************************************************************/

/* Define the minimum offset needed in a GKI buffer for
** sending BNEP packets. Note, we are currently not sending
** extension headers, but may in the future, so allow
** space for them
*/
#define BNEP_MINIMUM_OFFSET        (15 + L2CAP_MIN_OFFSET)
#define BNEP_INVALID_HANDLE         0xFFFF

/*****************************************************************************
**  Type Definitions
*****************************************************************************/

/* Define the result codes from BNEP
*/
enum
{
    BNEP_SUCCESS,                       /* Success                           */
    BNEP_CONN_DISCONNECTED,             /* Connection terminated   */
    BNEP_NO_RESOURCES,                  /* No resources                      */
    BNEP_MTU_EXCEDED,                   /* Attempt to write long data        */
    BNEP_INVALID_OFFSET,                /* Insufficient offset in GKI buffer */
    BNEP_CONN_FAILED,                   /* Connection failed                 */
    BNEP_CONN_FAILED_CFG,               /* Connection failed cos of config   */
    BNEP_CONN_FAILED_SRC_UUID,          /* Connection failed wrong source UUID   */
    BNEP_CONN_FAILED_DST_UUID,          /* Connection failed wrong destination UUID   */
    BNEP_CONN_FAILED_UUID_SIZE,         /* Connection failed wrong size UUID   */
    BNEP_Q_SIZE_EXCEEDED,               /* Too many buffers to dest          */
    BNEP_TOO_MANY_FILTERS,              /* Too many local filters specified  */
    BNEP_SET_FILTER_FAIL,               /* Set Filter failed  */
    BNEP_WRONG_HANDLE,                  /* Wrong handle for the connection  */
    BNEP_WRONG_STATE,                   /* Connection is in wrong state */
    BNEP_SECURITY_FAIL,                 /* Failed because of security */
    BNEP_IGNORE_CMD,                    /* To ignore the rcvd command */
    BNEP_TX_FLOW_ON,                    /* tx data flow enabled */
    BNEP_TX_FLOW_OFF                    /* tx data flow disabled */

}; typedef UINT8 tBNEP_RESULT;


/***************************
**  Callback Functions
****************************/

/* Connection state change callback prototype. Parameters are
**              Connection handle
**              BD Address of remote
**              Connection state change result
**                  BNEP_SUCCESS indicates connection is success
**                  All values are used to indicate the reason for failure
**              Flag to indicate if it is just a role change
*/
typedef void (tBNEP_CONN_STATE_CB) (UINT16 handle,
                                    BD_ADDR rem_bda,
                                    tBNEP_RESULT result,
                                    BOOLEAN is_role_change);




/* Connection indication callback prototype. Parameters are
**              BD Address of remote, remote UUID and local UUID
**              and flag to indicate role change and handle to the connection
**              When BNEP calls this function profile should
**              use BNEP_ConnectResp call to accept or reject the request
*/
typedef void (tBNEP_CONNECT_IND_CB) (UINT16 handle,
                                     BD_ADDR bd_addr,
                                     tBT_UUID *remote_uuid,
                                     tBT_UUID *local_uuid,
                                     BOOLEAN is_role_change);



/* Data buffer received indication callback prototype. Parameters are
**              Handle to the connection
**              Source BD/Ethernet Address
**              Dest BD/Ethernet address
**              Protocol
**              Pointer to the buffer
**              Flag to indicate whether extension headers to be forwarded are present
*/
typedef void (tBNEP_DATA_BUF_CB) (UINT16 handle,
                                  UINT8 *src,
                                  UINT8 *dst,
                                  UINT16 protocol,
                                  BT_HDR *p_buf,
                                  BOOLEAN fw_ext_present);


/* Data received indication callback prototype. Parameters are
**              Handle to the connection
**              Source BD/Ethernet Address
**              Dest BD/Ethernet address
**              Protocol
**              Pointer to the beginning of the data
**              Length of data
**              Flag to indicate whether extension headers to be forwarded are present
*/
typedef void (tBNEP_DATA_IND_CB) (UINT16 handle,
                                  UINT8 *src,
                                  UINT8 *dst,
                                  UINT16 protocol,
                                  UINT8 *p_data,
                                  UINT16 len,
                                  BOOLEAN fw_ext_present);

/* Flow control callback for TX data. Parameters are
**              Handle to the connection
**              Event  flow status
*/
typedef void (tBNEP_TX_DATA_FLOW_CB) (UINT16 handle,
                                      tBNEP_RESULT  event);

/* Filters received indication callback prototype. Parameters are
**              Handle to the connection
**              TRUE if the cb is called for indication
**              Ignore this if it is indication, otherwise it is the result
**                      for the filter set operation performed by the local
**                      device
**              Number of protocol filters present
**              Pointer to the filters start. Filters are present in pairs
**                      of start of the range and end of the range.
**                      They will be present in big endian order. First
**                      two bytes will be starting of the first range and
**                      next two bytes will be ending of the range.
*/
typedef void (tBNEP_FILTER_IND_CB) (UINT16 handle,
                                    BOOLEAN indication,
                                    tBNEP_RESULT result,
                                    UINT16 num_filters,
                                    UINT8 *p_filters);



/* Multicast Filters received indication callback prototype. Parameters are
**              Handle to the connection
**              TRUE if the cb is called for indication
**              Ignore this if it is indication, otherwise it is the result
**                      for the filter set operation performed by the local
**                      device
**              Number of multicast filters present
**              Pointer to the filters start. Filters are present in pairs
**                      of start of the range and end of the range.
**                      First six bytes will be starting of the first range and
**                      next six bytes will be ending of the range.
*/
typedef void (tBNEP_MFILTER_IND_CB) (UINT16 handle,
                                     BOOLEAN indication,
                                     tBNEP_RESULT result,
                                     UINT16 num_mfilters,
                                     UINT8 *p_mfilters);

/* This is the structure used by profile to register with BNEP */
typedef struct
{
    tBNEP_CONNECT_IND_CB    *p_conn_ind_cb;     /* To indicate the conn request */
    tBNEP_CONN_STATE_CB     *p_conn_state_cb;   /* To indicate conn state change */
    tBNEP_DATA_IND_CB       *p_data_ind_cb;     /* To pass the data received */
    tBNEP_DATA_BUF_CB       *p_data_buf_cb;     /* To pass the data buffer received */
    tBNEP_TX_DATA_FLOW_CB   *p_tx_data_flow_cb; /* data flow callback */
    tBNEP_FILTER_IND_CB     *p_filter_ind_cb;   /* To indicate that peer set protocol filters */
    tBNEP_MFILTER_IND_CB    *p_mfilter_ind_cb;  /* To indicate that peer set mcast filters */

} tBNEP_REGISTER;



/* This is the structure used by profile to get the status of BNEP */
typedef struct
{
#define BNEP_STATUS_FAILE            0
#define BNEP_STATUS_CONNECTED        1
    UINT8             con_status;

    UINT16            l2cap_cid;
    BD_ADDR           rem_bda;
    UINT16            rem_mtu_size;
    UINT16            xmit_q_depth;

    UINT16            sent_num_filters;
    UINT16            sent_mcast_filters;
    UINT16            rcvd_num_filters;
    UINT16            rcvd_mcast_filters;
    tBT_UUID          src_uuid;
    tBT_UUID          dst_uuid;

} tBNEP_STATUS;



/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
**
** Function         BNEP_Register
**
** Description      This function is called by the upper layer to register
**                  its callbacks with BNEP
**
** Parameters:      p_reg_info - contains all callback function pointers
**
**
** Returns          BNEP_SUCCESS        if registered successfully
**                  BNEP_FAILURE        if connection state callback is missing
**
*******************************************************************************/
extern tBNEP_RESULT BNEP_Register (tBNEP_REGISTER *p_reg_info);

/*******************************************************************************
**
** Function         BNEP_Deregister
**
** Description      This function is called by the upper layer to de-register
**                  its callbacks.
**
** Parameters:      void
**
**
** Returns          void
**
*******************************************************************************/
extern void BNEP_Deregister (void);


/*******************************************************************************
**
** Function         BNEP_Connect
**
** Description      This function creates a BNEP connection to a remote
**                  device.
**
** Parameters:      p_rem_addr  - BD_ADDR of the peer
**                  src_uuid    - source uuid for the connection
**                  dst_uuid    - destination uuid for the connection
**                  p_handle    - pointer to return the handle for the connection
**
** Returns          BNEP_SUCCESS                if connection started
**                  BNEP_NO_RESOURCES           if no resources
**
*******************************************************************************/
extern tBNEP_RESULT BNEP_Connect (BD_ADDR p_rem_bda,
                                  tBT_UUID *src_uuid,
                                  tBT_UUID *dst_uuid,
                                  UINT16 *p_handle);

/*******************************************************************************
**
** Function         BNEP_ConnectResp
**
** Description      This function is called in responce to connection indication
**
**
** Parameters:      handle  - handle given in the connection indication
**                  resp    - responce for the connection indication
**
** Returns          BNEP_SUCCESS                if connection started
**                  BNEP_WRONG_HANDLE           if the connection is not found
**                  BNEP_WRONG_STATE            if the responce is not expected
**
*******************************************************************************/
extern tBNEP_RESULT BNEP_ConnectResp (UINT16 handle, tBNEP_RESULT resp);

/*******************************************************************************
**
** Function         BNEP_Disconnect
**
** Description      This function is called to close the specified connection.
**
** Parameters:      handle   - handle of the connection
**
** Returns          BNEP_SUCCESS                if connection is disconnected
**                  BNEP_WRONG_HANDLE           if no connection is not found
**
*******************************************************************************/
extern tBNEP_RESULT BNEP_Disconnect (UINT16 handle);

/*******************************************************************************
**
** Function         BNEP_WriteBuf
**
** Description      This function sends data in a GKI buffer on BNEP connection
**
** Parameters:      handle       - handle of the connection to write
**                  p_dest_addr  - BD_ADDR/Ethernet addr of the destination
**                  p_buf        - pointer to address of buffer with data
**                  protocol     - protocol type of the packet
**                  p_src_addr   - (optional) BD_ADDR/ethernet address of the source
**                                 (should be NULL if it is local BD Addr)
**                  fw_ext_present - forwarded extensions present
**
** Returns:         BNEP_WRONG_HANDLE       - if passed handle is not valid
**                  BNEP_MTU_EXCEDED        - If the data length is greater than MTU
**                  BNEP_IGNORE_CMD         - If the packet is filtered out
**                  BNEP_Q_SIZE_EXCEEDED    - If the Tx Q is full
**                  BNEP_SUCCESS            - If written successfully
**
*******************************************************************************/
extern tBNEP_RESULT BNEP_WriteBuf (UINT16 handle,
                                   UINT8 *p_dest_addr,
                                   BT_HDR *p_buf,
                                   UINT16 protocol,
                                   UINT8 *p_src_addr,
                                   BOOLEAN fw_ext_present);

/*******************************************************************************
**
** Function         BNEP_Write
**
** Description      This function sends data over a BNEP connection
**
** Parameters:      handle       - handle of the connection to write
**                  p_dest_addr  - BD_ADDR/Ethernet addr of the destination
**                  p_data       - pointer to data start
**                  protocol     - protocol type of the packet
**                  p_src_addr   - (optional) BD_ADDR/ethernet address of the source
**                                 (should be NULL if it is local BD Addr)
**                  fw_ext_present - forwarded extensions present
**
** Returns:         BNEP_WRONG_HANDLE       - if passed handle is not valid
**                  BNEP_MTU_EXCEDED        - If the data length is greater than MTU
**                  BNEP_IGNORE_CMD         - If the packet is filtered out
**                  BNEP_Q_SIZE_EXCEEDED    - If the Tx Q is full
**                  BNEP_NO_RESOURCES       - If not able to allocate a buffer
**                  BNEP_SUCCESS            - If written successfully
**
*******************************************************************************/
extern tBNEP_RESULT  BNEP_Write (UINT16 handle,
                                 UINT8 *p_dest_addr,
                                 UINT8 *p_data,
                                 UINT16 len,
                                 UINT16 protocol,
                                 UINT8 *p_src_addr,
                                 BOOLEAN fw_ext_present);

/*******************************************************************************
**
** Function         BNEP_SetProtocolFilters
**
** Description      This function sets the protocol filters on peer device
**
** Parameters:      handle        - Handle for the connection
**                  num_filters   - total number of filter ranges
**                  p_start_array - Array of beginings of all protocol ranges
**                  p_end_array   - Array of ends of all protocol ranges
**
** Returns          BNEP_WRONG_HANDLE           - if the connection handle is not valid
**                  BNEP_SET_FILTER_FAIL        - if the connection is in wrong state
**                  BNEP_TOO_MANY_FILTERS       - if too many filters
**                  BNEP_SUCCESS                - if request sent successfully
**
*******************************************************************************/
extern tBNEP_RESULT BNEP_SetProtocolFilters (UINT16 handle,
                                             UINT16 num_filters,
                                             UINT16 *p_start_array,
                                             UINT16 *p_end_array);

/*******************************************************************************
**
** Function         BNEP_SetMulticastFilters
**
** Description      This function sets the filters for multicast addresses for BNEP.
**
** Parameters:      handle        - Handle for the connection
**                  num_filters   - total number of filter ranges
**                  p_start_array - Pointer to sequence of beginings of all
**                                         multicast address ranges
**                  p_end_array   - Pointer to sequence of ends of all
**                                         multicast address ranges
**
** Returns          BNEP_WRONG_HANDLE           - if the connection handle is not valid
**                  BNEP_SET_FILTER_FAIL        - if the connection is in wrong state
**                  BNEP_TOO_MANY_FILTERS       - if too many filters
**                  BNEP_SUCCESS                - if request sent successfully
**
*******************************************************************************/
extern tBNEP_RESULT BNEP_SetMulticastFilters (UINT16 handle,
                                              UINT16 num_filters,
                                              UINT8 *p_start_array,
                                              UINT8 *p_end_array);

/*******************************************************************************
**
** Function         BNEP_SetTraceLevel
**
** Description      This function sets the trace level for BNEP. If called with
**                  a value of 0xFF, it simply reads the current trace level.
**
** Returns          the new (current) trace level
**
*******************************************************************************/
extern UINT8 BNEP_SetTraceLevel (UINT8 new_level);

/*******************************************************************************
**
** Function         BNEP_Init
**
** Description      This function initializes the BNEP unit. It should be called
**                  before accessing any other APIs to initialize the control block
**
** Returns          void
**
*******************************************************************************/
extern void BNEP_Init (void);

/*******************************************************************************
**
** Function         BNEP_GetStatus
**
** Description      This function gets the status information for BNEP connection
**
** Returns          BNEP_SUCCESS            - if the status is available
**                  BNEP_NO_RESOURCES       - if no structure is passed for output
**                  BNEP_WRONG_HANDLE       - if the handle is invalid
**                  BNEP_WRONG_STATE        - if not in connected state
**
*******************************************************************************/
extern tBNEP_RESULT BNEP_GetStatus (UINT16 handle, tBNEP_STATUS *p_status);



#ifdef __cplusplus
}
#endif


#endif
