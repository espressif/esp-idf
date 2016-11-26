/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
 *  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
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
#ifndef _OI_STATUS_H
#define _OI_STATUS_H
/**
 * @file
 * This file contains status codes for BLUEmagic 3.0 software.
 */

#include "oi_stddefs.h"

/** \addtogroup Misc Miscellaneous APIs */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif


/** test it **/

/**
 * OI_STATUS must fit in 16 bits, so status codes can range from 0 to 66535, inclusive.
 */

typedef enum {
    OI_STATUS_SUCCESS                      = 0,   /**< function call succeeded alias for #OI_OK */
    OI_OK                                  = 0,   /**< function call succeeded alias for #OI_STATUS_SUCCESS */
    OI_STATUS_INVALID_PARAMETERS           = 101, /**< invalid function input parameters */
    OI_STATUS_NOT_IMPLEMENTED              = 102, /**< attempt to use an unimplemented function */
    OI_STATUS_NOT_INITIALIZED              = 103, /**< data not initialized */
    OI_STATUS_NO_RESOURCES                 = 104, /**< generic resource allocation failure status */
    OI_STATUS_INTERNAL_ERROR               = 105, /**< internal inconsistency */
    OI_STATUS_OUT_OF_MEMORY                = 106, /**< generally, OI_Malloc failed */
    OI_ILLEGAL_REENTRANT_CALL              = 107, /**< violation of non-reentrant module policy */
    OI_STATUS_INITIALIZATION_FAILED        = 108, /**< module initialization failed */
    OI_STATUS_INITIALIZATION_PENDING       = 109, /**< inititialization not yet complete */
    OI_STATUS_NO_SCO_SUPPORT               = 110, /**< SCO operation rejected; system not configured for SCO */
    OI_STATUS_OUT_OF_STATIC_MEMORY         = 111, /**< static malloc failed */
    OI_TIMEOUT                             = 112, /**< generic timeout */
    OI_OS_ERROR                            = 113, /**< some operating system error */
    OI_FAIL                                = 114, /**< generic failure */
    OI_STRING_FORMAT_ERROR                 = 115, /**< error in VarString formatting string */
    OI_STATUS_PENDING                      = 116, /**< The operation is pending. */
    OI_STATUS_INVALID_COMMAND              = 117, /**< The command was invalid. */
    OI_BUSY_FAIL                           = 118, /**< command rejected due to busy */
    OI_STATUS_ALREADY_REGISTERED           = 119, /**< The registration has already been performed. */
    OI_STATUS_NOT_FOUND                    = 120, /**< The referenced resource was not found. */
    OI_STATUS_NOT_REGISTERED               = 121, /**< not registered */
    OI_STATUS_NOT_CONNECTED                = 122, /**< not connected */
    OI_CALLBACK_FUNCTION_REQUIRED          = 123, /**< A callback function parameter was required. */
    OI_STATUS_MBUF_OVERFLOW                = 124, /**< There is no room to add another buffer to an mbuf. */
    OI_STATUS_MBUF_UNDERFLOW               = 125, /**< There was an attempt to pull too many bytes from an mbuf. */
    OI_STATUS_CONNECTION_EXISTS            = 126, /**< connection exists */
    OI_STATUS_NOT_CONFIGURED               = 127, /**< module not configured */
    OI_LOWER_STACK_ERROR                   = 128, /**< An error was reported by lower stack API. This is used for embedded platforms. */
    OI_STATUS_RESET_IN_PROGRESS            = 129, /**< Request failed/rejected because we're busy resetting. */
    OI_STATUS_ACCESS_DENIED                = 130, /**< Generic access denied error. */
    OI_STATUS_DATA_ERROR                   = 131, /**< Generic data error. */
    OI_STATUS_INVALID_ROLE                 = 132, /**< The requested role was invalid. */
    OI_STATUS_ALREADY_CONNECTED            = 133, /**< The requested connection is already established. */
    OI_STATUS_PARSE_ERROR                  = 134, /**< Parse error */
    OI_STATUS_END_OF_FILE                  = 135, /**< End of file */
    OI_STATUS_READ_ERROR                   = 136, /**< Generic read error */
    OI_STATUS_WRITE_ERROR                  = 137, /**< Generic write error */
    OI_STATUS_NEGOTIATION_FAILURE          = 138, /**< Error in negotiation */
    OI_STATUS_READ_IN_PROGRESS             = 139, /**< A read is already in progress */
    OI_STATUS_ALREADY_INITIALIZED          = 140, /**< Initialization has already been done */
    OI_STATUS_STILL_CONNECTED              = 141, /**< The service cannot be shutdown because there are still active connections. */
    OI_STATUS_MTU_EXCEEDED                 = 142, /**< The packet is too big */
    OI_STATUS_LINK_TERMINATED              = 143, /**< The link was terminated */
    OI_STATUS_PIN_CODE_TOO_LONG            = 144, /**< Application gave us a pin code that is too long */
    OI_STATUS_STILL_REGISTERED             = 145, /**< The service cannot be shutdown because there are still active registrations. */
    OI_STATUS_SPEC_VIOLATION               = 146, /**< Some application behavior contrary to BT specifications */


    OI_STATUS_PSM_ALREADY_REGISTERED       = 402, /**< L2CAP: The specified PSM has already been registered. */
    OI_STATUS_INVALID_CID                  = 403, /**< L2CAP: CID is invalid or no longer valid (connection terminated) */
    OI_STATUS_CID_NOT_FOUND                = 404, /**< L2CAP: CID does not represent a current connection */
    OI_STATUS_CHANNEL_NOT_FOUND            = 406, /**< L2CAP: CID does not represent a current connection */
    OI_STATUS_PSM_NOT_FOUND                = 407, /**< L2CAP: PSM not found */
    OI_STATUS_INVALID_STATE                = 408, /**< L2CAP: invalid state */
    OI_STATUS_WRITE_IN_PROGRESS            = 410, /**< L2CAP: write in progress */
    OI_STATUS_INVALID_PACKET               = 411, /**< L2CAP: invalid packet */
    OI_STATUS_SEND_COMPLETE                = 412, /**< L2CAP: send is complete */
    OI_STATUS_INVALID_HANDLE               = 414, /**< L2CAP: handle is invalid */
    OI_STATUS_GROUP_FULL                   = 418, /**< L2CAP: No more members can be added to the specified group. */
    OI_STATUS_DEVICE_ALREADY_IN_GROUP      = 423, /**< L2CAP: The device already exists in the group. */
    OI_STATUS_DUPLICATE_GROUP              = 425, /**< L2CAP: attempt to add more than one group */
    OI_STATUS_EMPTY_GROUP                  = 426, /**< L2CAP: group is empty */
    OI_STATUS_PACKET_NOT_FOUND             = 427, /**< L2CAP: packet not found */
    OI_STATUS_BUFFER_TOO_SMALL             = 428, /**< L2CAP: The buffer size is too small. */
    OI_STATUS_IDENTIFIER_NOT_FOUND         = 429, /**< L2CAP: identifier not found */

    OI_L2CAP_DISCONNECT_LOWER_LAYER        = 430, /**< L2CAP: The lower level forced a disconnect. */
    OI_L2CAP_DISCONNECT_REMOTE_REQUEST     = 431, /**< L2CAP: The remote device requested a disconnect. */
    OI_L2CAP_GROUP_ADD_CONNECT_FAIL        = 433, /**< L2CAP: Group add connect faiL */
    OI_L2CAP_GROUP_REMOVE_FAILURE          = 434, /**< L2CAP: Group remove failure */
    OI_L2CAP_DATA_WRITE_ERROR_LINK_TERM    = 435, /**< L2CAP: Data write error LINK_TERM */
    OI_L2CAP_DISCONNECT_LOCAL_REQUEST      = 436, /**< L2CAP: Disconnect local request */

    OI_L2CAP_CONNECT_TIMEOUT               = 437, /**< L2CAP: Connect timeout */
    OI_L2CAP_DISCONNECT_TIMEOUT            = 439, /**< L2CAP: Disconnect timeout */
    OI_L2CAP_PING_TIMEOUT                  = 440, /**< L2CAP: Ping timeout */
    OI_L2CAP_GET_INFO_TIMEOUT              = 441, /**< L2CAP: Get info timeout */
    OI_L2CAP_INVALID_ADDRESS               = 444, /**< L2CAP: Invalid address */
    OI_L2CAP_CMD_REJECT_RCVD               = 445, /**< L2CAP: remote sent us 'command reject' response */

    OI_L2CAP_CONNECT_BASE                  = 450, /**< L2CAP: Connect base */
    OI_L2CAP_CONNECT_PENDING               = 451, /**< L2CAP: Connect pending */
    OI_L2CAP_CONNECT_REFUSED_INVALID_PSM   = 452, /**< L2CAP: Connect refused invalid PSM */
    OI_L2CAP_CONNECT_REFUSED_SECURITY      = 453, /**< L2CAP: Connect refused security */
    OI_L2CAP_CONNECT_REFUSED_NO_RESOURCES  = 454, /**< L2CAP: Connect refused no resources */

    OI_L2CAP_CONFIG_BASE                   = 460, /**< L2CAP: Config base */
    OI_L2CAP_CONFIG_FAIL_INVALID_PARAMETERS = 461, /**< L2CAP: Config fail invalid parameters */
    OI_L2CAP_CONFIG_FAIL_NO_REASON         = 462, /**< L2CAP: Config fail no reason */
    OI_L2CAP_CONFIG_FAIL_UNKNOWN_OPTIONS   = 463, /**< L2CAP: Config fail unknown options */

    OI_L2CAP_GET_INFO_BASE                 = 470, /**< L2CAP: Get info base */
    OI_L2CAP_GET_INFO_NOT_SUPPORTED        = 471, /**< L2CAP: Get info not supported */
    OI_L2CAP_MTU_EXCEEDED                  = 472, /**< L2CAP: The MTU of the channel was exceeded */
    OI_L2CAP_INVALID_PSM                   = 482, /**< L2CAP: Invalid PSM */
    OI_L2CAP_INVALID_MTU                   = 483, /**< L2CAP: Invalid MTU */
    OI_L2CAP_INVALID_FLUSHTO               = 484, /**< L2CAP: Invalid flush timeout */

    OI_HCI_NO_SUCH_CONNECTION               = 601, /**< HCI: caller specified a non-existent connection handle */
    OI_HCI_CB_LIST_FULL                     = 603, /**< HCI: callback list is full, cannot attempt to send command */
    OI_HCI_EVENT_UNDERRUN                   = 605, /**< HCI: parsing event packet, premature end-of-parameters */
    OI_HCI_UNKNOWN_EVENT_CODE               = 607, /**< HCI: event received - event code is unknown */
    OI_HCI_BAD_EVENT_PARM_LEN               = 608, /**< HCI: event - parameter length is incorrect */
    OI_HCI_CMD_QUEUE_FULL                   = 611, /**< HCI: command queue is full */
    OI_HCI_SHORT_EVENT                      = 612, /**< HCI: event received, missing event code and/or parm len */
    OI_HCI_TRANSMIT_NOT_READY               = 613, /**< HCI: ACL/SCO transmit request failed - busy or no buffers available */
    OI_HCI_ORPHAN_SENT_EVENT                = 614, /**< HCI: got spurious 'sent' event from transport layer */
    OI_HCI_CMD_TABLE_ERROR                  = 615, /**< HCI: inconsistency in the internal command table */
    OI_HCI_UNKNOWN_CMD_ID                   = 616, /**< HCI: HciApi Command - unknown command id */
    OI_HCI_UNEXPECTED_EVENT                 = 619, /**< HCI: event received which only occurs in response to our cmd */
    OI_HCI_EVENT_TABLE_ERROR                = 620, /**< HCI: inconsistency in the internal event table */
    OI_HCI_EXPECTED_EVENT_TIMOUT            = 621, /**< HCI: timed out waiting for an expected event */
    OI_HCI_NO_CMD_DESC_FOR_OPCODE           = 622, /**< HCI: event opcode is not known */
    OI_HCI_INVALID_OPCODE_ERROR             = 623, /**< HCI: command opcode is invalid */
    OI_HCI_FLOW_CONTROL_DISABLED            = 624, /**< HCI: can not use host flow control APIs if disabled in configuration */
    OI_HCI_TX_COMPLETE                      = 625, /**< HCI: packet delivery to Host Controler complete */
    OI_HCI_TX_ERROR                         = 626, /**< HCI: failed to deliver packet to Host Controler */
    OI_HCI_DEVICE_NOT_INITIALIZED           = 627, /**< HCI: commands from upper layers disallowed until device is up and running */
    OI_HCI_UNSUPPORTED_COMMAND              = 628, /**< HCI: command requested is not supported by local device */
    OI_HCI_PASSTHROUGH_ERROR                = 629, /**< HCI: Error processing passthrough command */
    OI_HCI_PASSTHROUGH_ALREADY_SET          = 630, /**< HCI: Passthrough mode already enabled */
    OI_HCI_RESET_FAILURE                    = 631, /**< HCI: failed to reset the device/baseband */
    OI_HCI_TRANSPORT_RESET                  = 632, /**< HCI: some operation failed because of a reset in the transport */
    OI_HCIERR_HCIIFC_INIT_FAILURE           = 633, /**< HCI: failed to initialize transport layer interface */

    OI_HCIERR_FIRST_ERROR_VALUE                = 701, /**< marker for first HCI protocol error */
    OI_HCIERR_UNKNOWN_HCI_COMMAND              = 701, /**< HCI: protocol error 0x01 */
    OI_HCIERR_NO_CONNECTION                    = 702, /**< HCI: protocol error 0x02 */
    OI_HCIERR_HARDWARE_FAILURE                 = 703, /**< HCI: protocol error 0x03 */
    OI_HCIERR_PAGE_TIMEOUT                     = 704, /**< HCI: protocol error 0x04 */
    OI_HCIERR_AUTHENTICATION_FAILURE           = 705, /**< HCI: protocol error 0x05 */
    OI_HCIERR_KEY_MISSING                      = 706, /**< HCI: protocol error 0x06 */
    OI_HCIERR_MEMORY_FULL                      = 707, /**< HCI: protocol error 0x07 */
    OI_HCIERR_CONNECTION_TIMEOUT               = 708, /**< HCI: protocol error 0x08 */
    OI_HCIERR_MAX_NUM_OF_CONNECTIONS           = 709, /**< HCI: protocol error 0x09 */
    OI_HCIERR_MAX_NUM_OF_SCO_CONNECTIONS       = 710, /**< HCI: protocol error 0x0A */
    OI_HCIERR_ACL_CONNECTION_ALREADY_EXISTS    = 711, /**< HCI: protocol error 0x0B */
    OI_HCIERR_COMMAND_DISALLOWED               = 712, /**< HCI: protocol error 0x0C */
    OI_HCIERR_HOST_REJECTED_RESOURCES          = 713, /**< HCI: protocol error 0x0D */
    OI_HCIERR_HOST_REJECTED_SECURITY           = 714, /**< HCI: protocol error 0x0E */
    OI_HCIERR_HOST_REJECTED_PERSONAL_DEVICE    = 715, /**< HCI: protocol error 0x0F */
    OI_HCIERR_HOST_TIMEOUT                     = 716, /**< HCI: protocol error 0x10 */
    OI_HCIERR_UNSUPPORTED                      = 717, /**< HCI: protocol error 0x11 */
    OI_HCIERR_INVALID_PARAMETERS               = 718, /**< HCI: protocol error 0x12 */
    OI_HCIERR_OTHER_END_USER_DISCONNECT        = 719, /**< HCI: protocol error 0x13 */
    OI_HCIERR_OTHER_END_LOW_RESOURCES          = 720, /**< HCI: protocol error 0x14 */
    OI_HCIERR_OTHER_END_POWERING_OFF           = 721, /**< HCI: protocol error 0x15 */
    OI_HCIERR_CONNECTION_TERMINATED_LOCALLY    = 722, /**< HCI: protocol error 0x16 */
    OI_HCIERR_REPEATED_ATTEMPTS                = 723, /**< HCI: protocol error 0x17 */
    OI_HCIERR_PAIRING_NOT_ALLOWED              = 724, /**< HCI: protocol error 0x18 */
    OI_HCIERR_UNKNOWN_LMP_PDU                  = 725, /**< HCI: protocol error 0x19 */
    OI_HCIERR_UNSUPPORTED_REMOTE_FEATURE       = 726, /**< HCI: protocol error 0x1A */
    OI_HCIERR_SCO_OFFSET_REJECTED              = 727, /**< HCI: protocol error 0x1B */
    OI_HCIERR_SCO_INTERVAL_REJECTED            = 728, /**< HCI: protocol error 0x1C */
    OI_HCIERR_SCO_AIR_MODE_REJECTED            = 729, /**< HCI: protocol error 0x1D */
    OI_HCIERR_INVALID_LMP_PARMS                = 730, /**< HCI: protocol error 0x1E */
    OI_HCIERR_UNSPECIFIED_ERROR                = 731, /**< HCI: protocol error 0x1F */
    OI_HCIERR_UNSUPPORTED_LMP_PARAMETERS       = 732, /**< HCI: protocol error 0x20 */
    OI_HCIERR_ROLE_CHANGE_NOT_ALLOWED          = 733, /**< HCI: protocol error 0x21 */
    OI_HCIERR_LMP_RESPONSE_TIMEOUT             = 734, /**< HCI: protocol error 0x22 */
    OI_HCIERR_LMP_ERROR_TRANS_COLLISION        = 735, /**< HCI: protocol error 0x23 */
    OI_HCIERR_LMP_PDU_NOT_ALLOWED              = 736, /**< HCI: protocol error 0x24 */
    OI_HCIERR_ENCRYPTION_MODE_NOT_ACCEPTABLE   = 737, /**< HCI: protocol error 0x25 */
    OI_HCIERR_UNIT_KEY_USED                    = 738, /**< HCI: protocol error 0x26 */
    OI_HCIERR_QOS_NOT_SUPPORTED                = 739, /**< HCI: protocol error 0x27 */
    OI_HCIERR_INSTANT_PASSED                   = 740, /**< HCI: protocol error 0x28 */
    OI_HCIERR_UNIT_KEY_PAIRING_UNSUPPORTED     = 741, /**< HCI: protocol error 0x29 */
    OI_HCIERR_DIFFERENT_TRANS_COLLISION        = 742, /**< HCI: protocol error 0x2A */
    OI_HCIERR_RESERVED_2B                      = 743, /**< HCI: protocol error 0x2B */
    OI_HCIERR_QOS_UNACCEPTABLE_PARAMETER       = 744, /**< HCI: protocol error 0x2C */
    OI_HCIERR_QOS_REJECTED                     = 745, /**< HCI: protocol error 0x2D */
    OI_HCIERR_CHANNEL_CLASSIFICATION_NS        = 746, /**< HCI: protocol error 0x2E */
    OI_HCIERR_INSUFFICIENT_SECURITY            = 747, /**< HCI: protocol error 0x2F */
    OI_HCIERR_PARM_OUT_OF_MANDATORY_RANGE      = 748, /**< HCI: protocol error 0x30 */
    OI_HCIERR_RESERVED_31                      = 749, /**< HCI: protocol error 0x31 */
    OI_HCIERR_ROLE_SWITCH_PENDING              = 750, /**< HCI: protocol error 0x32 */
    OI_HCIERR_RESERVED_33                      = 751, /**< HCI: protocol error 0x33 */
    OI_HCIERR_RESERVED_SLOT_VIOLATION          = 752, /**< HCI: protocol error 0x34 */
    OI_HCIERR_ROLE_SWITCH_FAILED               = 753, /**< HCI: protocol error 0x35 */
    OI_HCIERR_EIR_TOO_LARGE                    = 754, /**< HCI: protocol error 0x36 */
    OI_HCIERR_SSP_NOT_SUPPORTED_BY_HOST        = 755, /**< HCI: protocol error 0x37 */
    OI_HCIERR_HOST_BUSY_PAIRING                = 756, /**< HCI: protocol error 0x38 */

    OI_HCIERR_UNKNOWN_ERROR                    = 757, /**< HCI: unknown error code */
    OI_HCIERR_LAST_ERROR_VALUE                 = 757, /**< marker for last HCI protocol error */

    OI_SDP_SPEC_ERROR                    = 800, /**< SDP: Base error status for mapping OI_STATUS codes to SDP errors */
    OI_SDP_INVALID_SERVICE_RECORD_HANDLE = (OI_SDP_SPEC_ERROR + 2), /**< SDP: protocol error Invalid Service Record Handle */
    OI_SDP_INVALID_REQUEST_SYNTAX        = (OI_SDP_SPEC_ERROR + 3), /**< SDP: protocol error Invalid Request Syntax */
    OI_SDP_INVALID_PDU_SIZE              = (OI_SDP_SPEC_ERROR + 4), /**< SDP: protocol error Invalid PDU Size */
    OI_SDP_INVALID_CONTINUATION_STATE    = (OI_SDP_SPEC_ERROR + 5), /**< SDP: protocol error Invalid Continuation State */
    OI_SDP_INSUFFICIENT_RESOURCES        = (OI_SDP_SPEC_ERROR + 6), /**< SDP: protocol error Insufficient Resources */
    OI_SDP_ERROR                         = 807, /**< SDP: server returned an error code */
    OI_SDP_CORRUPT_DATA_ELEMENT          = 808, /**< SDP: Invalid or corrupt data element representation */
    OI_SDP_SERVER_NOT_CONNECTED          = 810, /**< SDP: Attempt to disconnect from an unconnected server */
    OI_SDP_ACCESS_DENIED                 = 811, /**< SDP: Server denied access to server */
    OI_SDP_ATTRIBUTES_OUT_OF_ORDER       = 812, /**< SDP: Attributes in attribute list not in ascending order */
    OI_SDP_DEVICE_DOES_NOT_SUPPORT_SDP   = 813, /**< SDP: Tried to connect to a device that does not support SDP */
    OI_SDP_NO_MORE_DATA                  = 815, /**< SDP: Server does not have more continuation data */
    OI_SDP_REQUEST_PARAMS_TOO_LONG       = 816, /**< SDP: Parameters for a request exceed the L2CAP buffer size */
    OI_SDP_REQUEST_PENDING               = 817, /**< SDP: Cannot make a request when another request is being processed */
    OI_SDP_SERVER_CONNECT_FAILED         = 819, /**< SDP: Failed attempt to connect to an SDP server */
    OI_SDP_SERVER_TOO_MANY_CONNECTIONS   = 821, /**< SDP: Exceeded maximum number of simultaneous server connections */
    OI_SDP_NO_MATCHING_SERVICE_RECORD    = 823, /**< SDP: No service record matched the UUID list */
    OI_SDP_PARTIAL_RESPONSE              = 824, /**< SDP: Internal use only */
    OI_SDP_ILLEGAL_ARGUMENT              = 825, /**< SDP: Illegal argument passed to an SDP function */
    OI_SDP_ATTRIBUTE_NOT_FOUND           = 826, /**< SDP: A requested attribute was not found in a service record */
    OI_SDP_DATABASE_OUT_OF_RESOURCES     = 827, /**< SDP: server database is out of memory */
    OI_SDP_SHORT_PDU                     = 829, /**< SDP: Not enough bytes in the packet */
    OI_SDP_TRANSACTION_ID_MISMATCH       = 830, /**< SDP: Transaction Id was not as expected */
    OI_SDP_UNEXPECTED_RESPONSE_PDU_ID    = 831, /**< SDP: Did not expect this response PDU */
    OI_SDP_REQUEST_TIMEOUT               = 832, /**< SDP: Did not get a response within the timeout period */
    OI_SDP_INVALID_RESPONSE_SYNTAX       = 833, /**< SDP: Response is not correctly formatted */
    OI_SDP_CONNECTION_TIMEOUT            = 834, /**< SDP: Connection attempt timed out at a lower layer */
    OI_SDP_RESPONSE_DATA_ERROR           = 835, /**< SDP: Response to a service request appears to be corrupt */
    OI_SDP_TOO_MANY_ATTRIBUTE_BYTES      = 836, /**< SDP: Response contained more bytes than requested. */
    OI_SDP_TOO_MANY_SERVICE_RECORDS      = 837, /**< SDP: Response contained more service records than requested. */
    OI_SDP_INVALID_CONNECTION_ID         = 838, /**< SDP: Invalid connection ID in an SDP request */
    OI_SDP_CANNOT_SET_ATTRIBUTE          = 839, /**< SDP: Attempt to set a dynamic attribute value failed */
    OI_SDP_BADLY_FORMED_ATTRIBUTE_VALUE  = 840, /**< SDP: An attribute value has the wrong type or structure */
    OI_SDP_NO_ATTRIBUTE_LIST_TO_REMOVE   = 841, /**< SDP: Attempt to remove a non-existent attribute list from a service record */
    OI_SDP_ATTRIBUTE_LIST_ALREADY_ADDED  = 842, /**< SDP: An attribute list has already been added to the service record */
    OI_SDP_DATA_ELEMENT_TRUNCATED        = 843, /**< SDP: Data element truncated (too few bytes) */

    OI_RFCOMM_WRITE_IN_PROGRESS          = 901, /**< RFCOMM: Write in progress */
    OI_RFCOMM_INVALID_BAUDRATE           = 903, /**< RFCOMM: Invalid baudrate */
    OI_RFCOMM_INVALID_DATABIT            = 904, /**< RFCOMM: Invalid databit */
    OI_RFCOMM_INVALID_STOPBIT            = 905, /**< RFCOMM: Invalid stopbit */
    OI_RFCOMM_INVALID_PARITY             = 906, /**< RFCOMM: Invalid parity */
    OI_RFCOMM_INVALID_PARITYTYPE         = 907, /**< RFCOMM: Invalid paritytype */
    OI_RFCOMM_INVALID_FLOWCONTROL        = 908, /**< RFCOMM: Invalid flowcontrol */
    OI_RFCOMM_SESSION_EXISTS             = 909, /**< RFCOMM: Session exists */
    OI_RFCOMM_INVALID_CHANNEL            = 910, /**< RFCOMM: Invalid channel */
    OI_RFCOMM_DLCI_EXISTS                = 911, /**< RFCOMM: DLCI exists */
    OI_RFCOMM_LINK_NOT_FOUND             = 912, /**< RFCOMM: Link not found */
    OI_RFCOMM_REMOTE_REJECT              = 913, /**< RFCOMM: Remote reject */
    OI_RFCOMM_TEST_IN_PROGRESS           = 915, /**< RFCOMM: Test in progress */
    OI_RFCOMM_SESSION_NOT_FOUND          = 916, /**< RFCOMM: Session not found */
    OI_RFCOMM_INVALID_PACKET             = 917, /**< RFCOMM: Invalid packet */
    OI_RFCOMM_FRAMESIZE_EXCEEDED         = 918, /**< RFCOMM: Framesize exceeded */
    OI_RFCOMM_INVALID_DLCI               = 920, /**< RFCOMM: Invalid dlci */
    OI_RFCOMM_SERVER_NOT_REGISTERED      = 921, /**< RFCOMM: Server not registered */
    OI_RFCOMM_CREDIT_ERROR               = 922, /**< RFCOMM: Credit error */
    OI_RFCOMM_NO_CHANNEL_NUMBER          = 923, /**< RFCOMM: No channel number */
    OI_RFCOMM_QUERY_IN_PROGRESS          = 924, /**< RFCOMM: Query in progress */
    OI_RFCOMM_SESSION_SHUTDOWN           = 925, /**< RFCOMM: Session shutdown */
    OI_RFCOMM_LOCAL_DEVICE_DISCONNECTED  = 926, /**< RFCOMM: Local device disconnected */
    OI_RFCOMM_REMOTE_DEVICE_DISCONNECTED = 927, /**< RFCOMM: Remote device disconnected */
    OI_RFCOMM_OUT_OF_SERVER_CHANNELS     = 928, /**< RFCOMM: Out of server channels */

    OI_DISPATCH_INVALID_CB_HANDLE        = 1001, /**< Dispatcher was handed an invalid callback handle */
    OI_DISPATCH_TABLE_OVERFLOW           = 1002, /**< Dispatcher table is full */

    OI_TEST_UNKNOWN_TEST                 = 1101, /**< TEST: Unknown test */
    OI_TEST_FAIL                         = 1102, /**< TEST: Fail */

    OI_HCITRANS_CANNOT_CONNECT_TO_DEVICE   = 1201, /**< TRANSPORT: Cannot connect to device */
    OI_HCITRANS_BUFFER_TOO_SMALL           = 1203, /**< TRANSPORT: Buffer too small */
    OI_HCITRANS_NULL_DEVICE_HANDLE         = 1204, /**< TRANSPORT: Null device handle */
    OI_HCITRANS_IO_ERROR                   = 1205, /**< TRANSPORT: IO error */
    OI_HCITRANS_DEVICE_NOT_READY           = 1206, /**< TRANSPORT: Device not ready */
    OI_HCITRANS_FUNCTION_NOT_SUPPORTED     = 1207, /**< TRANSPORT: Function not supporteD */
    OI_HCITRANS_ACCESS_DENIED              = 1209, /**< TRANSPORT: win32 */
    OI_HCITRANS_ACL_DATA_ERROR             = 1210, /**< TRANSPORT: ACL data error */
    OI_HCITRANS_SCO_DATA_ERROR             = 1211, /**< TRANSPORT: SCO data error */
    OI_HCITRANS_EVENT_DATA_ERROR           = 1212, /**< TRANSPORT: HCI event data error */
    OI_HCITRANS_INTERNAL_ERROR             = 1214, /**< TRANSPORT: Internal error in the transport */
    OI_HCITRANS_LINK_NOT_ACTIVE            = 1215, /**< TRANSPORT: Link to the device is not currently active */
    OI_HCITRANS_INITIALIZING               = 1216, /**< TRANSPORT: Transport is initializing */

    OI_DEVMGR_NO_CONNECTION                = 1301, /**< DEVMGR: No connection */
    OI_DEVMGR_HARDWARE_ERROR               = 1305, /**< DEVMGR: error reported by HCI */
    OI_DEVMGR_PENDING_CONNECT_LIST_FULL    = 1307, /**< DEVMGR: Pending connect list full */
    OI_DEVMGR_CONNECTION_LIST_FULL         = 1309, /**< DEVMGR: Connection list full */
    OI_DEVMGR_NO_SUCH_CONNECTION           = 1310, /**< DEVMGR: No such connection */
    OI_DEVMGR_INQUIRY_IN_PROGRESS          = 1311, /**< DEVMGR: Inquiry in progress */
    OI_DEVMGR_PERIODIC_INQUIRY_ACTIVE      = 1312, /**< DEVMGR: Periodic inquiry active */
    OI_DEVMGR_NO_INQUIRIES_ACTIVE          = 1313, /**< DEVMGR: can not cancel/exit if not active */
    OI_DEVMGR_DUPLICATE_CONNECTION         = 1314, /**< DEVMGR: internal error */
    OI_DEVMGR_DUPLICATE_EVENT_CALLBACK     = 1316, /**< DEVMGR: attempt to register same callback twice */
    OI_DEVMGR_EVENT_CALLBACK_LIST_FULL     = 1317, /**< DEVMGR: can not register event callback, list is full */
    OI_DEVMGR_EVENT_CALLBACK_NOT_FOUND     = 1318, /**< DEVMGR: attempt to unregister callback failed */
    OI_DEVMGR_BUSY                         = 1319, /**< DEVMGR: some operations can only execute one at a time */
    OI_DEVMGR_ENUM_UNEXPECTED_INQ_COMPLETE = 1320, /**< DEVMGR: inquiry complete event in inappropriate enumeration state */
    OI_DEVMGR_ENUM_UNEXPECTED_INQ_RESULT   = 1321, /**< DEVMGR: inquiry result event in inappropriate enumeration state */
    OI_DEVMGR_ENUM_DATABASE_FULL           = 1322, /**< DEVMGR: device enumeration, database is full, couldn't add a new device */
    OI_DEVMGR_ENUM_INQUIRIES_OVERLAP       = 1323, /**< DEVMGR: device enumeration, periodic inquiries occurring too close together */
    OI_DEVMGR_UNKNOWN_LINK_TYPE            = 1324, /**< DEVMGR: HCI connect request with unkown link type */
    OI_DEVMGR_PARAM_IO_ACTIVE              = 1325, /**< DEVMGR: request for parameter read/write while param read/write active */
    OI_DEVMGR_UNKNOWN_IAC_LAP              = 1326, /**< DEVMGR: unrecognized IAC LAP */
    OI_DEVMGR_SCO_ALREADY_REGISTERED       = 1327, /**< DEVMGR: only one application can use SCO */
    OI_DEVMGR_SCO_NOT_REGISTERED           = 1328, /**< DEVMGR: SCO applications must register before using the API */
    OI_DEVMGR_SCO_WITHOUT_ACL              = 1329, /**< DEVMGR: Got SCO connection but there is no underlying ACL connection */
    OI_DEVMGR_NO_SUPPORT                   = 1330, /**< DEVMGR: Request is not supported by the device */
    OI_DEVMGR_WRITE_POLICY_FAILED          = 1331, /**< DEVMGR: connection attempt failed - unable to write link policy */
    OI_DEVMGR_NOT_IN_MASTER_MODE           = 1332, /**< DEVMGR: OI_DEVMGR EndMasterMode without prior OI_DEVMGR_BeginMasterMode */
    OI_DEVMGR_POLICY_VIOLATION             = 1333, /**< DEVMGR: low-power request is rejected - link policy does not allow it */
    OI_DEVMGR_BUSY_TIMEOUT                 = 1334, /**< DEVMGR: queued operation timed out while in the queue; \n
        timeout configurable via @ref OI_CONFIG_DEVMGR::connectQueueTimeoutSecs "connectQueueTimeoutSecs" */
    OI_DEVMGR_REENCRYPT_FAILED             = 1335, /**< DEVMGR: failed to re-encrypt link after role switch */
    OI_DEVMGR_ROLE_POLICY_CONFLICT         = 1336, /**< DEVMGR: requested role conflicts with current policy */
    OI_DEVMGR_BAD_INTERVAL                 = 1337, /**< DEVMGR: current linkTO outside range of requested min/max interval */
    OI_DEVMGR_INVALID_SCO_HANDLE           = 1338, /**< DEVMGR: HCI SCO event, invalid handle */
    OI_DEVMGR_CONNECTION_OVERLAP           = 1339, /**< DEVMGR: Connection failed due to race condition with remote side */
    OI_DEVMGR_ORPHAN_SUBRATE_COMPLETE      = 1340, /**< DEVMGR: sniff subrate complete, but no callback */
    OI_DEVMGR_EIR_RESPONSE_2_LARGE         = 1341, /**< DEVMGR: eir builder, response length would exceed spec max */

    OI_SECMGR_NO_POLICY                    = 1401, /**< SECMGR: no security policy has been established */
    OI_SECMGR_INTERNAL_ERROR               = 1402, /**< SECMGR: internal inconsistency */
    OI_SECMGR_ORPHANED_CALLBACK            = 1403, /**< SECMGR: we've been called back, but CB context is gone */
    OI_SECMGR_BUSY                         = 1404, /**< SECMGR: configure and access request cannot be concurrent */
    OI_SECMGR_DEVICE_NOT_TRUSTED           = 1405, /**< SECMGR: l2cap access denied - device is not trusted */
    OI_SECMGR_DEVICE_ENCRYPT_FAIL          = 1407, /**< SECMGR: l2cap access denied - failed to start encryption */
    OI_SECMGR_DISCONNECTED_FAIL            = 1408, /**< SECMGR: l2cap access denied - disconnected */
    OI_SECMGR_ACCESS_PENDING               = 1409, /**< SECMGR: l2cap access request is still pending  */
    OI_SECMGR_PIN_CODE_TOO_SHORT           = 1410, /**< SECMGR: Higher-layer process gave us a pin code that is too short */
    OI_SECMGR_UNKNOWN_ENCRYPT_VALUE        = 1411, /**< SECMGR: got EncryptionChange event, unknown encryption enable value */
    OI_SECMGR_INVALID_POLICY               = 1412, /**< SECMGR: the specified security policy is not valid for security mode */
    OI_SECMGR_AUTHORIZATION_FAILED         = 1413, /**< SECMGR: device authorization failed */
    OI_SECMGR_ENCRYPTION_FAILED            = 1414, /**< SECMGR: device encryption failed */
    OI_SECMGR_UNIT_KEY_UNSUPPORTED         = 1415, /**< SECMGR: authentication failed due to non-support of unit keys */
    OI_SECMGR_NOT_REGISTERED               = 1416, /**< SECMGR: required registrations have not yet occurred */
    OI_SECMGR_ILLEGAL_WRITE_SSP_MODE       = 1417, /**< SECMGR: 2.1 HCI spec does not allow SSP mode to be disabled */
    OI_SECMGR_INVALID_SEC_LEVEL            = 1418, /**< SECMGR: security level for a service is not a valid value */
    OI_SECMGR_INSUFFICIENT_LINK_KEY        = 1419, /**< SECMGR: link key type is not sufficient to meet service requirements */
    OI_SECMGR_INVALID_KEY_TYPE             = 1420, /**< SECMGR: link key type is not a valid value */
    OI_SECMGR_SSP_NOT_ENCRYPTED            = 1421, /**< SECMGR: ssp required encryption on incoming link */
    OI_SECMGR_ORPHAN_EVENT                 = 1422, /**< SECMGR: some HCI security event unrelated to current processes */
    OI_SECMGR_NOT_BONDABLE                 = 1423, /**< SECMGR: not in bondable mode */

    OI_TCS_INVALID_ELEMENT_TYPE            = 1602, /**< TCS: element type is invalid */
    OI_TCS_INVALID_PACKET                  = 1603, /**< TCS: packet is invalide */
    OI_TCS_CALL_IN_PROGRESS                = 1604, /**< TCS: call is in progress */
    OI_TCS_NO_CALL_IN_PROGRESS             = 1605, /**< TCS: no call in progress */

    OI_OBEX_CONTINUE                       = 1701, /**< OBEX: Continue processing OBEX request */
    OI_OBEX_COMMAND_ERROR                  = 1702, /**< OBEX: An unrecognized OBEX command opcode */
    OI_OBEX_CONNECTION_TIMEOUT             = 1703, /**< OBEX: Timeout waiting for a response to a request */
    OI_OBEX_CONNECT_FAILED                 = 1704, /**< OBEX: An OBEX connection request did not succeed */
    OI_OBEX_DISCONNECT_FAILED              = 1705, /**< OBEX: A disconnect failed probably because the connection did not exist */
    OI_OBEX_ERROR                          = 1706, /**< OBEX: Unspecified OBEX error */
    OI_OBEX_INCOMPLETE_PACKET              = 1707, /**< OBEX: Packet too short or corrupt */
    OI_OBEX_LENGTH_REQUIRED                = 1708, /**< OBEX: Length header required in OBEX command */
    OI_OBEX_NOT_CONNECTED                  = 1709, /**< OBEX: No connection to OBEX server */
    OI_OBEX_NO_MORE_CONNECTIONS            = 1710, /**< OBEX: Reached max connections limit */
    OI_OBEX_OPERATION_IN_PROGRESS          = 1711, /**< OBEX: Another operation is still in progress on a connection */
    OI_OBEX_PUT_RESPONSE_ERROR             = 1712, /**< OBEX: An error in the response to a PUT command */
    OI_OBEX_GET_RESPONSE_ERROR             = 1713, /**< OBEX: An error in the response to a GET command */
    OI_OBEX_REQUIRED_HEADER_NOT_FOUND      = 1714, /**< OBEX: packet was missing a required header */
    OI_OBEX_SERVICE_UNAVAILABLE            = 1715, /**< OBEX: Unown OBEX target or required service */
    OI_OBEX_TOO_MANY_HEADER_BYTES          = 1716, /**< OBEX: Headers will not fit in single OBEX packet */
    OI_OBEX_UNKNOWN_COMMAND                = 1717, /**< OBEX: Unrecognized OBEX command */
    OI_OBEX_UNSUPPORTED_VERSION            = 1718, /**< OBEX: Version mismatch */
    OI_OBEX_CLIENT_ABORTED_COMMAND         = 1719, /**< OBEX: server received abort command */
    OI_OBEX_BAD_PACKET                     = 1720, /**< OBEX: Any malformed OBEX packet */
    OI_OBEX_BAD_REQUEST                    = 1721, /**< OBEX: Maps to OBEX response of the same name */
    OI_OBEX_OBJECT_OVERFLOW                = 1723, /**< OBEX: Too many bytes received. */
    OI_OBEX_NOT_FOUND                      = 1724, /**< OBEX: Maps to obex response of same name */
    OI_OBEX_ACCESS_DENIED                  = 1735, /**< OBEX: Object could not be read or written. */
    OI_OBEX_VALUE_NOT_ACCEPTABLE           = 1736, /**< OBEX: Value in a command was not in the acceptable range. */
    OI_OBEX_PACKET_OVERFLOW                = 1737, /**< OBEX: Buffer will not fit in a single OBEX packet. */
    OI_OBEX_NO_SUCH_FOLDER                 = 1738, /**< OBEX: Error returned by a setpath operation. */
    OI_OBEX_NAME_REQUIRED                  = 1739, /**< OBEX: Name must be non-null and non-empty. */
    OI_OBEX_PASSWORD_TOO_LONG              = 1740, /**< OBEX: Password exceeds implementation imposed length limit. */
    OI_OBEX_PRECONDITION_FAILED            = 1741, /**< OBEX: response Precondition Failed */
    OI_OBEX_UNAUTHORIZED                   = 1742, /**< OBEX: authentication was not successful. */
    OI_OBEX_NOT_IMPLEMENTED                = 1743, /**< OBEX: Unimplemented feature. */
    OI_OBEX_INVALID_AUTH_DIGEST            = 1744, /**< OBEX: An authentication digest was bad. */
    OI_OBEX_INVALID_OPERATION              = 1745, /**< OBEX: Operation not allowed at this time. */
    OI_OBEX_DATABASE_FULL                  = 1746, /**< OBEX: Sync database full. */
    OI_OBEX_DATABASE_LOCKED                = 1747, /**< OBEX: Sync database locked. */
    OI_OBEX_INTERNAL_SERVER_ERROR          = 1748, /**< OBEX: response Internal Server Error */
    OI_OBEX_UNSUPPORTED_MEDIA_TYPE         = 1749, /**< OBEX: response Unsupported Media Type */
    OI_OBEX_PARTIAL_CONTENT                = 1750, /**< OBEX: response Partial Content */
    OI_OBEX_METHOD_NOT_ALLOWED             = 1751, /**< OBEX: response Method Not Allowed */
    OI_OBEXSRV_INCOMPLETE_GET              = 1752, /**< OBEX: Indicates to a GET handler that the request phase is still in progress */
    OI_OBEX_FOLDER_BROWSING_NOT_ALLOWED    = 1753, /**< OBEX: Indicates that an FTP server does not allow folder browsing */
    OI_OBEX_SERVER_FORCED_DISCONNECT       = 1754, /**< OBEX: connection was forcibly terminated by the server */
    OI_OBEX_OFS_ERROR                      = 1755, /**< OBEX: OPP object file system error occurred */
    OI_OBEX_FILEOP_ERROR                   = 1756, /**< OBEX: FTP/PBAP file operation system error occurred */
    OI_OBEX_USERID_TOO_LONG                = 1757, /**< OBEX: User Id exceeds spec limited length limit. */

    OI_HANDSFREE_EVENT_REPORTING_DISABLED  = 1801, /**< HANDSFREE: Event reporting disabled */
    OI_HANDSFREE_NOT_CONNECTED             = 1802, /**< HANDSFREE: Not connected */
    OI_HANDSFREE_SERVICE_NOT_STARTED       = 1803, /**< HANDSFREE: Cannot connect to handsfree AG if handsfree service not started */
    OI_HANDSFREE_AG_SERVICE_NOT_STARTED    = 1804, /**< HANDSFREE: Cannot connect to handsfree device if handsfree AG service not started */
    OI_HANDSFREE_COMMAND_IN_PROGRESS       = 1805, /**< HANDSFREE: Cannot accept a command at this time */
    OI_HANDSFREE_AUDIO_ALREADY_CONNECTED   = 1806, /**< HANDSFREE: Audio is already connected */
    OI_HANDSFREE_AUDIO_NOT_CONNECTED       = 1807, /**< HANDSFREE: Audio is not connected */
    OI_HANDSFREE_FEATURE_NOT_SUPPORTED     = 1808, /**< HANDSFREE: Local or remote feature not supported for requested command */

    OI_HEADSET_SERVICE_NOT_STARTED         = 1901, /**< HEADSET: Cannot connect to headset AG if headset service not started */
    OI_HEADSET_AG_SERVICE_NOT_STARTED      = 1902, /**< HEADSET: Cannot connect to headset device if headset AG service not started */
    OI_HEADSET_COMMAND_IN_PROGRESS         = 1903, /**< HEADSET: Cannot accept a command at this time */

    OI_BNEP_INVALID_MTU                             = 2001, /**< BNEP: The remote device cannot support the minimum BNEP MTU */
    OI_BNEP_SETUP_TIMEOUT                           = 2002, /**< BNEP: The setup request timed out. */
    OI_BNEP_SERVICE_NOT_REGISTERED                  = 2003, /**< BNEP: The requested service was not found. */
    OI_BNEP_INVALID_HANDLE                          = 2004, /**< BNEP: The specified connection handle is not valid. */
    OI_BNEP_RESPONSE_TIMEOUT                        = 2005, /**< BNEP: The timer for receiving a response has expired. */
    OI_BNEP_INVALID_CONNECTION                      = 2006, /**< BNEP: Invalid connection */
    OI_BNEP_INVALID_FILTER                          = 2007, /**< BNEP: The supplied filter was invalid. */
    OI_BNEP_CONNECTION_EXISTS                       = 2008, /**< BNEP: An attempt was made to create a duplicate connection. */
    OI_BNEP_NOT_INITIALIZED                         = 2009, /**< BNEP: Init has not been called */
    OI_BNEP_CONNECT_BASE                            = 2010, /**< BNEP: connection response codes */
    OI_BNEP_CONNECT_FAILED_INVALID_DEST_UUID        = 2011, /**< BNEP: connect response code Invalid Dest UUID */
    OI_BNEP_CONNECT_FAILED_INVALID_SOURCE_UUID      = 2012, /**< BNEP: connect response code Invalid Source UUID */
    OI_BNEP_CONNECT_FAILED_INVALID_UUID_SIZE        = 2013, /**< BNEP: connect response code Invalid UUID Size */
    OI_BNEP_CONNECT_FAILED_NOT_ALLOWED              = 2014, /**< BNEP: connect response code Not Allowed */
    OI_BNEP_FILTER_NET_BASE                         = 2020, /**< BNEP: filter response codes */
    OI_BNEP_FILTER_NET_UNSUPPORTED_REQUEST          = 2021, /**< BNEP: filter response code Unsupported Request */
    OI_BNEP_FILTER_NET_FAILED_INVALID_PROTOCOL_TYPE = 2022, /**< BNEP: filter response code Invalid Protocol Type */
    OI_BNEP_FILTER_NET_FAILED_MAX_LIMIT_REACHED     = 2023, /**< BNEP: filter response code Max Limit Reached */
    OI_BNEP_FILTER_NET_FAILED_SECURITY              = 2024, /**< BNEP: filter response code Security */
    OI_BNEP_FILTER_MULTI_BASE                       = 2030, /**< BNEP: multicast response codes */
    OI_BNEP_FILTER_MULTI_UNSUPPORTED_REQUEST        = 2031, /**< BNEP: multicast response code Unsupported Request */
    OI_BNEP_FILTER_MULTI_FAILED_INVALID_ADDRESS     = 2032, /**< BNEP: multicast response code Invalid Address */
    OI_BNEP_FILTER_MULTI_FAILED_MAX_LIMIT_REACHED   = 2033, /**< BNEP: multicast response code Max Limit Reached */
    OI_BNEP_FILTER_MULTI_FAILED_SECURITY            = 2034, /**< BNEP: multicast response code Security */
    OI_BNEP_LOCAL_DEVICE_MUST_BE_MASTER             = 2040, /**< BNEP: Device must be master of the piconet for this function */
    OI_BNEP_PACKET_FILTERED_OUT                     = 2041, /**< BNEP: Packet did not pass current filters */

    OI_NETIFC_UP_FAILED                    = 2101, /**< NETIFC: Could not bring up network interface */
    OI_NETIFC_COULD_NOT_CREATE_THREAD      = 2102, /**< NETIFC: Network interface could not create a read thread */
    OI_NETIFC_INITIALIZATION_FAILED        = 2103, /**< NETIFC: Error in network interface initialization */
    OI_NETIFC_INTERFACE_ALREADY_UP         = 2104, /**< NETIFC: Network interface is already up */
    OI_NETIFC_INTERFACE_NOT_UP             = 2105, /**< NETIFC: Network interface is not up */
    OI_NETIFC_PACKET_TOO_BIG               = 2106, /**< NETIFC: The packet is too big */

    OI_PAN_ROLE_ALREADY_REGISTERED         = 2201, /**< PAN: This PAN role was already registered */
    OI_PAN_ROLE_NOT_ALLOWED                = 2202, /**< PAN: The PAN role is not currently allowed */
    OI_PAN_INCOMPATIBLE_ROLES              = 2203, /**< PAN: Only certain local and remote role combinations are permitted */
    OI_PAN_INVALID_ROLE                    = 2204, /**< PAN: Role specified is not one the defined PAN roles */
    OI_PAN_CONNECTION_IN_PROGRESS          = 2205, /**< PAN: A PAN connection is currently being established */
    OI_PAN_USER_ALREADY_CONNECTED          = 2206, /**< PAN: PAN user role only allows a single connection */
    OI_PAN_DEVICE_CONNECTED                = 2207, /**< PAN: A PAN connection already exists to specified device */

    OI_CODEC_SBC_NO_SYNCWORD               = 2301, /**< CODEC: Couldn't find an SBC SYNCWORD */
    OI_CODEC_SBC_NOT_ENOUGH_HEADER_DATA    = 2302, /**< CODEC: Not enough data provided to decode an SBC header */
    OI_CODEC_SBC_NOT_ENOUGH_BODY_DATA      = 2303, /**< CODEC: Decoded the header, but not enough data to contain the rest of the frame */
    OI_CODEC_SBC_NOT_ENOUGH_AUDIO_DATA     = 2304, /**< CODEC: Not enough audio data for this frame */
    OI_CODEC_SBC_CHECKSUM_MISMATCH         = 2305, /**< CODEC: The frame header didn't match the checksum */
    OI_CODEC_SBC_PARTIAL_DECODE            = 2306, /**< CODEC: Decoding was successful, but frame data still remains. Next call will provide audio without consuming input data. */

    OI_FIFOQ_QUEUE_NOT_ALIGNED             = 2401, /**< FIFOQ: queue must be 32-bit aligned */
    OI_FIFOQ_INVALID_Q                     = 2402, /**< FIFOQ: queue parameter is not a valid queue */
    OI_FIFOQ_BUF_TOO_LARGE                 = 2403, /**< FIFOQ: attempt to queue a buffer which is too large */
    OI_FIFOQ_FULL                          = 2404, /**< FIFOQ: enqueue() failed, queue is full */
    OI_FIFOQ_NOT_ALLOCATED                 = 2405, /**< FIFOQ: Enqueue QBuf() failed, buffer not allocated */
    OI_FIFOQ_INVALID_DATA_PTR              = 2406, /**< FIFOQ: Enqueue QBuf() failed, data pointer does not match */

    OI_HID_HOST_SERVICE_NOT_STARTED        = 2601, /**< HID: Cannot connect to a HID device unless HID host is started */
    OI_HID_DEVICE_SERVICE_NOT_STARTED      = 2602, /**< HID: Cannot connect to a HID host unless HID device is started */

    OI_AT_ERROR                            = 2701, /**< AT: ERROR response */
    OI_AT_NO_CARRIER                       = 2702, /**< AT: NO CARRIER response */
    OI_AT_BUSY                             = 2703, /**< AT: BUSY response */
    OI_AT_NO_ANSWER                        = 2704, /**< AT: NO ANSWER response */
    OI_AT_DELAYED                          = 2705, /**< AT: DELAYED response */
    OI_AT_BLACKLISTED                      = 2706, /**< AT: BLACKLISTED response */
    OI_AT_CME_ERROR                        = 2707, /**< AT: +CME ERROR response */
    OI_AT_CMS_ERROR                        = 2708, /**< AT: +CMS ERROR response */

    OI_BLST_CHARACTER_TIMEOUT              = 2801, /**< BLST: Timeout expired while waiting for a character from the client. */
    OI_BLST_ACKNOWLDGE_TIMEOUT             = 2802, /**< BLST: Timeout expired while waiting for event acknowledgment from the client */
    OI_BLST_TX_NOT_READY                   = 2803, /**< BLST: BLST is not ready to send a BHAPI message to the client. */
    OI_BLST_TX_BUSY                        = 2804, /**< BLST: BLST transmit buffer is in use. */

    OI_AVDTP_CONNECTION_SEQ_ERROR          = 2901, /**< AVDTP: sequencing of signalling/media channel connections broken. */
    OI_AVDTP_OUT_OF_RESOURCES              = 2902, /**< AVDTP: Tried to allocate too many endpoints or signalling channels. */

    OI_PBAP_REPOSITORY_NOT_SET             = 3001, /**< PBAP: Phonebook repository must be set for operation to complete. */
    OI_PBAP_PHONEBOOK_NOT_SET              = 3002, /**< PBAP: Phonebook be set for operation to complete. */

    OI_AADP_BAD_ENDPOINT                   = 3101, /**< AADP: Invalid local endpoint specified */
    OI_AADP_BAD_STATE                      = 3102, /**< AADP: AADP State is not correct for this operation. */

    OI_UNICODE_INVALID_SOURCE              = 3200, /**< Unicode Conversion: Source string has invalid character encoding. */
    OI_UNICODE_SOURCE_EXHAUSTED            = 3201, /**< Unicode Conversion: Incomplete Unicode character at end of source buffer. */
    OI_UNICODE_DESTINATION_EXHAUSTED       = 3202, /**< Unicode Conversion: Destination buffer not large enough to hold resulting Unicode string. */

    OI_AVRCP_TOO_MANY_CONNECTIONS          = 3300, /**< AVRCP: Exceeded maximum number of simultaneous AVCTP connections. */
    OI_AVRCP_NOT_IMPLEMENTED               = 3301, /**< AVRCP: The target does not implement the command specified by the opcode and operand. */
    OI_AVRCP_REJECTED                      = 3302, /**< AVRCP: The target cannot respond because of invalid operands in command packet. */
    OI_AVRCP_INVALID_RESPONSE              = 3303, /**< AVRCP: The controller received the response with invalid parameters */
    OI_AVRCP_RESPONSE_PACKET_OVERFLOW      = 3304, /**< AVRCP: The response message does not fir in one AVRCP packet (512 bytes), has to be fragmented. */
    OI_AVRCP_RESPONSE_INVALID_PDU          = 3305, /**< AVRCP: Command rejected: target received a PDU that it did not understand. */
    OI_AVRCP_RESPONSE_INVALID_PARAMETER    = 3306, /**< AVRCP: Command rejected: target received a PDU with a parameter ID that it did not understand. */
    OI_AVRCP_RESPONSE_PARAMETER_NOT_FOUND  = 3307, /**< AVRCP: Command rejected: specified parameter not found, sent if the parameter ID is understood, but content is wrong or corrupted.*/
    OI_AVRCP_RESPONSE_INTERNAL_ERROR       = 3308, /**< AVRCP: Command rejected: target detected other error conditions. */
    OI_MAX_BM3_STATUS_VAL,       /* Maximum BM3 status code */

    /* Status code values reserved for BM3 SDK platform-specific implementations */
    OI_STATUS_RESERVED_FOR_BCOT = 9000,

    /* Status code values reserved for BHAPI products */
    OI_STATUS_RESERVED_FOR_BHAPI = 9200,

    /* Status code values reserved for Soundabout products */
    OI_STATUS_RESERVED_FOR_SOUNDABOUT = 9400,

    /*
     * Status code values greater than or equal to this value are reserved for use by applications.
     * However, because of differences between compilers, and differences between 16-bit and 32-bit
     * platforms custom status codes should be in the 16-bit range, so status codes can range from 0
     * to 65534, inclusive (65535 is reserved)
     */
    OI_STATUS_RESERVED_FOR_APPS = 10000,



    OI_STATUS_NONE = 0xffff     /**< Special status code to indicate that there is no status. (Only to be used for special cases involving OI_SLOG_ERROR() and OI_SLOG_WARNING().) */

} OI_STATUS;


/* Remeber to update the #define below when new reserved blocks are added to
 * the list above. */
#define OI_NUM_RESERVED_STATUS_BLOCKS 4 /**< Number of status code blocks reserved, including user apps */


/**
 * Test for success
 */
#define OI_SUCCESS(x)    ((x) == OI_OK)

/*****************************************************************************/
#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* _OI_STATUS_H */

