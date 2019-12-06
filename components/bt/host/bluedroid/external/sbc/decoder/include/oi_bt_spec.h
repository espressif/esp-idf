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
#ifndef _OI_BT_SPEC_H
#define _OI_BT_SPEC_H
/**
 * @file
 *
 * This file contains common definitions from the Bluetooth specification.
 *
 */

/**********************************************************************************
  $Revision: #1 $
***********************************************************************************/

#include "oi_stddefs.h"

/** \addtogroup Misc Miscellaneous APIs */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/** The maximum number of active slaves in a piconet. */
#define OI_BT_MAX_ACTIVE_SLAVES 7

/** the number of bytes in a Bluetooth device address (BD_ADDR) */
#define OI_BD_ADDR_BYTE_SIZE   6

/**
 * 48-bit Bluetooth device address
 *
 * Because 48-bit integers may not be supported on all platforms, the
 * address is defined as an array of bytes. This array is big-endian,
 * meaning that
 *  - array[0] contains bits 47-40,
 *  - array[1] contains bits 39-32,
 *  - array[2] contains bits 31-24,
 *  - array[3] contains bits 23-16,
 *  - array[4] contains bits 15-8, and
 *  - array[5] contains bits 7-0.
 */
typedef struct  {
    OI_UINT8    addr[OI_BD_ADDR_BYTE_SIZE] ;   /**< Bluetooth device address represented as an array of 8-bit values */
} OI_BD_ADDR ;

/**
 * @name Data types for working with UUIDs
 * UUIDs are 16 bytes (128 bits).
 *
 * To avoid having to pass around 128-bit values all the time, 32-bit and 16-bit
 * UUIDs are defined, along with a mapping from the shorter versions to the full
 * version.
 *
 * @{
 */

/**
 * 16-bit representation of a 128-bit UUID
 */
typedef OI_UINT16 OI_UUID16;

/**
 * 32-bit representation of a 128-bit UUID
 */
typedef OI_UINT32 OI_UUID32;

/**
 * number of bytes in a 128 bit UUID
 */
#define OI_BT_UUID128_SIZE  16

/**
 * number of bytes in IPv6 style addresses
 */
#define OI_BT_IPV6ADDR_SIZE  16

/**
 * type definition for a 128-bit UUID
 *
 * To simplify conversion between 128-bit UUIDs and 16-bit and 32-bit UUIDs,
 * the most significant 32 bits are stored with the same endian-ness as is
 * native on the target (local) device. The remainder of the 128-bit UUID is
 * stored as bytes in big-endian order.
 */
typedef struct {
    OI_UINT32 ms32bits;                                    /**< most significant 32 bits of 128-bit UUID */
    OI_UINT8 base[OI_BT_UUID128_SIZE - sizeof(OI_UINT32)]; /**< remainder of 128-bit UUID, array of 8-bit values */
} OI_UUID128;

/** @} */

/** number of bytes in a link key */
#define OI_BT_LINK_KEY_SIZE    16

/**
 * type definition for a baseband link key
 *
 * Because 128-bit integers may not be supported on all platforms, we define
 * link keys as an array of bytes. Unlike the Bluetooth device address,
 * the link key is stored in little-endian order, meaning that
 *  - array[0]  contains bits 0  - 7,
 *  - array[1]  contains bits 8  - 15,
 *  - array[2]  contains bits 16 - 23,
 *  - array[3]  contains bits 24 - 31,
 *  - array[4]  contains bits 32 - 39,
 *  - array[5]  contains bits 40 - 47,
 *  - array[6]  contains bits 48 - 55,
 *  - array[7]  contains bits 56 - 63,
 *  - array[8]  contains bits 64 - 71,
 *  - array[9]  contains bits 72 - 79,
 *  - array[10] contains bits 80 - 87,
 *  - array[11] contains bits 88 - 95,
 *  - array[12] contains bits 96 - 103,
 *  - array[13] contains bits 104- 111,
 *  - array[14] contains bits 112- 119, and
 *  - array[15] contains bits 120- 127.
 */
typedef struct {
    OI_UINT8    key[OI_BT_LINK_KEY_SIZE] ;   /**< link key represented as an array of 8-bit values */
} OI_LINK_KEY ;


/** Out-of-band data size - C and R values are 16-bytes each */
#define OI_BT_OOB_NUM_BYTES     16

typedef struct {
    OI_UINT8    value[OI_BT_OOB_NUM_BYTES] ;   /**< same struct used for C and R values */
} OI_OOB_DATA ;


/**
 * link key types
 */
typedef enum  {
    OI_LINK_KEY_TYPE_COMBO              = 0,    /**< combination key */
    OI_LINK_KEY_TYPE_LOCAL_UNIT         = 1,    /**< local unit key */
    OI_LINK_KEY_TYPE_REMOTE_UNIT        = 2,    /**< remote unit key */
    OI_LINK_KEY_TYPE_DEBUG_COMBO        = 3,    /**< debug combination key */
    OI_LINK_KEY_TYPE_UNAUTHENTICATED    = 4,    /**< Unauthenticated */
    OI_LINK_KEY_TYPE_AUTHENTICATED      = 5,    /**< Authenticated */
    OI_LINK_KEY_TYPE_CHANGED_COMBO      = 6     /**< Changed */

} OI_BT_LINK_KEY_TYPE ;


/** amount of space allocated for a PIN (personal indentification number) in bytes */
#define OI_BT_PIN_CODE_SIZE    16

/** data type for a PIN (PINs are treated as strings, so endianness does not apply.) */
typedef struct  {
    OI_UINT8    pin[OI_BT_PIN_CODE_SIZE] ; /**< PIN represented as an array of 8-bit values */
} OI_PIN_CODE ;

/** maximum number of SCO connections per device, which is 3 as of version 2.0+EDR
    of the Bluetooth specification (see sec 4.3 of vol 2 part B) */
#define OI_BT_MAX_SCO_CONNECTIONS  3

/** data type for clock offset */
typedef OI_UINT16   OI_BT_CLOCK_OFFSET ;

/** data type for a LM handle */
typedef OI_UINT16 OI_HCI_LM_HANDLE;

/** opaque data type for a SCO or ACL connection handle */
typedef struct _OI_HCI_CONNECTION *OI_HCI_CONNECTION_HANDLE;

/** data type for HCI Error Code, as defined in oi_hcispec.h */
typedef OI_UINT8    OI_HCI_ERROR_CODE ;

/**
 * The Bluetooth device type is indicated by a 24-bit bitfield, represented as a
 * 32-bit number in the stack. The bit layout and values for device class are specified
 * in the file oi_bt_assigned_nos.h and in the Bluetooth "Assigned Numbers" specification
 * at http://www.bluetooth.org/assigned-numbers/.
 */
typedef OI_UINT32   OI_BT_DEVICE_CLASS ;

#define OI_BT_DEV_CLASS_FORMAT_MASK        0x000003    /**< Bits 0-1 contain format type. */
#define OI_BT_DEV_CLASS_MINOR_DEVICE_MASK  0x0000FC    /**< Bits 2-7 contain minor device class value. */
#define OI_BT_DEV_CLASS_MAJOR_DEVICE_MASK  0x001F00    /**< Bits 8-12 contain major device class value. */
#define OI_BT_DEV_CLASS_MAJOR_SERVICE_MASK 0xFFE000    /**< Bits 13-23 contain major service class value. */

/** There is currently only one device class format defined, type 00. */
#define OI_BT_DEV_CLASS_FORMAT_TYPE        00

/** Bit 13 in device class indicates limited discoverability mode (GAP v2.0+EDR, section 4.1.2.2) */
#define OI_BT_DEV_CLASS_LIMITED_DISCO_BIT  BIT13

/** macro to test validity of the Device Class Format */
#define OI_BT_VALID_DEVICE_CLASS_FORMAT(class) (OI_BT_DEV_CLASS_FORMAT_TYPE == ((class) & OI_BT_DEV_CLASS_FORMAT_MASK))

/** the time between baseband clock ticks, currently 625 microseconds (one slot) */
#define OI_BT_TICK 625
/** some macros to convert to/from baseband clock ticks - use no floating point! */
#define OI_SECONDS_TO_BT_TICKS(secs)    ((secs)*1600)
#define OI_BT_TICKS_TO_SECONDS(ticks)   ((ticks)/1600)
#define OI_MSECS_TO_BT_TICKS(msecs)     (((msecs)*8)/5)
#define OI_BT_TICKS_TO_MSECS(ticks)     (((ticks)*5)/8)

/** EIR byte order */
#define OI_EIR_BYTE_ORDER   OI_LITTLE_ENDIAN_BYTE_ORDER


#ifdef __cplusplus
}
#endif

/**@}*/

/*****************************************************************************/
#endif /* _OI_BT_SPEC_H */
