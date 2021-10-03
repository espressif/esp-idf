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
#ifndef HIDD_API_H
#define HIDD_API_H
#include "hiddefs.h"
#include "sdp_api.h"

#if (HID_DEV_INCLUDED == TRUE)
/*****************************************************************************
 *  Type Definitions
 ****************************************************************************/
enum {
    HID_CHANNEL_INTR,
    HID_CHANNEL_CTRL
};
/*
    HID_DHOST_EVT_OPEN  - connected to host device (CTRL and INTR), data = n/a
    HID_DHOST_EVT_CLOSE - disconnected from host device, data=reason
    HID_DHOST_EVT_GET_REPORT - got GET_REPORT from host
    HID_DHOST_EVT_SET_REPORT - got SET_REPORT from host
    HID_DHOST_EVT_SET_PROTOCOL - got SET_PROTOCOL from host
*/

enum {
    HID_DHOST_EVT_OPEN,
    HID_DHOST_EVT_CLOSE,
    HID_DHOST_EVT_GET_REPORT,
    HID_DHOST_EVT_SET_REPORT,
    HID_DHOST_EVT_SET_PROTOCOL,
    HID_DHOST_EVT_INTR_DATA,
    HID_DHOST_EVT_VC_UNPLUG,
    HID_DHOST_EVT_SUSPEND,
    HID_DHOST_EVT_EXIT_SUSPEND,
};

typedef void (tHID_DEV_HOST_CALLBACK)(BD_ADDR bd_addr, uint8_t event, uint32_t data, BT_HDR* p_buf);

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 *  External Function Declarations
 ****************************************************************************/

/*******************************************************************************
 *
 * Function         HID_DevInit
 *
 * Description      Initializes control block
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevInit(void);

/*******************************************************************************
 *
 * Function         HID_DevInit
 *
 * Description      Deinitializes control block
 *
 * Returns          void
 *
 ******************************************************************************/
extern void HID_DevDeinit(void);

/*******************************************************************************
 *
 * Function         HID_DevRegister
 *
 * Description      Registers HID device with lower layers
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevRegister(tHID_DEV_HOST_CALLBACK* host_cback);

/*******************************************************************************
 *
 * Function         HID_DevDeregister
 *
 * Description      Deregisters HID device with lower layers
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevDeregister(void);

/*******************************************************************************
 *
 * Function         HID_DevSetSecurityLevel
 *
 * Description      Sets security level for HID device connections
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevSetSecurityLevel(uint8_t sec_lvl);

/*******************************************************************************
 *
 * Function         HID_DevAddRecord
 *
 * Description      Creates SDP record for HID device
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevAddRecord(uint32_t handle, char* p_name,
                                    char* p_description, char* p_provider,
                                    uint16_t subclass, uint16_t desc_len,
                                    uint8_t* p_desc_data);

/*******************************************************************************
 *
 * Function         HID_DevSendReport
 *
 * Description      Sends report
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevSendReport(uint8_t channel, uint8_t type, uint8_t id,
                                     uint16_t len, uint8_t* p_data);

/*******************************************************************************
 *
 * Function         HID_DevVirtualCableUnplug
 *
 * Description      Sends Virtual Cable Unplug
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevVirtualCableUnplug(void);

/*******************************************************************************
 *
 * Function         HID_DevPlugDevice
 *
 * Description      Establishes virtual cable to given host
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevPlugDevice(BD_ADDR addr);

/*******************************************************************************
 *
 * Function         HID_DevUnplugDevice
 *
 * Description      Unplugs virtual cable from given host
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevUnplugDevice(BD_ADDR addr);

/*******************************************************************************
 *
 * Function         HID_DevConnect
 *
 * Description      Connects to device
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevConnect(void);

/*******************************************************************************
 *
 * Function         HID_DevDisconnect
 *
 * Description      Disconnects from device
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevDisconnect(void);

/*******************************************************************************
 *
 * Function         HID_DevSetIncomingPolicy
 *
 * Description      Sets policy for incoming connections (allowed/disallowed)
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevSetIncomingPolicy(bool allow);

/*******************************************************************************
 *
 * Function         HID_DevReportError
 *
 * Description      Reports error for Set Report via HANDSHAKE
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevReportError(uint8_t error);

/*******************************************************************************
 *
 * Function         HID_DevGetDevice
 *
 * Description      Returns the BD Address of virtually cabled device
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevGetDevice(BD_ADDR* addr);

/*******************************************************************************
 *
 * Function         HID_DevSetIncomingQos
 *
 * Description      Sets Incoming QoS values for Interrupt L2CAP Channel
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevSetIncomingQos(
    uint8_t service_type, uint32_t token_rate, uint32_t token_bucket_size,
    uint32_t peak_bandwidth, uint32_t latency, uint32_t delay_variation);

/*******************************************************************************
 *
 * Function         HID_DevSetOutgoingQos
 *
 * Description      Sets Outgoing QoS values for Interrupt L2CAP Channel
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
extern tHID_STATUS HID_DevSetOutgoingQos(
    uint8_t service_type, uint32_t token_rate, uint32_t token_bucket_size,
    uint32_t peak_bandwidth, uint32_t latency, uint32_t delay_variation);

/*******************************************************************************
 *
 * Function         HID_DevSetTraceLevel
 *
 * Description      This function sets the trace level for HID Dev. If called
 *                  with a value of 0xFF, it simply reads the current trace level.
 *
 * Returns          the new (current) trace level
 *
 ******************************************************************************/
extern uint8_t HID_DevSetTraceLevel(uint8_t new_level);

#ifdef __cplusplus
}
#endif

#endif
#endif /* HIDD_API_H */
