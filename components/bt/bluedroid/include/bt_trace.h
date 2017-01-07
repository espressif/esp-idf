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

#ifndef _BT_TRACE_H_
#define _BT_TRACE_H_

#include <stdio.h>
#include "bt_types.h"

#include "esp_log.h"

#define TAG		"BT"

#define BT_PRINTF(fmt, ...)   ESP_LOGE(TAG, fmt, ##__VA_ARGS__)

#ifndef assert
#define assert(x)   do { if (!(x)) BT_PRINTF("bt host error %s %u\n", __FILE__, __LINE__); } while (0)
#endif

inline void trc_dump_buffer(const char *prefix, uint8_t *data, uint16_t len)
{
    uint16_t i;

    if (!data || !len) {
        return;
    }

    if (prefix) {
        BT_PRINTF("%s: len %d\n", prefix, len);
    }

    for (i = 0; i < len; i+=16) {
        BT_PRINTF("%02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x\n",
                *(data + i), *(data + i + 1), *(data + i + 2), *(data + i + 3), *(data + i + 4), *(data + i + 5), *(data + i + 6), *(data + i + 7), 
                *(data + i + 8), *(data + i + 9), *(data + i + 10), *(data + i + 11), *(data + i + 12), *(data + i + 13), *(data + i + 14), *(data + i + 15));
    }
    BT_PRINTF("\n");
}

#ifdef BTTRC_DUMP_BUFFER
#define BTTRC_DUMP_BUFFER(_prefix, _data, _len) trc_dump_buffer(_data, _len)
#else
#define BTTRC_DUMP_BUFFER(_prefix, _data, _len)
#endif

//static const char BTE_LOGMSG_MODULE[] = "bte_logmsg_module";

/* BTrgs);E tracing IDs for debug purposes */
/* LayerIDs for stack */
#define BTTRC_ID_STK_GKI                   1
#define BTTRC_ID_STK_BTU                   2
#define BTTRC_ID_STK_HCI                   3
#define BTTRC_ID_STK_L2CAP                 4
#define BTTRC_ID_STK_RFCM_MX               5
#define BTTRC_ID_STK_RFCM_PRT              6
#define BTTRC_ID_STK_OBEX_C                7
#define BTTRC_ID_STK_OBEX_S                8
#define BTTRC_ID_STK_AVCT                  9
#define BTTRC_ID_STK_AVDT                  10
#define BTTRC_ID_STK_AVRC                  11
#define BTTRC_ID_STK_BIC                   12
#define BTTRC_ID_STK_BIS                   13
#define BTTRC_ID_STK_BNEP                  14
#define BTTRC_ID_STK_BPP                   15
#define BTTRC_ID_STK_BTM_ACL               16
#define BTTRC_ID_STK_BTM_PM                17
#define BTTRC_ID_STK_BTM_DEV_CTRL          18
#define BTTRC_ID_STK_BTM_SVC_DSC           19
#define BTTRC_ID_STK_BTM_INQ               20
#define BTTRC_ID_STK_BTM_SCO               21
#define BTTRC_ID_STK_BTM_SEC               22
#define BTTRC_ID_STK_HID                   24
#define BTTRC_ID_STK_HSP2                  25
#define BTTRC_ID_STK_CTP                   26
#define BTTRC_ID_STK_FTC                   27
#define BTTRC_ID_STK_FTS                   28
#define BTTRC_ID_STK_GAP                   29
#define BTTRC_ID_STK_HCRP                  31
#define BTTRC_ID_STK_ICP                   32
#define BTTRC_ID_STK_OPC                   33
#define BTTRC_ID_STK_OPS                   34
#define BTTRC_ID_STK_PAN                   35
#define BTTRC_ID_STK_SAP                   36
#define BTTRC_ID_STK_SDP                   37
#define BTTRC_ID_STK_SLIP                  38
#define BTTRC_ID_STK_SPP                   39
#define BTTRC_ID_STK_TCS                   40
#define BTTRC_ID_STK_VDP                   41
#define BTTRC_ID_STK_MCAP                  42
#define BTTRC_ID_STK_GATT                  43
#define BTTRC_ID_STK_SMP                   44
#define BTTRC_ID_STK_NFC                   45
#define BTTRC_ID_STK_NCI                   46
#define BTTRC_ID_STK_IDEP                  47
#define BTTRC_ID_STK_NDEP                  48
#define BTTRC_ID_STK_LLCP                  49
#define BTTRC_ID_STK_RW                    50
#define BTTRC_ID_STK_CE                    51
#define BTTRC_ID_STK_SNEP                  52
#define BTTRC_ID_STK_NDEF                  53

/* LayerIDs for BTA */
#define BTTRC_ID_BTA_ACC                   55         /* Advanced Camera Client */
#define BTTRC_ID_BTA_AG                    56         /* audio gateway */
#define BTTRC_ID_BTA_AV                    57         /* Advanced audio */
#define BTTRC_ID_BTA_BIC                   58         /* Basic Imaging Client */
#define BTTRC_ID_BTA_BIS                   59         /* Basic Imaging Server */
#define BTTRC_ID_BTA_BP                    60         /* Basic Printing Client */
#define BTTRC_ID_BTA_CG                    61
#define BTTRC_ID_BTA_CT                    62         /* cordless telephony terminal */
#define BTTRC_ID_BTA_DG                    63         /* data gateway */
#define BTTRC_ID_BTA_DM                    64         /* device manager */
#define BTTRC_ID_BTA_DM_SRCH               65         /* device manager search */
#define BTTRC_ID_BTA_DM_SEC                66         /* device manager security */
#define BTTRC_ID_BTA_FM                    67
#define BTTRC_ID_BTA_FTC                   68         /* file transfer client */
#define BTTRC_ID_BTA_FTS                   69         /* file transfer server */
#define BTTRC_ID_BTA_HIDH                  70
#define BTTRC_ID_BTA_HIDD                  71
#define BTTRC_ID_BTA_JV                    72
#define BTTRC_ID_BTA_OPC                   73         /* object push client */
#define BTTRC_ID_BTA_OPS                   74         /* object push server */
#define BTTRC_ID_BTA_PAN                   75         /* Personal Area Networking */
#define BTTRC_ID_BTA_PR                    76         /* Printer client */
#define BTTRC_ID_BTA_SC                    77         /* SIM Card Access server */
#define BTTRC_ID_BTA_SS                    78         /* synchronization server */
#define BTTRC_ID_BTA_SYS                   79         /* system manager */
#define BTTRC_ID_AVDT_SCB                  80         /* avdt scb */
#define BTTRC_ID_AVDT_CCB                  81         /* avdt ccb */

// btla-specific ++
/* LayerIDs added for BTL-A. Probably should modify bte_logmsg.c in future. */
#define BTTRC_ID_STK_RFCOMM                82
#define BTTRC_ID_STK_RFCOMM_DATA           83
#define BTTRC_ID_STK_OBEX                  84
#define BTTRC_ID_STK_A2D                   85
#define BTTRC_ID_STK_BIP                   86

/* LayerIDs for BT APP */
#define BTTRC_ID_BTAPP                     87
#define BTTRC_ID_BT_PROTOCOL               88         /* this is a temporary solution to allow dynamic
                                                         enable/disable of BT_PROTOCOL_TRACE */
#define BTTRC_ID_MAX_ID                    BTTRC_ID_BT_PROTOCOL
// btla-specific --
#define BTTRC_ID_ALL_LAYERS                0xFF       /* all trace layers */
/* Parameter datatypes used in Trace APIs */
#define BTTRC_PARAM_UINT8                  1
#define BTTRC_PARAM_UINT16                 2
#define BTTRC_PARAM_UINT32                 3

/* Enables or disables verbose trace information. */
#ifndef BT_TRACE_VERBOSE
#define BT_TRACE_VERBOSE    FALSE
#endif

/* Enables or disables all trace messages. */
#ifndef BT_USE_TRACES
#define BT_USE_TRACES       TRUE
#endif

/******************************************************************************
**
** Trace Levels
**
** The following values may be used for different levels:
**      BT_TRACE_LEVEL_NONE    0        * No trace messages to be generated
**      BT_TRACE_LEVEL_ERROR   1        * Error condition trace messages
**      BT_TRACE_LEVEL_WARNING 2        * Warning condition trace messages
**      BT_TRACE_LEVEL_API     3        * API traces
**      BT_TRACE_LEVEL_EVENT   4        * Debug messages for events
**      BT_TRACE_LEVEL_DEBUG   5        * Debug messages (general)
******************************************************************************/

// btla-specific ++
/* Core Stack default trace levels */
#ifndef HCI_INITIAL_TRACE_LEVEL
#define HCI_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifndef BTM_INITIAL_TRACE_LEVEL
#define BTM_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifndef L2CAP_INITIAL_TRACE_LEVEL
#define L2CAP_INITIAL_TRACE_LEVEL           BT_TRACE_LEVEL_WARNING
#endif

#ifndef RFCOMM_INITIAL_TRACE_LEVEL
#define RFCOMM_INITIAL_TRACE_LEVEL          BT_TRACE_LEVEL_WARNING
#endif

#ifndef SDP_INITIAL_TRACE_LEVEL
#define SDP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifndef GAP_INITIAL_TRACE_LEVEL
#define GAP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifndef BNEP_INITIAL_TRACE_LEVEL
#define BNEP_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifndef PAN_INITIAL_TRACE_LEVEL
#define PAN_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifndef A2D_INITIAL_TRACE_LEVEL
#define A2D_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifndef AVDT_INITIAL_TRACE_LEVEL
#define AVDT_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifndef AVCT_INITIAL_TRACE_LEVEL
#define AVCT_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifndef AVRC_INITIAL_TRACE_LEVEL
#define AVRC_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifndef MCA_INITIAL_TRACE_LEVEL
#define MCA_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifndef HID_INITIAL_TRACE_LEVEL
#define HID_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifndef APPL_INITIAL_TRACE_LEVEL
#define APPL_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifndef BT_TRACE_APPL
#define BT_TRACE_APPL   BT_USE_TRACES
#endif

#ifndef GATT_INITIAL_TRACE_LEVEL
#define GATT_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifndef SMP_INITIAL_TRACE_LEVEL
#define SMP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif
// btla-specific --

/* Define common tracing for all */
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_WARN      2
#define LOG_LEVEL_INFO      3
#define LOG_LEVEL_DEBUG     4
#define LOG_LEVEL_VERBOSE   5
#ifndef LOG_LEVEL
#define LOG_LEVEL   LOG_LEVEL_INFO
#endif
#define LOG_ERROR(fmt, args...)         do {if (LOG_LEVEL >= LOG_LEVEL_ERROR) BT_PRINTF(fmt,## args);} while(0)
#define LOG_WARN(fmt, args...)          do {if (LOG_LEVEL >= LOG_LEVEL_WARN) BT_PRINTF(fmt,## args);} while(0)
#define LOG_INFO(fmt, args...)          do {if (LOG_LEVEL >= LOG_LEVEL_INFO) BT_PRINTF(fmt,## args);} while(0)
#define LOG_DEBUG(fmt, args...)         do {if (LOG_LEVEL >= LOG_LEVEL_DEBUG) BT_PRINTF(fmt,## args);} while(0)
#define LOG_VERBOSE(fmt, args...)       do {if (LOG_LEVEL >= LOG_LEVEL_VERBOSE) BT_PRINTF(fmt,## args);} while(0)

/* Define tracing for the HCI unit
*/
#define HCI_TRACE_ERROR(fmt, args...)       {if (btu_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt,## args);}
#define HCI_TRACE_WARNING(fmt, args...)     {if (btu_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt,## args);}
#define HCI_TRACE_EVENT(fmt, args...)       {if (btu_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt,## args);}
#define HCI_TRACE_DEBUG(fmt, args...)       {if (btu_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt,## args);}

/* Define tracing for BTM
*/
#define BTM_TRACE_ERROR(fmt, args...)       {if (btm_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define BTM_TRACE_WARNING(fmt, args...)     {if (btm_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define BTM_TRACE_API(fmt, args...)     {if (btm_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define BTM_TRACE_EVENT(fmt, args...)       {if (btm_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define BTM_TRACE_DEBUG(fmt, args...)       {if (btm_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}

/* Define tracing for the L2CAP unit
*/
#define L2CAP_TRACE_ERROR(fmt, args...)     {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define L2CAP_TRACE_WARNING(fmt, args...)   {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define L2CAP_TRACE_API(fmt, args...)       {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define L2CAP_TRACE_EVENT(fmt, args...)     {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define L2CAP_TRACE_DEBUG(fmt, args...)     {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}

/* Define tracing for the SDP unit
*/
#define SDP_TRACE_ERROR(fmt, args...)       {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define SDP_TRACE_WARNING(fmt, args...)     {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define SDP_TRACE_API(fmt, args...)     {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define SDP_TRACE_EVENT(fmt, args...)       {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define SDP_TRACE_DEBUG(fmt, args...)       {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}

/* Define tracing for the RFCOMM unit
*/
#define RFCOMM_TRACE_ERROR(fmt, args...)    {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define RFCOMM_TRACE_WARNING(fmt, args...)  {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define RFCOMM_TRACE_API(fmt, args...)      {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define RFCOMM_TRACE_EVENT(fmt, args...)    {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define RFCOMM_TRACE_DEBUG(fmt, args...)    {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}

/* Generic Access Profile traces */
#define GAP_TRACE_ERROR(fmt, args...)       {if (gap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define GAP_TRACE_EVENT(fmt, args...)       {if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define GAP_TRACE_API(fmt, args...)     {if (gap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define GAP_TRACE_WARNING(fmt, args...)     {if (gap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}

/* define traces for HID Host */
#define HIDH_TRACE_ERROR(fmt, args...)      {if (hh_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define HIDH_TRACE_WARNING(fmt, args...)    {if (hh_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define HIDH_TRACE_API(fmt, args...)        {if (hh_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define HIDH_TRACE_EVENT(fmt, args...)      {if (hh_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define HIDH_TRACE_DEBUG(fmt, args...)      {if (hh_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}

/* define traces for BNEP */

#define BNEP_TRACE_ERROR(fmt, args...)      {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define BNEP_TRACE_WARNING(fmt, args...)    {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define BNEP_TRACE_API(fmt, args...)        {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define BNEP_TRACE_EVENT(fmt, args...)      {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define BNEP_TRACE_DEBUG(fmt, args...)      {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}

/* define traces for PAN */

#define PAN_TRACE_ERROR(fmt, args...)       {if (pan_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define PAN_TRACE_WARNING(fmt, args...)     {if (pan_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define PAN_TRACE_API(fmt, args...)     {if (pan_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define PAN_TRACE_EVENT(fmt, args...)       {if (pan_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define PAN_TRACE_DEBUG(fmt, args...)       {if (pan_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}

/* Define tracing for the A2DP profile
*/
#define A2D_TRACE_ERROR(fmt, args...)       {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define A2D_TRACE_WARNING(fmt, args...)     {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define A2D_TRACE_EVENT(fmt, args...)       {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define A2D_TRACE_DEBUG(fmt, args...)       {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}
#define A2D_TRACE_API(fmt, args...)     {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}

/* AVDTP
*/
#define AVDT_TRACE_ERROR(fmt, args...)      {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define AVDT_TRACE_WARNING(fmt, args...)    {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define AVDT_TRACE_EVENT(fmt, args...)      {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define AVDT_TRACE_DEBUG(fmt, args...)      {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}
#define AVDT_TRACE_API(fmt, args...)        {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}

/* Define tracing for the AVCTP protocol
*/
#define AVCT_TRACE_ERROR(fmt, args...)      {if (avct_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define AVCT_TRACE_WARNING(fmt, args...)    {if (avct_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define AVCT_TRACE_EVENT(fmt, args...)      {if (avct_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define AVCT_TRACE_DEBUG(fmt, args...)      {if (avct_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}
#define AVCT_TRACE_API(fmt, args...)        {if (avct_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}

/* Define tracing for the AVRCP profile
*/
#define AVRC_TRACE_ERROR(fmt, args...)      {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define AVRC_TRACE_WARNING(fmt, args...)    {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define AVRC_TRACE_EVENT(fmt, args...)      {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define AVRC_TRACE_DEBUG(fmt, args...)      {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}
#define AVRC_TRACE_API(fmt, args...)        {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}

/* MCAP
*/
#define MCA_TRACE_ERROR(fmt, args...)       {if (mca_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define MCA_TRACE_WARNING(fmt, args...)     {if (mca_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define MCA_TRACE_EVENT(fmt, args...)       {if (mca_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define MCA_TRACE_DEBUG(fmt, args...)       {if (mca_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}
#define MCA_TRACE_API(fmt, args...)     {if (mca_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}

/* Define tracing for the ATT/GATT unit
*/
#define GATT_TRACE_ERROR(fmt, args...)      {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define GATT_TRACE_WARNING(fmt, args...)    {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define GATT_TRACE_API(fmt, args...)        {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define GATT_TRACE_EVENT(fmt, args...)      {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define GATT_TRACE_DEBUG(fmt, args...)      {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}

/* Define tracing for the SMP unit
*/
#define SMP_TRACE_ERROR(fmt, args...)       {if (smp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define SMP_TRACE_WARNING(fmt, args...)     {if (smp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define SMP_TRACE_API(fmt, args...)     {if (smp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define SMP_TRACE_EVENT(fmt, args...)       {if (smp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define SMP_TRACE_DEBUG(fmt, args...)       {if (smp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}

extern UINT8 btif_trace_level;

// define traces for application
#define BTIF_TRACE_ERROR(fmt, args...)      {if (btif_trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define BTIF_TRACE_WARNING(fmt, args...)    {if (btif_trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define BTIF_TRACE_API(fmt, args...)        {if (btif_trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define BTIF_TRACE_EVENT(fmt, args...)      {if (btif_trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define BTIF_TRACE_DEBUG(fmt, args...)      {if (btif_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}
#define BTIF_TRACE_VERBOSE(fmt, args...)    {if (btif_trace_level >= BT_TRACE_LEVEL_VERBOSE)BT_PRINTF(fmt, ## args);}

/* define traces for application */

#define APPL_TRACE_ERROR(fmt, args...)      {if (appl_trace_level >= BT_TRACE_LEVEL_ERROR) BT_PRINTF(fmt, ## args);}
#define APPL_TRACE_WARNING(fmt, args...)    {if (appl_trace_level >= BT_TRACE_LEVEL_WARNING) BT_PRINTF(fmt, ## args);}
#define APPL_TRACE_API(fmt, args...)        {if (appl_trace_level >= BT_TRACE_LEVEL_API) BT_PRINTF(fmt, ## args);}
#define APPL_TRACE_EVENT(fmt, args...)      {if (appl_trace_level >= BT_TRACE_LEVEL_EVENT) BT_PRINTF(fmt, ## args);}
#define APPL_TRACE_DEBUG(fmt, args...)      {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_PRINTF(fmt, ## args);}
#define APPL_TRACE_VERBOSE(fmt, args...)    {if (appl_trace_level >= BT_TRACE_LEVEL_VERBOSE) BT_PRINTF(fmt, ## args);}

/* Simplified Trace Helper Macro
*/
#define bdld(fmt, args...) \
    do{\
        if((MY_LOG_LEVEL) >= BT_TRACE_LEVEL_DEBUG) \
        BT_PRINTF(fmt, ## args);        \
    }while(0)

#define bdlw(fmt, args...) \
    do{\
        if((MY_LOG_LEVEL) >= BT_TRACE_LEVEL_DEBUG) \
        BT_PRINTF(fmt, ## args);        \
    }while(0)

#define bdle(fmt, args...) \
    do{\
        if((MY_LOG_LEVEL) >= BT_TRACE_LEVEL_DEBUG) \
        BT_PRINTF(fmt, ## args);        \
    }while(0)

#define bdla(assert_if) \
    do{\
        if(((MY_LOG_LEVEL) >= BT_TRACE_LEVEL_ERROR) && !(assert_if)) \
        BT_PRINTF("%s: assert failed\n", #assert_if);       \
    }while(0)

typedef UINT8 tBTTRC_PARAM_TYPE;
typedef UINT8 tBTTRC_LAYER_ID;
typedef UINT8 tBTTRC_TYPE;

typedef struct {
    tBTTRC_LAYER_ID layer_id;
    tBTTRC_TYPE     type;      /* TODO: use tBTTRC_TYPE instead of "classical level 0-5" */
} tBTTRC_LEVEL;

typedef UINT8 (tBTTRC_SET_TRACE_LEVEL)( UINT8 );

typedef struct {
    const tBTTRC_LAYER_ID         layer_id_start;
    const tBTTRC_LAYER_ID         layer_id_end;
    tBTTRC_SET_TRACE_LEVEL        *p_f;
    const char                    *trc_name;
    UINT8                         trace_level;
} tBTTRC_FUNC_MAP;

/* External declaration for appl_trace_level here to avoid to add the declaration in all the files using APPL_TRACExxx macros */
extern UINT8 appl_trace_level;

#endif /*_BT_TRACE_H_*/
