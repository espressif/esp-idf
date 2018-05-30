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

#include "sdkconfig.h"

#include <stdio.h>
#include "stack/bt_types.h"

#ifndef LOG_LOCAL_LEVEL
#ifndef BOOTLOADER_BUILD
#define LOG_LOCAL_LEVEL  CONFIG_LOG_DEFAULT_LEVEL
#else
#define LOG_LOCAL_LEVEL  CONFIG_LOG_BOOTLOADER_LEVEL
#endif
#endif

#include "esp_log.h"

// Mapping between ESP_LOG_LEVEL and BT_TRACE_LEVEL
#if (LOG_LOCAL_LEVEL >= 4)
#define LOG_LOCAL_LEVEL_MAPPING (LOG_LOCAL_LEVEL+1)
#else
#define LOG_LOCAL_LEVEL_MAPPING LOG_LOCAL_LEVEL
#endif

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define BT_LOG_LEVEL_CHECK(LAYER, LEVEL) (MAX(LAYER##_INITIAL_TRACE_LEVEL, LOG_LOCAL_LEVEL_MAPPING) >= BT_TRACE_LEVEL_##LEVEL)

//#define TAG		"BT"

#define BT_PRINT_E(tag, format, ...)   {esp_log_write(ESP_LOG_ERROR,   tag, LOG_FORMAT(E, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BT_PRINT_W(tag, format, ...)   {esp_log_write(ESP_LOG_WARN,    tag, LOG_FORMAT(W, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BT_PRINT_I(tag, format, ...)   {esp_log_write(ESP_LOG_INFO,    tag, LOG_FORMAT(I, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BT_PRINT_D(tag, format, ...)   {esp_log_write(ESP_LOG_DEBUG,   tag, LOG_FORMAT(D, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }
#define BT_PRINT_V(tag, format, ...)   {esp_log_write(ESP_LOG_VERBOSE, tag, LOG_FORMAT(V, format), esp_log_timestamp(), tag, ##__VA_ARGS__); }

#ifndef assert
#define assert(x)   do { if (!(x)) BT_PRINT_E("bt host error %s %u\n", __FILE__, __LINE__); } while (0)
#endif

inline void trc_dump_buffer(const char *prefix, uint8_t *data, uint16_t len)
{
    uint16_t i;

    if (!data || !len) {
        return;
    }

    if (prefix) {
        printf("%s: len %d\r\n", prefix, len);
    }

    for (i = 0; i < len; i+=16) {
        printf("%02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x\r\n",
                *(data + i), *(data + i + 1), *(data + i + 2), *(data + i + 3), *(data + i + 4), *(data + i + 5), *(data + i + 6), *(data + i + 7), 
                *(data + i + 8), *(data + i + 9), *(data + i + 10), *(data + i + 11), *(data + i + 12), *(data + i + 13), *(data + i + 14), *(data + i + 15));
    }
    printf("\r\n");
}

#ifdef BTTRC_DUMP_BUFFER
#define BTTRC_DUMP_BUFFER(_prefix, _data, _len) trc_dump_buffer(_prefix, _data, _len)
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
#define BT_USE_TRACES       FALSE
#endif

#ifndef BT_TRACE_APPL
#define BT_TRACE_APPL   BT_USE_TRACES
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
#ifdef  CONFIG_HCI_INITIAL_TRACE_LEVEL
#define HCI_INITIAL_TRACE_LEVEL             CONFIG_HCI_INITIAL_TRACE_LEVEL
#else
#define HCI_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_BTM_INITIAL_TRACE_LEVEL
#define BTM_INITIAL_TRACE_LEVEL             CONFIG_BTM_INITIAL_TRACE_LEVEL
#else
#define BTM_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_L2CAP_INITIAL_TRACE_LEVEL
#define L2CAP_INITIAL_TRACE_LEVEL           CONFIG_L2CAP_INITIAL_TRACE_LEVEL
#else
#define L2CAP_INITIAL_TRACE_LEVEL           BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_RFCOMM_INITIAL_TRACE_LEVEL
#define RFCOMM_INITIAL_TRACE_LEVEL          CONFIG_RFCOMM_INITIAL_TRACE_LEVEL
#else
#define RFCOMM_INITIAL_TRACE_LEVEL          BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_SDP_INITIAL_TRACE_LEVEL
#define SDP_INITIAL_TRACE_LEVEL             CONFIG_SDP_INITIAL_TRACE_LEVEL
#else
#define SDP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_GAP_INITIAL_TRACE_LEVEL
#define GAP_INITIAL_TRACE_LEVEL             CONFIG_GAP_INITIAL_TRACE_LEVEL
#else
#define GAP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_BNEP_INITIAL_TRACE_LEVEL
#define BNEP_INITIAL_TRACE_LEVEL            CONFIG_BNEP_INITIAL_TRACE_LEVEL
#else
#define BNEP_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_PAN_INITIAL_TRACE_LEVEL
#define PAN_INITIAL_TRACE_LEVEL             CONFIG_PAN_INITIAL_TRACE_LEVEL
#else
#define PAN_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_A2D_INITIAL_TRACE_LEVEL
#define A2D_INITIAL_TRACE_LEVEL             CONFIG_A2D_INITIAL_TRACE_LEVEL
#else
#define A2D_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_AVDT_INITIAL_TRACE_LEVEL
#define AVDT_INITIAL_TRACE_LEVEL            CONFIG_AVDT_INITIAL_TRACE_LEVEL
#else
#define AVDT_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_AVCT_INITIAL_TRACE_LEVEL
#define AVCT_INITIAL_TRACE_LEVEL            CONFIG_AVCT_INITIAL_TRACE_LEVEL
#else
#define AVCT_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_AVRC_INITIAL_TRACE_LEVEL
#define AVRC_INITIAL_TRACE_LEVEL            CONFIG_AVRC_INITIAL_TRACE_LEVEL
#else
#define AVRC_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_MCA_INITIAL_TRACE_LEVEL
#define MCA_INITIAL_TRACE_LEVEL             CONFIG_MCA_INITIAL_TRACE_LEVEL
#else
#define MCA_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_HID_INITIAL_TRACE_LEVEL
#define HID_INITIAL_TRACE_LEVEL             CONFIG_HID_INITIAL_TRACE_LEVEL
#else
#define HID_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_APPL_INITIAL_TRACE_LEVEL
#define APPL_INITIAL_TRACE_LEVEL            CONFIG_APPL_INITIAL_TRACE_LEVEL
#else
#define APPL_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_GATT_INITIAL_TRACE_LEVEL
#define GATT_INITIAL_TRACE_LEVEL            CONFIG_GATT_INITIAL_TRACE_LEVEL
#else
#define GATT_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_SMP_INITIAL_TRACE_LEVEL
#define SMP_INITIAL_TRACE_LEVEL             CONFIG_SMP_INITIAL_TRACE_LEVEL
#else
#define SMP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_BTIF_INITIAL_TRACE_LEVEL
#define BTIF_INITIAL_TRACE_LEVEL            CONFIG_BTIF_INITIAL_TRACE_LEVEL
#else
#define BTIF_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_BTC_INITIAL_TRACE_LEVEL
#define BTC_INITIAL_TRACE_LEVEL             CONFIG_BTC_INITIAL_TRACE_LEVEL
#else
#define BTC_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_OSI_INITIAL_TRACE_LEVEL
#define OSI_INITIAL_TRACE_LEVEL            CONFIG_OSI_INITIAL_TRACE_LEVEL
#else
#define OSI_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

#ifdef  CONFIG_BLUFI_INITIAL_TRACE_LEVEL
#define BLUFI_INITIAL_TRACE_LEVEL            CONFIG_BLUFI_INITIAL_TRACE_LEVEL
#else
#define BLUFI_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_WARNING
#endif

// btla-specific --

#if !CONFIG_BT_STACK_NO_LOG
#define LOG_ERROR(format, ... )             {if (LOG_LOCAL_LEVEL >= ESP_LOG_ERROR)    esp_log_write(ESP_LOG_ERROR,   "BT_LOG", LOG_FORMAT(E, format), esp_log_timestamp(), "BT_LOG", ##__VA_ARGS__); }
#define LOG_WARN(format, ... )              {if (LOG_LOCAL_LEVEL >= ESP_LOG_WARN)     esp_log_write(ESP_LOG_WARN,    "BT_LOG", LOG_FORMAT(W, format), esp_log_timestamp(), "BT_LOG", ##__VA_ARGS__); }
#define LOG_INFO(format, ... )              {if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO)     esp_log_write(ESP_LOG_INFO,    "BT_LOG", LOG_FORMAT(I, format), esp_log_timestamp(), "BT_LOG", ##__VA_ARGS__); }
#define LOG_DEBUG(format, ... )             {if (LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG)    esp_log_write(ESP_LOG_DEBUG,   "BT_LOG", LOG_FORMAT(D, format), esp_log_timestamp(), "BT_LOG", ##__VA_ARGS__); }
#define LOG_VERBOSE(format, ... )           {if (LOG_LOCAL_LEVEL >= ESP_LOG_VERBOSE)  esp_log_write(ESP_LOG_VERBOSE, "BT_LOG", LOG_FORMAT(V, format), esp_log_timestamp(), "BT_LOG", ##__VA_ARGS__); }

/* Define tracing for BTM
*/
#define BTM_TRACE_ERROR(fmt, args...)       {if (btm_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(BTM, ERROR)) BT_PRINT_E("BT_BTM", fmt, ## args);}
#define BTM_TRACE_WARNING(fmt, args...)     {if (btm_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(BTM, WARNING)) BT_PRINT_W("BT_BTM", fmt, ## args);}
#define BTM_TRACE_API(fmt, args...)         {if (btm_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(BTM,API)) BT_PRINT_I("BT_BTM", fmt, ## args);}
#define BTM_TRACE_EVENT(fmt, args...)       {if (btm_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(BTM,EVENT)) BT_PRINT_D("BT_BTM", fmt, ## args);}
#define BTM_TRACE_DEBUG(fmt, args...)       {if (btm_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(BTM,DEBUG)) BT_PRINT_D("BT_BTM", fmt, ## args);}

/* Define tracing for the L2CAP unit
*/
#define L2CAP_TRACE_ERROR(fmt, args...)     {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(L2CAP, ERROR)) BT_PRINT_E("BT_L2CAP", fmt, ## args);}
#define L2CAP_TRACE_WARNING(fmt, args...)   {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(L2CAP, WARNING)) BT_PRINT_W("BT_L2CAP", fmt, ## args);}
#define L2CAP_TRACE_API(fmt, args...)       {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(L2CAP,API)) BT_PRINT_I("BT_L2CAP", fmt, ## args);}
#define L2CAP_TRACE_EVENT(fmt, args...)     {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(L2CAP,EVENT)) BT_PRINT_D("BT_L2CAP", fmt, ## args);}
#define L2CAP_TRACE_DEBUG(fmt, args...)     {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(L2CAP,DEBUG)) BT_PRINT_D("BT_L2CAP", fmt, ## args);}

/* Define tracing for the SDP unit
*/
#define SDP_TRACE_ERROR(fmt, args...)       {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(SDP, ERROR)) BT_PRINT_E("BT_SDP", fmt, ## args);}
#define SDP_TRACE_WARNING(fmt, args...)     {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(SDP, WARNING)) BT_PRINT_W("BT_SDP", fmt, ## args);}
#define SDP_TRACE_API(fmt, args...)         {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(SDP,API)) BT_PRINT_I("BT_SDP", fmt, ## args);}
#define SDP_TRACE_EVENT(fmt, args...)       {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(SDP,EVENT)) BT_PRINT_D("BT_SDP", fmt, ## args);}
#define SDP_TRACE_DEBUG(fmt, args...)       {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(SDP,DEBUG)) BT_PRINT_D("BT_SDP", fmt, ## args);}

/* Define tracing for the RFCOMM unit
*/
#define RFCOMM_TRACE_ERROR(fmt, args...)    {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(RFCOMM, ERROR)) BT_PRINT_E("BT_RFCOMM", fmt, ## args);}
#define RFCOMM_TRACE_WARNING(fmt, args...)  {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(RFCOMM, WARNING)) BT_PRINT_W("BT_RFCOMM", fmt, ## args);}
#define RFCOMM_TRACE_API(fmt, args...)      {if (rfc_cb.trace_level >=BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(RFCOMM,API)) BT_PRINT_I("BT_RFCOMM", fmt, ## args);}
#define RFCOMM_TRACE_EVENT(fmt, args...)    {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(RFCOMM,EVENT)) BT_PRINT_D("BT_RFCOMM", fmt, ## args);}
#define RFCOMM_TRACE_DEBUG(fmt, args...)    {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(RFCOMM,DEBUG)) BT_PRINT_D("BT_RFCOMM", fmt, ## args);}

/* Generic Access Profile traces */
#define GAP_TRACE_ERROR(fmt, args...)       {if (gap_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(GAP, ERROR)) BT_PRINT_E("BT_GAP", fmt, ## args);}
#define GAP_TRACE_API(fmt, args...)         {if (gap_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(GAP,API)) BT_PRINT_I("BT_GAP", fmt, ## args);}
#define GAP_TRACE_EVENT(fmt, args...)       {if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(GAP,EVENT)) BT_PRINT_D("BT_GAP", fmt, ## args);}
#define GAP_TRACE_WARNING(fmt, args...)     {if (gap_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(GAP, WARNING)) BT_PRINT_W("BT_GAP", fmt, ## args);}

/* define traces for HID Host */
#define HIDH_TRACE_ERROR(fmt, args...)      {if (hh_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(HIDH, ERROR)) BT_PRINT_E("BT_HIDH", fmt, ## args);}
#define HIDH_TRACE_WARNING(fmt, args...)    {if (hh_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(HIDH, WARNING)) BT_PRINT_W("BT_HIDH", fmt, ## args);}
#define HIDH_TRACE_API(fmt, args...)        {if (hh_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(HIDH,API)) BT_PRINT_I("BT_HIDH", fmt, ## args);}
#define HIDH_TRACE_EVENT(fmt, args...)      {if (hh_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(HIDH,EVENT)) BT_PRINT_D("BT_HIDH", fmt, ## args);}
#define HIDH_TRACE_DEBUG(fmt, args...)      {if (hh_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(HIDH,DEBUG)) BT_PRINT_D("BT_HIDH", fmt, ## args);}

/* define traces for BNEP */

#define BNEP_TRACE_ERROR(fmt, args...)      {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(BNEP, ERROR)) BT_PRINT_E("BT_BNEP", fmt, ## args);}
#define BNEP_TRACE_WARNING(fmt, args...)    {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(BNEP, WARNING)) BT_PRINT_W("BT_BNEP", fmt, ## args);}
#define BNEP_TRACE_API(fmt, args...)        {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(BNEP,API)) BT_PRINT_I("BT_BNEP", fmt, ## args);}
#define BNEP_TRACE_EVENT(fmt, args...)      {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(BNEP,EVENT)) BT_PRINT_D("BT_BNEP", fmt, ## args);}
#define BNEP_TRACE_DEBUG(fmt, args...)      {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(BNEP,DEBUG)) BT_PRINT_D("BT_BNEP", fmt, ## args);}

/* define traces for PAN */

#define PAN_TRACE_ERROR(fmt, args...)       {if (pan_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(PAN, ERROR)) BT_PRINT_E("BT_PAN", fmt, ## args);}
#define PAN_TRACE_WARNING(fmt, args...)     {if (pan_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(PAN, WARNING)) BT_PRINT_W("BT_PAN", fmt, ## args);}
#define PAN_TRACE_API(fmt, args...)         {if (pan_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(PAN,API)) BT_PRINT_I("BT_PAN", fmt, ## args);}
#define PAN_TRACE_EVENT(fmt, args...)       {if (pan_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(PAN,EVENT)) BT_PRINT_D("BT_PAN", fmt, ## args);}
#define PAN_TRACE_DEBUG(fmt, args...)       {if (pan_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(PAN,DEBUG)) BT_PRINT_D("BT_PAN", fmt, ## args);}

/* Define tracing for the A2DP profile
*/
#define A2D_TRACE_ERROR(fmt, args...)       {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(A2D, ERROR)) BT_PRINT_E("BT_A2D", fmt, ## args);}
#define A2D_TRACE_WARNING(fmt, args...)     {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(A2D, WARNING)) BT_PRINT_W("BT_A2D", fmt, ## args);}
#define A2D_TRACE_API(fmt, args...)         {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(A2D,API)) BT_PRINT_I("BT_A2D", fmt, ## args);}
#define A2D_TRACE_EVENT(fmt, args...)       {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(A2D,EVENT)) BT_PRINT_D("BT_A2D", fmt, ## args);}
#define A2D_TRACE_DEBUG(fmt, args...)       {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(A2D,DEBUG)) BT_PRINT_D("BT_A2D", fmt, ## args);}

/* AVDTP
*/
#define AVDT_TRACE_ERROR(fmt, args...)      {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(AVDT, ERROR)) BT_PRINT_E("BT_AVDT", fmt, ## args);}
#define AVDT_TRACE_WARNING(fmt, args...)    {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(AVDT, WARNING)) BT_PRINT_W("BT_AVDT", fmt, ## args);}
#define AVDT_TRACE_API(fmt, args...)        {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(AVDT,API)) BT_PRINT_I("BT_AVDT", fmt, ## args);}
#define AVDT_TRACE_EVENT(fmt, args...)      {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(AVDT,EVENT)) BT_PRINT_D("BT_AVDT", fmt, ## args);}
#define AVDT_TRACE_DEBUG(fmt, args...)      {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(AVDT,DEBUG)) BT_PRINT_D("BT_AVDT", fmt, ## args);}

/* Define tracing for the AVCTP protocol
*/
#define AVCT_TRACE_ERROR(fmt, args...)      {if (avct_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(AVCT, ERROR)) BT_PRINT_E("BT_AVCT", fmt, ## args);}
#define AVCT_TRACE_WARNING(fmt, args...)    {if (avct_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(AVCT, WARNING)) BT_PRINT_W("BT_AVCT", fmt, ## args);}
#define AVCT_TRACE_API(fmt, args...)        {if (avct_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(AVCT,API)) BT_PRINT_I("BT_AVCT", fmt, ## args);}
#define AVCT_TRACE_EVENT(fmt, args...)      {if (avct_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(AVCT,EVENT)) BT_PRINT_D("BT_AVCT", fmt, ## args);}
#define AVCT_TRACE_DEBUG(fmt, args...)      {if (avct_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(AVCT,DEBUG)) BT_PRINT_D("BT_AVCT", fmt, ## args);}

/* Define tracing for the AVRCP profile
*/
#define AVRC_TRACE_ERROR(fmt, args...)      {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(AVRC, ERROR)) BT_PRINT_E("BT_AVRC", fmt, ## args);}
#define AVRC_TRACE_WARNING(fmt, args...)    {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(AVRC, WARNING)) BT_PRINT_W("BT_AVRC", fmt, ## args);}
#define AVRC_TRACE_API(fmt, args...)        {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(AVRC,API)) BT_PRINT_I("BT_AVRC", fmt, ## args);}
#define AVRC_TRACE_EVENT(fmt, args...)      {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(AVRC,EVENT)) BT_PRINT_D("BT_AVRC", fmt, ## args);}
#define AVRC_TRACE_DEBUG(fmt, args...)      {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(AVRC,DEBUG)) BT_PRINT_D("BT_AVRC", fmt, ## args);}

/* MCAP
*/
#define MCA_TRACE_ERROR(fmt, args...)       {if (mca_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(MCA, ERROR)) BT_PRINT_E("BT_MCA", fmt, ## args);}
#define MCA_TRACE_WARNING(fmt, args...)     {if (mca_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(MCA, WARNING)) BT_PRINT_W("BT_MCA", fmt, ## args);}
#define MCA_TRACE_API(fmt, args...)         {if (mca_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(MCA,API)) BT_PRINT_I("BT_MCA", fmt, ## args);}
#define MCA_TRACE_EVENT(fmt, args...)       {if (mca_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(MCA,EVENT)) BT_PRINT_D("BT_MCA", fmt, ## args);}
#define MCA_TRACE_DEBUG(fmt, args...)       {if (mca_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(MCA,DEBUG)) BT_PRINT_D("BT_MCA", fmt, ## args);}

/* Define tracing for the ATT/GATT unit
*/
#define GATT_TRACE_ERROR(fmt, args...)      {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(GATT, ERROR)) BT_PRINT_E("BT_GATT", fmt, ## args);}
#define GATT_TRACE_WARNING(fmt, args...)    {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(GATT, WARNING)) BT_PRINT_W("BT_GATT", fmt, ## args);}
#define GATT_TRACE_API(fmt, args...)        {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(GATT,API)) BT_PRINT_I("BT_GATT", fmt, ## args);}
#define GATT_TRACE_EVENT(fmt, args...)      {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(GATT,EVENT)) BT_PRINT_D("BT_GATT", fmt, ## args);}
#define GATT_TRACE_DEBUG(fmt, args...)      {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(GATT,DEBUG)) BT_PRINT_D("BT_GATT", fmt, ## args);}

/* Define tracing for the SMP unit
*/
#define SMP_TRACE_ERROR(fmt, args...)       {if (smp_cb.trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(SMP, ERROR)) BT_PRINT_E("BT_SMP", fmt, ## args);}
#define SMP_TRACE_WARNING(fmt, args...)     {if (smp_cb.trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(SMP, WARNING)) BT_PRINT_W("BT_SMP", fmt, ## args);}
#define SMP_TRACE_API(fmt, args...)         {if (smp_cb.trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(SMP,API)) BT_PRINT_I("BT_SMP", fmt, ## args);}
#define SMP_TRACE_EVENT(fmt, args...)       {if (smp_cb.trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(SMP,EVENT)) BT_PRINT_D("BT_SMP", fmt, ## args);}
#define SMP_TRACE_DEBUG(fmt, args...)       {if (smp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(SMP,DEBUG)) BT_PRINT_D("BT_SMP", fmt, ## args);}


extern UINT8 btif_trace_level;

// define traces for application
#define BTIF_TRACE_ERROR(fmt, args...)      {if (btif_trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(BTIF, ERROR)) BT_PRINT_E("BT_BTIF", fmt, ## args);}
#define BTIF_TRACE_WARNING(fmt, args...)    {if (btif_trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(BTIF, WARNING)) BT_PRINT_W("BT_BTIF", fmt, ## args);}
#define BTIF_TRACE_API(fmt, args...)        {if (btif_trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(BTIF,API)) BT_PRINT_I("BT_BTIF", fmt, ## args);}
#define BTIF_TRACE_EVENT(fmt, args...)      {if (btif_trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(BTIF,EVENT)) BT_PRINT_D("BT_BTIF", fmt, ## args);}
#define BTIF_TRACE_DEBUG(fmt, args...)      {if (btif_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(BTIF,DEBUG)) BT_PRINT_D("BT_BTIF", fmt, ## args);}
#define BTIF_TRACE_VERBOSE(fmt, args...)    {if (btif_trace_level >= BT_TRACE_LEVEL_VERBOSE && BT_LOG_LEVEL_CHECK(BTIF,VERBOSE)) BT_PRINT_V("BT_BTIF", fmt, ## args);}

/* define traces for application */

#define APPL_TRACE_ERROR(fmt, args...)      {if (appl_trace_level >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(APPL, ERROR)) BT_PRINT_E("BT_APPL", fmt, ## args);}
#define APPL_TRACE_WARNING(fmt, args...)    {if (appl_trace_level >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(APPL, WARNING)) BT_PRINT_W("BT_APPL", fmt, ## args);}
#define APPL_TRACE_API(fmt, args...)        {if (appl_trace_level >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(APPL,API)) BT_PRINT_I("BT_APPL", fmt, ## args);}
#define APPL_TRACE_EVENT(fmt, args...)      {if (appl_trace_level >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(APPL,EVENT)) BT_PRINT_D("BT_APPL", fmt, ## args);}
#define APPL_TRACE_DEBUG(fmt, args...)      {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(APPL,DEBUG)) BT_PRINT_D("BT_APPL", fmt, ## args);}
#define APPL_TRACE_VERBOSE(fmt, args...)    {if (appl_trace_level >= BT_TRACE_LEVEL_VERBOSE && BT_LOG_LEVEL_CHECK(APPL,VERBOSE)) BT_PRINT_V("BT_APPL", fmt, ## args);}

/* Define tracing for the HCI unit
 * Modified from `btu_cb.trace_level` to `HCI_INITIAL_TRACE_LEVEL`,
 * to use HCI_TRACE_XXXX in hci_layer.c without including `btu.h`
*/
#define HCI_TRACE_ERROR(fmt, args...)       {if (HCI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(HCI, ERROR)) BT_PRINT_E("BT_HCI", fmt,## args);}
#define HCI_TRACE_WARNING(fmt, args...)     {if (HCI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(HCI, WARNING)) BT_PRINT_W("BT_HCI", fmt,## args);}
#define HCI_TRACE_EVENT(fmt, args...)       {if (HCI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(HCI,EVENT)) BT_PRINT_D("BT_HCI", fmt,## args);}
#define HCI_TRACE_DEBUG(fmt, args...)       {if (HCI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(HCI,DEBUG)) BT_PRINT_D("BT_HCI", fmt,## args);}

/* define traces for BTC */
#define BTC_TRACE_ERROR(fmt, args...)      {if (BTC_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(BTC, ERROR)) BT_PRINT_E("BT_BTC", fmt, ## args);}
#define BTC_TRACE_WARNING(fmt, args...)    {if (BTC_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(BTC, WARNING)) BT_PRINT_W("BT_BTC", fmt, ## args);}
#define BTC_TRACE_API(fmt, args...)        {if (BTC_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(BTC,API)) BT_PRINT_I("BT_BTC", fmt, ## args);}
#define BTC_TRACE_EVENT(fmt, args...)      {if (BTC_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(BTC,EVENT)) BT_PRINT_D("BT_BTC", fmt, ## args);}
#define BTC_TRACE_DEBUG(fmt, args...)      {if (BTC_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(BTC,DEBUG)) BT_PRINT_D("BT_BTC", fmt, ## args);}
#define BTC_TRACE_VERBOSE(fmt, args...)    {if (BTC_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_VERBOSE && BT_LOG_LEVEL_CHECK(BTC,VERBOSE)) BT_PRINT_V("BT_BTC", fmt, ## args);}

/* define traces for OSI */
#define OSI_TRACE_ERROR(fmt, args...)      {if (OSI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(OSI, ERROR)) BT_PRINT_E("BT_OSI", fmt, ## args);}
#define OSI_TRACE_WARNING(fmt, args...)    {if (OSI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(OSI, WARNING)) BT_PRINT_W("BT_OSI", fmt, ## args);}
#define OSI_TRACE_API(fmt, args...)        {if (OSI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(OSI,API)) BT_PRINT_I("BT_OSI", fmt, ## args);}
#define OSI_TRACE_EVENT(fmt, args...)      {if (OSI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(OSI,EVENT)) BT_PRINT_D("BT_OSI", fmt, ## args);}
#define OSI_TRACE_DEBUG(fmt, args...)      {if (OSI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(OSI,DEBUG)) BT_PRINT_D("BT_OSI", fmt, ## args);}
#define OSI_TRACE_VERBOSE(fmt, args...)    {if (OSI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_VERBOSE && BT_LOG_LEVEL_CHECK(OSI,VERBOSE)) BT_PRINT_V("BT_OSI", fmt, ## args);}

/* define traces for BLUFI */
#define BLUFI_TRACE_ERROR(fmt, args...)      {if (BLUFI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_ERROR && BT_LOG_LEVEL_CHECK(BLUFI, ERROR)) BT_PRINT_E("BT_BLUFI", fmt, ## args);}
#define BLUFI_TRACE_WARNING(fmt, args...)    {if (BLUFI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_WARNING && BT_LOG_LEVEL_CHECK(BLUFI, WARNING)) BT_PRINT_W("BT_BLUFI", fmt, ## args);}
#define BLUFI_TRACE_API(fmt, args...)        {if (BLUFI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_API && BT_LOG_LEVEL_CHECK(BLUFI,API)) BT_PRINT_I("BT_BLUFI", fmt, ## args);}
#define BLUFI_TRACE_EVENT(fmt, args...)      {if (BLUFI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_EVENT && BT_LOG_LEVEL_CHECK(BLUFI,EVENT)) BT_PRINT_D("BT_BLUFI", fmt, ## args);}
#define BLUFI_TRACE_DEBUG(fmt, args...)      {if (BLUFI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_DEBUG && BT_LOG_LEVEL_CHECK(BLUFI,DEBUG)) BT_PRINT_D("BT_BLUFI", fmt, ## args);}
#define BLUFI_TRACE_VERBOSE(fmt, args...)    {if (BLUFI_INITIAL_TRACE_LEVEL >= BT_TRACE_LEVEL_VERBOSE && BT_LOG_LEVEL_CHECK(BLUFI,VERBOSE)) BT_PRINT_V("BT_BLUFI", fmt, ## args);}

#else
#define LOG_ERROR(fmt, args...)            
#define LOG_WARN(fmt, args...)             
#define LOG_INFO(fmt, args...)             
#define LOG_DEBUG(fmt, args...)            
#define LOG_VERBOSE(fmt, args...)          

/* Define tracing for the HCI unit
*/
#define HCI_TRACE_ERROR(fmt, args...)      
#define HCI_TRACE_WARNING(fmt, args...)    
#define HCI_TRACE_EVENT(fmt, args...)      
#define HCI_TRACE_DEBUG(fmt, args...)      

/* Define tracing for BTM
*/
#define BTM_TRACE_ERROR(fmt, args...)      
#define BTM_TRACE_WARNING(fmt, args...)    
#define BTM_TRACE_API(fmt, args...)        
#define BTM_TRACE_EVENT(fmt, args...)      
#define BTM_TRACE_DEBUG(fmt, args...)      

/* Define tracing for the L2CAP unit
*/
#define L2CAP_TRACE_ERROR(fmt, args...)    
#define L2CAP_TRACE_WARNING(fmt, args...)  
#define L2CAP_TRACE_API(fmt, args...)      
#define L2CAP_TRACE_EVENT(fmt, args...)    
#define L2CAP_TRACE_DEBUG(fmt, args...)    

/* Define tracing for the SDP unit
*/
#define SDP_TRACE_ERROR(fmt, args...)      
#define SDP_TRACE_WARNING(fmt, args...)    
#define SDP_TRACE_API(fmt, args...)        
#define SDP_TRACE_EVENT(fmt, args...)      
#define SDP_TRACE_DEBUG(fmt, args...)      

/* Define tracing for the RFCOMM unit
*/
#define RFCOMM_TRACE_ERROR(fmt, args...)   
#define RFCOMM_TRACE_WARNING(fmt, args...) 
#define RFCOMM_TRACE_API(fmt, args...)     
#define RFCOMM_TRACE_EVENT(fmt, args...)   
#define RFCOMM_TRACE_DEBUG(fmt, args...)   

/* Generic Access Profile traces */
#define GAP_TRACE_ERROR(fmt, args...)      
#define GAP_TRACE_EVENT(fmt, args...)      
#define GAP_TRACE_API(fmt, args...)        
#define GAP_TRACE_WARNING(fmt, args...)    

/* define traces for HID Host */
#define HIDH_TRACE_ERROR(fmt, args...)     
#define HIDH_TRACE_WARNING(fmt, args...)   
#define HIDH_TRACE_API(fmt, args...)       
#define HIDH_TRACE_EVENT(fmt, args...)     
#define HIDH_TRACE_DEBUG(fmt, args...)     

/* define traces for BNEP */

#define BNEP_TRACE_ERROR(fmt, args...)     
#define BNEP_TRACE_WARNING(fmt, args...)   
#define BNEP_TRACE_API(fmt, args...)       
#define BNEP_TRACE_EVENT(fmt, args...)     
#define BNEP_TRACE_DEBUG(fmt, args...)     

/* define traces for PAN */

#define PAN_TRACE_ERROR(fmt, args...)      
#define PAN_TRACE_WARNING(fmt, args...)    
#define PAN_TRACE_API(fmt, args...)        
#define PAN_TRACE_EVENT(fmt, args...)      
#define PAN_TRACE_DEBUG(fmt, args...)      

/* Define tracing for the A2DP profile
*/
#define A2D_TRACE_ERROR(fmt, args...)      
#define A2D_TRACE_WARNING(fmt, args...)    
#define A2D_TRACE_EVENT(fmt, args...)      
#define A2D_TRACE_DEBUG(fmt, args...)      
#define A2D_TRACE_API(fmt, args...)        

/* AVDTP
*/
#define AVDT_TRACE_ERROR(fmt, args...)     
#define AVDT_TRACE_WARNING(fmt, args...)   
#define AVDT_TRACE_EVENT(fmt, args...)     
#define AVDT_TRACE_DEBUG(fmt, args...)     
#define AVDT_TRACE_API(fmt, args...)       

/* Define tracing for the AVCTP protocol
*/
#define AVCT_TRACE_ERROR(fmt, args...)     
#define AVCT_TRACE_WARNING(fmt, args...)   
#define AVCT_TRACE_EVENT(fmt, args...)     
#define AVCT_TRACE_DEBUG(fmt, args...)     
#define AVCT_TRACE_API(fmt, args...)       

/* Define tracing for the AVRCP profile
*/
#define AVRC_TRACE_ERROR(fmt, args...)     
#define AVRC_TRACE_WARNING(fmt, args...)   
#define AVRC_TRACE_EVENT(fmt, args...)     
#define AVRC_TRACE_DEBUG(fmt, args...)     
#define AVRC_TRACE_API(fmt, args...)       

/* MCAP
*/
#define MCA_TRACE_ERROR(fmt, args...)      
#define MCA_TRACE_WARNING(fmt, args...)    
#define MCA_TRACE_EVENT(fmt, args...)      
#define MCA_TRACE_DEBUG(fmt, args...)      
#define MCA_TRACE_API(fmt, args...)        

/* Define tracing for the ATT/GATT unit
*/
#define GATT_TRACE_ERROR(fmt, args...)     
#define GATT_TRACE_WARNING(fmt, args...)   
#define GATT_TRACE_API(fmt, args...)       
#define GATT_TRACE_EVENT(fmt, args...)     
#define GATT_TRACE_DEBUG(fmt, args...)     

/* Define tracing for the SMP unit
*/
#define SMP_TRACE_ERROR(fmt, args...)      
#define SMP_TRACE_WARNING(fmt, args...)    
#define SMP_TRACE_API(fmt, args...)        
#define SMP_TRACE_EVENT(fmt, args...)      
#define SMP_TRACE_DEBUG(fmt, args...)      

extern UINT8 btif_trace_level;

// define traces for application
#define BTIF_TRACE_ERROR(fmt, args...)     
#define BTIF_TRACE_WARNING(fmt, args...)   
#define BTIF_TRACE_API(fmt, args...)       
#define BTIF_TRACE_EVENT(fmt, args...)     
#define BTIF_TRACE_DEBUG(fmt, args...)     
#define BTIF_TRACE_VERBOSE(fmt, args...)   

/* define traces for application */

#define APPL_TRACE_ERROR(fmt, args...)     
#define APPL_TRACE_WARNING(fmt, args...)   
#define APPL_TRACE_API(fmt, args...)       
#define APPL_TRACE_EVENT(fmt, args...)     
#define APPL_TRACE_DEBUG(fmt, args...)     
#define APPL_TRACE_VERBOSE(fmt, args...)   

/* define traces for BTC */
#define BTC_TRACE_ERROR(fmt, args...) 
#define BTC_TRACE_WARNING(fmt, args...)
#define BTC_TRACE_API(fmt, args...)
#define BTC_TRACE_EVENT(fmt, args...)
#define BTC_TRACE_DEBUG(fmt, args...)
#define BTC_TRACE_VERBOSE(fmt, args...)

/* define traces for OSI */
#define OSI_TRACE_ERROR(fmt, args...) 
#define OSI_TRACE_WARNING(fmt, args...)
#define OSI_TRACE_API(fmt, args...)
#define OSI_TRACE_EVENT(fmt, args...)
#define OSI_TRACE_DEBUG(fmt, args...)
#define OSI_TRACE_VERBOSE(fmt, args...)

/* define traces for BLUFI */
#define BLUFI_TRACE_ERROR(fmt, args...) 
#define BLUFI_TRACE_WARNING(fmt, args...)
#define BLUFI_TRACE_API(fmt, args...)
#define BLUFI_TRACE_EVENT(fmt, args...)
#define BLUFI_TRACE_DEBUG(fmt, args...)
#define BLUFI_TRACE_VERBOSE(fmt, args...)
#endif  ///CONFIG_BT_STACK_NO_LOG


/* Simplified Trace Helper Macro
*/
#define bdld(fmt, args...) \
    do{\
        if((MY_LOG_LEVEL) >= BT_TRACE_LEVEL_DEBUG) \
        BT_PRINT_D(fmt, ## args);        \
    }while(0)

#define bdlw(fmt, args...) \
    do{\
        if((MY_LOG_LEVEL) >= BT_TRACE_LEVEL_WARNING) \
        BT_PRINT_W(fmt, ## args);        \
    }while(0)

#define bdle(fmt, args...) \
    do{\
        if((MY_LOG_LEVEL) >= BT_TRACE_LEVEL_ERROR) \
        BT_PRINT_E(fmt, ## args);        \
    }while(0)

#define bdla(assert_if) \
    do{\
        if(((MY_LOG_LEVEL) >= BT_TRACE_LEVEL_ERROR) && !(assert_if)) \
        BT_PRINT_E("%s: assert failed\n", #assert_if);       \
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
