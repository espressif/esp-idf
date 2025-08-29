/*
 * SPDX-FileCopyrightText: 2020-2024 The Apache Software Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2024 Espressif Systems (Shanghai) CO LTD
 */

/****************************************************************************
 * apps/netutils/ptpd/ptpv2.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __APPS_NETUTILS_PTPD_PTPV2_H
#define __APPS_NETUTILS_PTPD_PTPV2_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Time-critical messages (id < 8) are sent to port 319,
 * other messages to port 320.
 */

#define PTP_UDP_PORT_EVENT 319
#define PTP_UDP_PORT_INFO  320

/* Multicast address to send to: 224.0.1.129 */

#define PTP_MULTICAST_ADDR ((in_addr_t)0xE0000181)

/* Multicast MAC addresses for PTP */

#define LLDP_MULTICAST_ADDR (uint8_t[6]){0x01, 0x80, 0xC2, 0x00, 0x00, 0x0e} // for all messages in case of gPTP
#define PTP4L_MULTICAST_ADDR (uint8_t[6]){0x01, 0x1B, 0x19, 0x00, 0x00, 0x00} // for sync, announce, follow_up (non-gPTP)

/* Message types */

#define PTP_MSGTYPE_MASK                   0x0F
#define PTP_MSGTYPE_SYNC                   0x00
#define PTP_MSGTYPE_FOLLOW_UP              0x08
#define PTP_MSGTYPE_ANNOUNCE               0x0b
#define PTP_MSGTYPE_DELAY_REQ              0x01
#define PTP_MSGTYPE_DELAY_RESP             0x09
#define PTP_MSGTYPE_PDELAY_REQ             0x02 // only used in gPTP
#define PTP_MSGTYPE_PDELAY_RESP            0x03 // only used in gPTP
#define PTP_MSGTYPE_PDELAY_RESP_FOLLOW_UP  0x1a // only used in gPTP

/* Message flags */

#define PTP_FLAGS0_TWOSTEP        (1 << 1) // flag indicating there will be a follow-up message
#define PTP_FLAGS1_PTP_TIMESCALE  (1 << 3) // flag indicating use of PTP timescale (gPTP required)
#define PTP_MSGTYPE_SDOID_GPTP    (1 << 4) // flag indicating a gPTP message

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Defined in IEEE 1588-2008 Precision Time Protocol and IEEE 802.1AS-2011
 * All multi-byte fields are big-endian.
 */

/* Path trace TLV for gPTP follow up messages */

struct ptp_pathtrace_tlv_s
{
  uint8_t type[2];
  uint8_t length[2];
  uint8_t pathsequence[8]; // this can have more but gPTP endpoints will ignore
};

/* Information TLV for gPTP announce messages */

struct ptp_info_tlv_s
{
  uint8_t type[2];
  uint8_t length[2];
  uint8_t orgidentity[3];
  uint8_t orgsubtype[3];
  uint8_t cumulativescaledrateoffset[4];
  uint8_t gmtimebaseindicator[2];
  uint8_t lastgmphasechange[12];
  uint8_t scaledlastgmfreqchange[4];
};

/* Common header for all message types */

struct ptp_header_s
{
  uint8_t messagetype;
  uint8_t version;
  uint8_t messagelength[2];
  uint8_t domain;
  uint8_t reserved1;
  uint8_t flags[2];
  uint8_t correction[8];
  uint8_t reserved2[4];
  uint8_t sourceidentity[8];
  uint8_t sourceportindex[2];
  uint8_t sequenceid[2];
  uint8_t controlfield;
  uint8_t logmessageinterval;
};

/* Announce a master clock */

struct ptp_announce_s
{
  struct ptp_header_s header;
  uint8_t origintimestamp[10];
  uint8_t utcoffset[2];
  uint8_t reserved;
  uint8_t gm_priority1;
  uint8_t gm_quality[4];
  uint8_t gm_priority2;
  uint8_t gm_identity[8];
  uint8_t stepsremoved[2];
  uint8_t timesource;
  struct ptp_pathtrace_tlv_s pathtracetlv; // gPTP required
};

/* Sync: transmit timestamp from master clock */

struct ptp_sync_s
{
  struct ptp_header_s header;
  uint8_t origintimestamp[10]; // in gPTP profile, this will be ignored
};

/* FollowUp: actual timestamp of when sync message was sent */

struct ptp_follow_up_s
{
  struct ptp_header_s header;
  uint8_t origintimestamp[10];
  uint8_t informationtlv[32]; // gPTP required
};

/* DelayReq: request delay measurement (path delay or peer delay) */

struct ptp_delay_req_s
{
  struct ptp_header_s header;
  uint8_t origintimestamp[10]; // in gPTP profile, this will be ignored
};

/* DelayResp: response to DelayReq (path delay or peer delay)*/

struct ptp_delay_resp_s
{
  struct ptp_header_s header;
  uint8_t receivetimestamp[10];
  uint8_t reqidentity[8];
  uint8_t reqportindex[2];
};

/* DelayResp: follow up to DelayResp (gPTP only)*/

struct ptp_delay_resp_follow_up_s
{
  struct ptp_header_s header;
  uint8_t origintimestamp[10];
  uint8_t reqidentity[8];
  uint8_t reqportindex[2];
};

#endif /* __APPS_NETUTILS_PTPD_PTPV2_H */
