/******************************************************************************
 *
 *  Copyright (C) 2016 The Android Open Source Project
 *  Copyright (C) 2002-2012 Broadcom Corporation
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
#ifndef BTA_HD_API_H
#define BTA_HD_API_H

#include "bta_api.h"
#include "stack/hidd_api.h"

#if BTA_HD_INCLUDED == TRUE

/*****************************************************************************
 *  Constants and Type Definitions
 ****************************************************************************/

#ifndef BTA_HD_DEBUG
#define BTA_HD_DEBUG TRUE
#endif

/* BTA HID Device callback events */
#define BTA_HD_ENABLE_EVT 0         /* BT-HD enabled */
#define BTA_HD_DISABLE_EVT 1        /* BT-HD disabled */
#define BTA_HD_REGISTER_APP_EVT 2   /* application registered */
#define BTA_HD_UNREGISTER_APP_EVT 3 /* application unregistered */
#define BTA_HD_OPEN_EVT 4           /* connection to host opened */
#define BTA_HD_CLOSE_EVT 5          /* connection to host closed */
#define BTA_HD_GET_REPORT_EVT 6     /* GET_REPORT request from host */
#define BTA_HD_SET_REPORT_EVT 7     /* SET_REPORT request from host */
#define BTA_HD_SET_PROTOCOL_EVT 8   /* SET_PROTOCOL request from host */
#define BTA_HD_INTR_DATA_EVT 9      /* DATA received from host on intr */
#define BTA_HD_VC_UNPLUG_EVT 10     /* Virtual Cable Unplug */
// #define BTA_HD_CONN_STATE_EVT 11    /* Report connection state change */
#define BTA_HD_SEND_REPORT_EVT 12     /* Send report finish */
#define BTA_HD_REPORT_ERR_EVT 13     /* Report Handshake finish */
#define BTA_HD_API_ERR_EVT 99       /* BT-HD API error */

typedef uint16_t tBTA_HD_EVT;

enum { BTA_HD_OK, BTA_HD_ERROR };

typedef enum {
    BTA_HD_CONN_STATE_CONNECTED,
    BTA_HD_CONN_STATE_CONNECTING,
    BTA_HD_CONN_STATE_DISCONNECTED,
    BTA_HD_CONN_STATE_DISCONNECTING,
    BTA_HD_CONN_STATE_UNKNOWN
} tBTA_HD_CONN_STAT;

typedef uint8_t tBTA_HD_STATUS;
typedef tHID_DEV_DSCP_INFO tBTA_HD_DEV_DESCR;

typedef struct {
    char *p_name;
    char *p_description;
    char *p_provider;
    uint8_t subclass;
    tBTA_HD_DEV_DESCR descriptor;
} tBTA_HD_APP_INFO;

typedef struct {
    uint8_t service_type;
    uint32_t token_rate;
    uint32_t token_bucket_size;
    uint32_t peak_bandwidth;
    uint32_t access_latency;
    uint32_t delay_variation;
} tBTA_HD_QOS_INFO;

typedef struct {
    bool use_intr;
    uint8_t type;
    uint8_t id;
    uint16_t len;
    uint8_t *p_data;
} tBTA_HD_REPORT;

typedef struct {
    tBTA_HD_STATUS status;
    bool in_use;
    BD_ADDR bda;
} tBTA_HD_REG_STATUS;

typedef struct {
    BD_ADDR bda;
    tBTA_HD_STATUS status;
    tBTA_HD_CONN_STAT conn_status;
} tBTA_HD_CONN;

typedef struct {
    uint8_t report_type;
    uint8_t report_id;
    uint16_t buffer_size;
} tBTA_HD_GET_REPORT;

typedef struct {
    uint8_t report_type;
    uint8_t report_id;
    uint16_t len;
    uint8_t *p_data;
} tBTA_HD_SET_REPORT;

typedef uint8_t tBTA_HD_SET_PROTOCOL;

typedef struct {
    uint8_t report_id;
    uint16_t len;
    uint8_t *p_data;
} tBTA_HD_INTR_DATA;

typedef struct {
    tBTA_HD_STATUS status;
    uint8_t reason;
    uint8_t report_type;
    uint8_t report_id;
} tBTA_HD_API_SEND_REPORT;

typedef struct {
    tBTA_HD_STATUS status;
    uint8_t reason;
} tBTA_HD_API_REPORT_ERR;

/* union of data associated with HD callback */
typedef union {
    tBTA_HD_STATUS status;                  /* BTA_HD_ENABLE_EVT
                                               BTA_HD_DISABLE_EVT
                                               BTA_HD_UNREGISTER_APP_EVT */
    tBTA_HD_REG_STATUS reg_status;          /* BTA_HD_REGISTER_APP_EVT */
    tBTA_HD_CONN conn;                      /* BTA_HD_OPEN_EVT
                                               BTA_HD_CLOSE_EVT
                                               BTA_HD_VC_UNPLUG_EVT
                                               BTA_HD_OWN_VC_UNPLUG_EVT */
    tBTA_HD_GET_REPORT get_report;          /* BTA_HD_GET_REPORT */
    tBTA_HD_SET_REPORT set_report;          /* BTA_HD_SET_REPORT */
    tBTA_HD_SET_PROTOCOL set_protocol;      /* BTA_HD_SETPROTOCOL */
    tBTA_HD_INTR_DATA intr_data;            /* BTA_HD_INTR_DATA_EVT */
    tBTA_HD_API_SEND_REPORT send_report;    /* BTA_HD_API_SEND_REPORT_EVT */
    tBTA_HD_API_REPORT_ERR report_err;      /* BTA_HD_API_REPORT_ERR_EVT */
} tBTA_HD;

/* BTA HD callback function */
typedef void (tBTA_HD_CBACK)(tBTA_HD_EVT event, tBTA_HD *p_data);

/*****************************************************************************
 *  External Function Declarations
 ****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *
 * Function         BTA_HhRegister
 *
 * Description      This function enable HID host and registers HID-Host with
 *                  lower layers.
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdEnable(tBTA_HD_CBACK *p_cback);

/*******************************************************************************
 *
 * Function         BTA_HhDeregister
 *
 * Description      This function is called when the host is about power down.
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdDisable(void);

/*******************************************************************************
 *
 * Function         BTA_HdRegisterApp
 *
 * Description      This function is called when application should be
 *                  registered
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdRegisterApp(tBTA_HD_APP_INFO *p_app_info, tBTA_HD_QOS_INFO *p_in_qos, tBTA_HD_QOS_INFO *p_out_qos);

/*******************************************************************************
 *
 * Function         BTA_HdUnregisterApp
 *
 * Description      This function is called when application should be
 *                  unregistered
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdUnregisterApp(void);

/*******************************************************************************
 *
 * Function         BTA_HdSendReport
 *
 * Description      This function is called when report is to be sent
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdSendReport(tBTA_HD_REPORT *p_report);

/*******************************************************************************
 *
 * Function         BTA_HdVirtualCableUnplug
 *
 * Description      This function is called when VCU shall be sent
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdVirtualCableUnplug(void);

/*******************************************************************************
 *
 * Function         BTA_HdConnect
 *
 * Description      This function is called when connection to host shall be
 *                  made.
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdConnect(BD_ADDR addr);

/*******************************************************************************
 *
 * Function         BTA_HdDisconnect
 *
 * Description      This function is called when host shall be disconnected
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdDisconnect(void);

/*******************************************************************************
 *
 * Function         BTA_HdAddDevice
 *
 * Description      This function is called when a device is virtually cabled
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdAddDevice(BD_ADDR addr);

/*******************************************************************************
 *
 * Function         BTA_HdRemoveDevice
 *
 * Description      This function is called when a device is virtually uncabled
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdRemoveDevice(BD_ADDR addr);

/*******************************************************************************
 *
 * Function         BTA_HdReportError
 *
 * Description      This function is called when reporting error for set report
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdReportError(uint8_t error);

#ifdef __cplusplus
}
#endif

#endif /* BTA_HD_INCLUDED */
#endif /* BTA_HD_API_H */
