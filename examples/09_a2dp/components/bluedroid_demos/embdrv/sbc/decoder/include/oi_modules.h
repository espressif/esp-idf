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
#ifndef _OI_MODULES_H
#define _OI_MODULES_H
/**
 * @file
 *
 * Enumeration type defining the inidivual stack components.
 *
 */

/**********************************************************************************
  $Revision: #1 $
***********************************************************************************/


/** \addtogroup Misc Miscellaneous APIs */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif


/**
 * This enumeration lists constants for referencing the components of
 * the BLUEmagic 3.0 protocol stack, profiles, and other functionalities.
 *
 * In order to distinguish types of modules, items are grouped with markers to
 * delineate start and end of the groups
 *
 * The module type is used for various purposes:
 *      identification in debug print statements
 *      access to initialization flags
 *      access to the configuration table
 */

typedef enum {
    /* profiles and protocols  --> Updates to oi_debug.c and oi_config_table.c */

                                /*   XX --> Keep Enum values up-to-date! */
    OI_MODULE_AT,               /**< 00 AT command processing */
    OI_MODULE_A2DP,             /**< 01 Advanced Audio Distribution Profile */
    OI_MODULE_AVCTP,            /**< 02 Audio-Visual Control Transport Profile */
    OI_MODULE_AVDTP,            /**< 03 Audio-Visual Distribution Protocol */
    OI_MODULE_AVRCP,            /**< 04 Audio-Visual Remote Control Profile */
    OI_MODULE_BIP_CLI,          /**< 05 Basic Imaging Profile protocol client */
    OI_MODULE_BIP_SRV,          /**< 06 Basic Imaging Profile protocol server */
    OI_MODULE_BNEP,             /**< 07 Bluetooth Network Encapsulation Protocol */
    OI_MODULE_BPP_SENDER,       /**< 08 Basic Printing Profile */
    OI_MODULE_BPP_PRINTER,      /**< 09 Basic Printing Profile */
    OI_MODULE_CTP,              /**< 10 Cordless Telephony Profile */
    OI_MODULE_DUN,              /**< 11 Dial-Up Networking Profile */
    OI_MODULE_FAX,              /**< 12 Fax Profile */
    OI_MODULE_FTP_CLI,          /**< 13 File Transfer Profile protocol client */
    OI_MODULE_FTP_SRV,          /**< 14 File Transfer Profile protocol server */
    OI_MODULE_HANDSFREE,        /**< 15 Hands-Free Profile */
    OI_MODULE_HANDSFREE_AG,     /**< 16 Hands-Free Profile */
    OI_MODULE_HCRP_CLI,         /**< 17 Hardcopy Cable Replacement Profile */
    OI_MODULE_HCRP_SRV,         /**< 18 Hardcopy Cable Replacement Profile */
    OI_MODULE_HEADSET,          /**< 19 Headset Profile */
    OI_MODULE_HEADSET_AG,       /**< 20 Headset Profile */
    OI_MODULE_HID,              /**< 21 Human Interface Device profile */
    OI_MODULE_INTERCOM,         /**< 22 Intercom Profile */
    OI_MODULE_OBEX_CLI,         /**< 23 OBEX protocol client, Generic Object Exchange Profile */
    OI_MODULE_OBEX_SRV,         /**< 24 OBEX protocol server, Generic Object Exchange Profile */
    OI_MODULE_OPP_CLI,          /**< 25 Object Push Profile protocol client */
    OI_MODULE_OPP_SRV,          /**< 26 Object Push Profile protocol server */
    OI_MODULE_PAN,              /**< 27 PAN profile */
    OI_MODULE_PBAP_CLI,         /**< 28 Phonebook Access Profile client */
    OI_MODULE_PBAP_SRV,         /**< 29 Phonebook Access Profile server */
    OI_MODULE_SAP_CLI,          /**< 30 SIM Access Profile */
    OI_MODULE_SAP_SRV,          /**< 31 SIM Access Profile */
    OI_MODULE_SPP,              /**< 32 Serial Port Profile */
    OI_MODULE_SYNC_CLI,         /**< 33 Synchronization Profile */
    OI_MODULE_SYNC_SRV,         /**< 34 Synchronization Profile */
    OI_MODULE_SYNC_CMD_CLI,     /**< 35 Synchronization Profile */
    OI_MODULE_SYNC_CMD_SRV,     /**< 36 Synchronization Profile */
    OI_MODULE_SYNCML,           /**< 37 SyncML Profile */
    OI_MODULE_TCS,              /**< 38 TCS Binary */
    OI_MODULE_VDP,              /**< 39 Video Distribution Profile */

    /* corestack components   --> Updates to oi_debug.c and oi_config_table.c */

    OI_MODULE_COMMON_CONFIG,    /**< 40 Common configuration, module has no meaning other than for config struct */
    OI_MODULE_CMDCHAIN,         /**< 41 Command chaining utility */
    OI_MODULE_DISPATCH,         /**< 42 Dispatcher */
    OI_MODULE_DATAELEM,         /**< 43 Data Elements, marshaller */
    OI_MODULE_DEVMGR,           /**< 44 Device Manager */
    OI_MODULE_DEVMGR_MODES,     /**< 45 Device Manager connectability/discoverability modes */
    OI_MODULE_HCI,              /**< 46 Host Controller Interface command layer */
    OI_MODULE_L2CAP,            /**< 47 L2CAP */
    OI_MODULE_MEMMGR,           /**< 48 modules that do memory management */
    OI_MODULE_POLICYMGR,        /**< 49 Policy Manager */
    OI_MODULE_RFCOMM,           /**< 50 RFCOMM */
    OI_MODULE_RFCOMM_SD,        /**< 51 RFCOMM Service discovery */
    OI_MODULE_SDP_CLI,          /**< 52 Service Discovery Protocol client */
    OI_MODULE_SDP_SRV,          /**< 53 Service Discovery Protocol server */
    OI_MODULE_SDPDB,            /**< 54 Service Discovery Protocol database */
    OI_MODULE_SECMGR,           /**< 55 Security Manager */
    OI_MODULE_SNIFFLOG,         /**< 56 sniff log */
    OI_MODULE_SUPPORT,          /**< 57 support functions, including CThru Dispatcher, time functions, and stack initialization */
    OI_MODULE_TRANSPORT,        /**< 58 transport layer between HCI command layer and driver */
    OI_MODULE_TEST,             /**< 59 used to debug output from internal test programs */
    OI_MODULE_XML,              /**< 60 XML/CSS parser */

    OI_MODULE_DI,               /**< 61 Device Identification Profile */

    // bhapi components  --> Updates to oi_debug.c

    OI_MODULE_BHAPI,            /**< 62 BLUEmagic Host API generic */
    OI_MODULE_BHCLI,            /**< 63 BLUEmagic Host API client side */
    OI_MODULE_BHSRV,            /**< 64 BLUEmagic Host API server side */
    OI_MODULE_MSGQ,             /**< 65 module that handles message queuing */
    OI_MODULE_BHAPI_TRANSPORT,  /**< 66 module that handles message queuing */
    OI_MODULE_BLST_SRV,         /**< 67 module that provides server side BHAPI Lightweight Serial Transport */
    OI_MODULE_BLST_CLI,         /**< 68 module that provides client side BHAPI Lightweight Serial Transport */

    // OEM files --> Updates to oi_debug.c
    OI_MODULE_OEM,              /**< 69 Application Memory allocation */

    // Application glue --> Updates to oi_debug.c
    OI_MODULE_APP,              /**< 70 Application Memory allocation */

    /* various pieces of code depend on these last 2 elements occuring in a specific order:
       OI_MODULE_ALL must be the 2nd to last element
       OI_MODULE_UNKNOWN must be the last element
       */
    OI_MODULE_ALL,              /**< 71 special value identifying all modules - used for control of debug print statements */
    OI_MODULE_UNKNOWN           /**< 72 special value - used for debug print statements */
} OI_MODULE;

/**
 * This constant is the number of actual modules in the list.  ALL and UNKNOWN are
 * special values that are not actually modules.
 * Used for debug print and memmgr profiling
 */
#define OI_NUM_MODULES  OI_MODULE_ALL


/**
 * This constant is the number of profile and core components.  It is used to size
 * the initialization and configuration tables.
 */
#define OI_NUM_STACK_MODULES    OI_MODULE_BHAPI


#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* _OI_MODULES_H */

