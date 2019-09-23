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
 *  This file contains internally used ATT definitions
 *
 ******************************************************************************/

#ifndef  _GATTDEFS_H
#define  _GATTDEFS_H

#define GATT_ILLEGAL_UUID            0

/* GATT attribute types
*/
#define GATT_UUID_PRI_SERVICE           0x2800
#define GATT_UUID_SEC_SERVICE           0x2801
#define GATT_UUID_INCLUDE_SERVICE       0x2802
#define GATT_UUID_CHAR_DECLARE          0x2803      /*  Characteristic Declaration*/

#define GATT_UUID_CHAR_EXT_PROP         0x2900      /*  Characteristic Extended Properties */
#define GATT_UUID_CHAR_DESCRIPTION      0x2901      /*  Characteristic User Description*/
#define GATT_UUID_CHAR_CLIENT_CONFIG    0x2902      /*  Client Characteristic Configuration */
#define GATT_UUID_CHAR_SRVR_CONFIG      0x2903      /*  Server Characteristic Configuration */
#define GATT_UUID_CHAR_PRESENT_FORMAT   0x2904      /*  Characteristic Presentation Format*/
#define GATT_UUID_CHAR_AGG_FORMAT       0x2905      /*  Characteristic Aggregate Format*/
#define GATT_UUID_CHAR_VALID_RANGE       0x2906      /*  Characteristic Valid Range */
#define GATT_UUID_EXT_RPT_REF_DESCR     0x2907
#define GATT_UUID_RPT_REF_DESCR         0x2908


/* GAP Profile Attributes
*/
#define GATT_UUID_GAP_DEVICE_NAME        0x2A00
#define GATT_UUID_GAP_ICON               0x2A01
#define GATT_UUID_GAP_PREF_CONN_PARAM    0x2A04
#define GATT_UUID_GAP_CENTRAL_ADDR_RESOL 0x2AA6

/* Attribute Profile Attribute UUID */
#define GATT_UUID_GATT_SRV_CHGD          0x2A05
/* Attribute Protocol Test */

/* Link Loss Service */
#define GATT_UUID_ALERT_LEVEL           0x2A06      /* Alert Level */
#define GATT_UUID_TX_POWER_LEVEL        0x2A07      /* TX power level */

/* Time Profile */
/* Current Time Service */
#define GATT_UUID_CURRENT_TIME          0x2A2B      /* Current Time */
#define GATT_UUID_LOCAL_TIME_INFO       0x2A0F      /* Local time info */
#define GATT_UUID_REF_TIME_INFO         0x2A14      /* reference time information */

/* NwA Profile */
#define GATT_UUID_NW_STATUS             0x2A18      /* network availability status */
#define GATT_UUID_NW_TRIGGER            0x2A1A      /* Network availability trigger */

/* phone alert */
#define GATT_UUID_ALERT_STATUS          0x2A3F    /* alert status */
#define GATT_UUID_RINGER_CP             0x2A40    /* ringer control point */
#define GATT_UUID_RINGER_SETTING        0x2A41    /* ringer setting */

/* Glucose Service */
#define GATT_UUID_GM_MEASUREMENT        0x2A18
#define GATT_UUID_GM_CONTEXT            0x2A34
#define GATT_UUID_GM_CONTROL_POINT      0x2A52
#define GATT_UUID_GM_FEATURE            0x2A51

/* device infor characteristic */
#define GATT_UUID_SYSTEM_ID             0x2A23
#define GATT_UUID_MODEL_NUMBER_STR      0x2A24
#define GATT_UUID_SERIAL_NUMBER_STR     0x2A25
#define GATT_UUID_FW_VERSION_STR        0x2A26
#define GATT_UUID_HW_VERSION_STR        0x2A27
#define GATT_UUID_SW_VERSION_STR        0x2A28
#define GATT_UUID_MANU_NAME             0x2A29
#define GATT_UUID_IEEE_DATA             0x2A2A
#define GATT_UUID_PNP_ID                0x2A50

/* HID characteristics */
#define GATT_UUID_HID_INFORMATION       0x2A4A
#define GATT_UUID_HID_REPORT_MAP        0x2A4B
#define GATT_UUID_HID_CONTROL_POINT     0x2A4C
#define GATT_UUID_HID_REPORT            0x2A4D
#define GATT_UUID_HID_PROTO_MODE        0x2A4E
#define GATT_UUID_HID_BT_KB_INPUT       0x2A22
#define GATT_UUID_HID_BT_KB_OUTPUT      0x2A32
#define GATT_UUID_HID_BT_MOUSE_INPUT    0x2A33

/* Battery Service char */
#define GATT_UUID_BATTERY_LEVEL         0x2A19

#define GATT_UUID_SC_CONTROL_POINT      0x2A55
#define GATT_UUID_SENSOR_LOCATION       0x2A5D

/* RUNNERS SPEED AND CADENCE SERVICE      */
#define GATT_UUID_RSC_MEASUREMENT       0x2A53
#define GATT_UUID_RSC_FEATURE           0x2A54

/* CYCLING SPEED AND CADENCE SERVICE      */
#define GATT_UUID_CSC_MEASUREMENT       0x2A5B
#define GATT_UUID_CSC_FEATURE           0x2A5C


/* Scan Parameter charatceristics */
#define GATT_UUID_SCAN_INT_WINDOW       0x2A4F
#define GATT_UUID_SCAN_REFRESH          0x2A31

#endif
