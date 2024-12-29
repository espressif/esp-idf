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

/* Message types */

#define PTP_MSGTYPE_MASK       0x0F
#define PTP_MSGTYPE_SYNC          0
#define PTP_MSGTYPE_DELAY_REQ     1
#define PTP_MSGTYPE_FOLLOW_UP     8
#define PTP_MSGTYPE_DELAY_RESP    9
#define PTP_MSGTYPE_ANNOUNCE     11

/* Message flags */

#define PTP_FLAGS0_TWOSTEP        (1 << 1)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Defined in IEEE 1588-2008 Precision Time Protocol
 * All multi-byte fields are big-endian.
 */

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
};

/* Sync: transmit timestamp from master clock */

struct ptp_sync_s
{
  struct ptp_header_s header;
  uint8_t origintimestamp[10];
};

/* FollowUp: actual timestamp of when sync message was sent */

struct ptp_follow_up_s
{
  struct ptp_header_s header;
  uint8_t origintimestamp[10];
};

/* DelayReq: request delay measurement */

struct ptp_delay_req_s
{
  struct ptp_header_s header;
  uint8_t origintimestamp[10];
};

/* DelayResp: response to DelayReq */

struct ptp_delay_resp_s
{
  struct ptp_header_s header;
  uint8_t receivetimestamp[10];
  uint8_t reqidentity[8];
  uint8_t reqportindex[2];
};

#endif /* __APPS_NETUTILS_PTPD_PTPV2_H */
