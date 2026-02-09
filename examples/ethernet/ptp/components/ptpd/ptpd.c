/*
 * SPDX-FileCopyrightText: 2020-2024 The Apache Software Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2024-2025 Espressif Systems (Shanghai) CO LTD
 */

/****************************************************************************
 * apps/netutils/ptpd/ptpd.c
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

#define ESP_PTP 1

/****************************************************************************
 * Included Files
 ****************************************************************************/
#ifndef ESP_PTP
#include <nuttx/config.h>
#endif

#include <stdbool.h>
#include <stdint.h>

#include <sys/socket.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sched.h>
#include <assert.h>
#include <errno.h>
#ifndef ESP_PTP
#include <debug.h>
#endif
#include <unistd.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef ESP_PTP
#include <netutils/ipmsfilter.h>
#endif

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#ifndef ESP_PTP
#include <nuttx/net/netconfig.h>
#include <netutils/ptpd.h>
#endif

#include "ptpv2.h"

#ifdef ESP_PTP
#include "ptpd.h"
#include "esp_eth_driver.h"
#include "esp_vfs_l2tap.h"
#include "semaphore.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lwip/prot/ethernet.h" // Ethernet headers

#include "esp_eth_time.h"

#define ETH_TYPE_PTP 0x88F7

#define SET_MAC_ADDR(addr, a, b, c, d, e, f) do { \
    addr[0] = a; addr[1] = b; addr[2] = c; \
    addr[3] = d; addr[4] = e; addr[5] = f; \
} while(0)

#define ERROR ESP_FAIL
#define OK ESP_OK

#define UNUSED (void)

#define MSEC_PER_SEC 1000
#define NSEC_PER_USEC 1000
#define NSEC_PER_MSEC 1000000ll
#define NSEC_PER_SEC 1000000000ll

// To able to set either only server or only client
#ifndef CONFIG_NETUTILS_PTPD_TIMEOUT_MS
#define CONFIG_NETUTILS_PTPD_TIMEOUT_MS 0
#endif
#ifndef CONFIG_NETUTILS_PTPD_SETTIME_THRESHOLD_MS
#define CONFIG_NETUTILS_PTPD_SETTIME_THRESHOLD_MS 0
#endif
#ifndef CONFIG_NETUTILS_PTPD_MAX_PATH_DELAY_NS
#define CONFIG_NETUTILS_PTPD_MAX_PATH_DELAY_NS 0
#endif
#ifndef CONFIG_NETUTILS_PTPD_DELAYREQ_AVGCOUNT
#define CONFIG_NETUTILS_PTPD_DELAYREQ_AVGCOUNT 0
#endif
#ifndef CONFIG_NETUTILS_PTPD_DELAYRESP_INTERVAL
#define CONFIG_NETUTILS_PTPD_DELAYRESP_INTERVAL 0
#endif
#ifndef CONFIG_NETUTILS_PTPD_PATH_DELAY_STABILITY_NS
#define CONFIG_NETUTILS_PTPD_PATH_DELAY_STABILITY_NS 0
#endif

#define clock_timespec_subtract(ts1, ts2, ts3) timespecsub(ts1, ts2, ts3)
#define clock_timespec_add(ts1, ts2, ts3) timespecadd(ts1, ts2, ts3)

#endif // ESP_PTP

/****************************************************************************
 * Private Data
 ****************************************************************************/

#ifdef ESP_PTP
#define ADJ_FREQ_MAX 512000 // TODO tuneup
typedef struct
{
  int32_t kp;
  int32_t ki;
  int32_t drift_acc;
} pi_cntrl_t;
#endif // ESP_PTP

/* Carrier structure for querying PTPD status */

struct ptpd_statusreq_s
{
  FAR sem_t *done;
  FAR struct ptpd_status_s *dest;
};

/* Main PTPD state storage */

struct ptp_state_s
{
  /* Request for PTPD task to stop or report status */

  bool stop;
  struct ptpd_statusreq_s status_req;

#ifdef ESP_PTP
  uint8_t intf_hw_addr[ETH_ADDR_LEN];
  int ptp_socket;

  int64_t remote_time_ns_prev;
  int64_t local_time_ns_prev;

  int64_t last_offset_ns;

  pi_cntrl_t offset_pi;
#else
  /* Address of network interface we are operating on */

  struct sockaddr_in interface_addr;

  /* Socket bound to interface for transmission */

  int tx_socket;

  /* Sockets for PTP event and information ports */

  int event_socket;

  int info_socket;
#endif // ESP_PTP

  /* Our own identity as a clock source */

  struct ptp_announce_s own_identity;

  /* Sequence number counters per message type */

  uint16_t announce_seq;
  uint16_t sync_seq;
  uint16_t delay_req_seq;

  /* Previous measurement and estimated clock drift rate */

  struct timespec last_delta_timestamp;
  int64_t last_delta_ns;
  int64_t last_adjtime_ns;
  long drift_avg_total_ms;
  long drift_ppb;

  /* Identity of currently selected clock source,
   * from the latest announcement message.
   *
   * The timestamps are used for timeout when a source disappears.
   * They are from the local CLOCK_MONOTONIC.
   */

  bool selected_source_valid;              /* True if operating as client */
  struct ptp_announce_s selected_source;   /* Currently selected server */
  struct timespec last_received_multicast; /* Any multicast packet */
  struct timespec last_received_announce;  /* Announce from any server */
  struct timespec last_received_sync;      /* Sync from selected source */

  /* Last transmitted packet timestamps (CLOCK_MONOTONIC)
   * Used to set transmission interval.
   */

  struct timespec last_transmitted_sync;
  struct timespec last_transmitted_announce;
  struct timespec last_transmitted_delayresp;
  struct timespec last_transmitted_delayreq;

  /* Timestamps related to path delay calculation (CLOCK_REALTIME) */

  bool can_send_delayreq;
  struct timespec delayreq_time;
  int path_delay_avgcount;
  long path_delay_ns;
  long delayreq_interval;

  /* Latest received packet and its timestamp (CLOCK_REALTIME) */

  struct timespec rxtime;
  union
  {
    struct ptp_header_s     header;
    struct ptp_announce_s   announce;
    struct ptp_sync_s       sync;
    struct ptp_follow_up_s  follow_up;
    struct ptp_delay_req_s  delay_req;
    struct ptp_delay_resp_s delay_resp;
    uint8_t                 raw[128];
  } rxbuf;

#ifndef ESP_PTP
  uint8_t rxcmsg[CMSG_LEN(sizeof(struct timeval))];
#endif // ESP_PTP

  /* Buffered sync packet for two-step clock setting where server sends
   * the accurate timestamp in a separate follow-up message.
   */

  struct ptp_sync_s twostep_packet;
  struct timespec twostep_rxtime;
};

#ifdef CONFIG_NETUTILS_PTPD_SERVER
#  define PTPD_POLL_INTERVAL CONFIG_NETUTILS_PTPD_SYNC_INTERVAL_MSEC
#else
#  define PTPD_POLL_INTERVAL CONFIG_NETUTILS_PTPD_TIMEOUT_MS
#endif

/* PTP debug messages are enabled by either CONFIG_DEBUG_NET_INFO
 * or separately by CONFIG_NETUTILS_PTPD_DEBUG. This simplifies
 * debugging without having excessive amount of logging from net.
 */

#ifdef ESP_PTP
static const char *TAG = "ptpd";
#define ptpinfo(format, ...) ESP_LOGI(TAG, format, ##__VA_ARGS__)
#define ptpwarn(format, ...) ESP_LOGW(TAG, format, ##__VA_ARGS__)
#define ptperr(format, ...) ESP_LOGE(TAG, format, ##__VA_ARGS__)
#else
#ifdef CONFIG_NETUTILS_PTPD_DEBUG
#  define ptpinfo _info
#  define ptpwarn _warn
#  define ptperr  _err
#else
#  define ptpinfo ninfo
#  define ptpwarn nwarn
#  define ptperr  nerr
#endif
#endif // ESP_PTP

#ifdef ESP_PTP
static struct ptp_state_s *s_state;
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/
#ifdef ESP_PTP
static int ptp_get_esp_eth_handle(struct ptp_state_s *state, esp_eth_handle_t *eth_handle)
{
  return ioctl(state->ptp_socket, L2TAP_G_DEVICE_DRV_HNDL, eth_handle);
}

static void ptp_create_eth_frame(struct ptp_state_s *state, uint8_t *eth_frame, void *ptp_msg, uint16_t ptp_msg_len)
{
  struct eth_hdr eth_hdr = {
    //.dest.addr = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E}, // TODO only for Pdelay_Req, Pdelay_Resp and Pdelay_Resp_Follow_Up
    .dest.addr = {0x01, 0x1B, 0x19, 0x00, 0x00, 0x00}, // All except peer delay messages, ptp4l sends everything at this addr
    .type = htons(ETH_TYPE_PTP)
  };
  memcpy(&eth_hdr.src.addr, state->intf_hw_addr, ETH_ADDR_LEN);

  memcpy(eth_frame, &eth_hdr, sizeof(eth_hdr));
  memcpy(eth_frame + sizeof(eth_hdr), ptp_msg, ptp_msg_len);
}

static int ptp_net_send(FAR struct ptp_state_s *state, void *ptp_msg, uint16_t ptp_msg_len, struct timespec *ts)
{
  uint8_t eth_frame[ptp_msg_len + ETH_HEADER_LEN];
  ptp_create_eth_frame(state, eth_frame, ptp_msg, ptp_msg_len);

  // wrap "Info Records Buffer" into union to ensure proper alignment of data (this is typically needed when
  // accessing double word variables or structs containing double word variables)
  union {
      uint8_t info_recs_buff[L2TAP_IREC_SPACE(sizeof(struct timespec))];
      l2tap_irec_hdr_t align;
  } u;

  l2tap_extended_buff_t ptp_msg_ext_buff;

  ptp_msg_ext_buff.info_recs_len = sizeof(u.info_recs_buff);
  ptp_msg_ext_buff.info_recs_buff = u.info_recs_buff;
  ptp_msg_ext_buff.buff = eth_frame;
  ptp_msg_ext_buff.buff_len = sizeof(eth_frame);

  l2tap_irec_hdr_t *ts_info = L2TAP_IREC_FIRST(&ptp_msg_ext_buff);
  ts_info->len = L2TAP_IREC_LEN(sizeof(struct timespec));
  ts_info->type = L2TAP_IREC_TIME_STAMP;

  int ret = write(state->ptp_socket, &ptp_msg_ext_buff, 0);

  // check if write was successful, ts exists and ts_info is valid
  if (ret > 0 && ts && ts_info->type == L2TAP_IREC_TIME_STAMP)
    {
      *ts = *(struct timespec *)ts_info->data;
    }

  return ret;
}

static int ptp_net_recv(FAR struct ptp_state_s *state, void *ptp_msg, uint16_t ptp_msg_len, struct timespec *ts)
{
  uint8_t eth_frame[ptp_msg_len + ETH_HEADER_LEN];

  // wrap "Info Records Buffer" into union to ensure proper alignment of data (this is typically needed when
  // accessing double word variables or structs containing double word variables)
  union {
      uint8_t info_recs_buff[L2TAP_IREC_SPACE(sizeof(struct timespec))];
      l2tap_irec_hdr_t align;
  } u;
  l2tap_extended_buff_t ptp_msg_ext_buff;

  ptp_msg_ext_buff.info_recs_len = sizeof(u.info_recs_buff);
  ptp_msg_ext_buff.info_recs_buff = u.info_recs_buff;
  ptp_msg_ext_buff.buff = eth_frame;
  ptp_msg_ext_buff.buff_len = sizeof(eth_frame);

  l2tap_irec_hdr_t *ts_info = L2TAP_IREC_FIRST(&ptp_msg_ext_buff);
  ts_info->len = L2TAP_IREC_LEN(sizeof(struct timespec));
  ts_info->type = L2TAP_IREC_TIME_STAMP;

  int ret = read(state->ptp_socket, &ptp_msg_ext_buff, 0);

  // check if read was successful, ts exists and ts_info is valid
  if (ret > 0 && ts && ts_info->type == L2TAP_IREC_TIME_STAMP)
    {
      *ts = *(struct timespec *)ts_info->data;
    }

  memcpy(ptp_msg, &eth_frame[ETH_HEADER_LEN], ret);

  return ret;
}

static int64_t timespec_to_ns(FAR const struct timespec *ts)
{
  return ts->tv_sec * NSEC_PER_SEC + (ts->tv_nsec);
}
#endif // ESP_PTP

/* Convert from timespec to PTP format */

static void timespec_to_ptp_format(FAR struct timespec *ts,
                                   FAR uint8_t *timestamp)
{
  /* IEEE 1588 uses 48 bits for seconds and 32 bits for nanoseconds,
   * both fields big-endian.
   */

#ifdef CONFIG_SYSTEM_TIME64
  timestamp[0] = (uint8_t)(ts->tv_sec >> 40);
  timestamp[1] = (uint8_t)(ts->tv_sec >> 32);
#else
  timestamp[0] = 0;
  timestamp[1] = 0;
#endif
  timestamp[2] = (uint8_t)(ts->tv_sec >> 24);
  timestamp[3] = (uint8_t)(ts->tv_sec >> 16);
  timestamp[4] = (uint8_t)(ts->tv_sec >>  8);
  timestamp[5] = (uint8_t)(ts->tv_sec >>  0);

  timestamp[6] = (uint8_t)(ts->tv_nsec >>  24);
  timestamp[7] = (uint8_t)(ts->tv_nsec >>  16);
  timestamp[8] = (uint8_t)(ts->tv_nsec >>   8);
  timestamp[9] = (uint8_t)(ts->tv_nsec >>   0);
}

/* Convert from PTP format to timespec */

static void ptp_format_to_timespec(FAR const uint8_t *timestamp,
                                   FAR struct timespec *ts)
{
  ts->tv_sec =
      (((int64_t)timestamp[0]) << 40)
    | (((int64_t)timestamp[1]) << 32)
    | (((int64_t)timestamp[2]) << 24)
    | (((int64_t)timestamp[3]) << 16)
    | (((int64_t)timestamp[4]) <<  8)
    | (((int64_t)timestamp[5]) <<  0);

  ts->tv_nsec =
      (((long)timestamp[6]) << 24)
    | (((long)timestamp[7]) << 16)
    | (((long)timestamp[8]) <<  8)
    | (((long)timestamp[9]) <<  0);
}

/* Returns true if A is a better clock source than B.
 * Implements Best Master Clock algorithm from IEEE-1588.
 */

static bool is_better_clock(FAR const struct ptp_announce_s *a,
                            FAR const struct ptp_announce_s *b)
{
  if  (a->gm_priority1 < b->gm_priority1     /* Main priority field */
    || a->gm_quality[0] < b->gm_quality[0]   /* Clock class */
    || a->gm_quality[1] < b->gm_quality[1]   /* Clock accuracy */
    || a->gm_quality[2] < b->gm_quality[2]   /* Clock variance high byte */
    || a->gm_quality[3] < b->gm_quality[3]   /* Clock variance low byte */
    || a->gm_priority2 < b->gm_priority2     /* Sub priority field */
    || memcmp(a->gm_identity, b->gm_identity, sizeof(a->gm_identity)) < 0)
    {
      return true;
    }
    else
    {
      return false;
    }
}

static int64_t timespec_to_ms(FAR const struct timespec *ts)
{
  return ts->tv_sec * MSEC_PER_SEC + (ts->tv_nsec / NSEC_PER_MSEC);
}

/* Get positive or negative delta between two timespec values.
 * If value would exceed int64 limit (292 years), return INT64_MAX/MIN.
 */

static int64_t timespec_delta_ns(FAR const struct timespec *ts1,
                                 FAR const struct timespec *ts2)
{
  int64_t delta_s;

  delta_s = ts1->tv_sec - ts2->tv_sec;

#ifdef CONFIG_SYSTEM_TIME64
  /* Conversion to nanoseconds could overflow if the system time is 64-bit */

  if (delta_s >= INT64_MAX / NSEC_PER_SEC)
    {
      return INT64_MAX;
    }
  else if (delta_s <= INT64_MIN / NSEC_PER_SEC)
    {
      return INT64_MIN;
    }
#endif

  return delta_s * NSEC_PER_SEC + (ts1->tv_nsec - ts2->tv_nsec);
}

/* Check if the currently selected source is still valid */

static bool is_selected_source_valid(FAR struct ptp_state_s *state)
{
  struct timespec time_now;
  struct timespec delta;

  if ((state->selected_source.header.messagetype & PTP_MSGTYPE_MASK)
      != PTP_MSGTYPE_ANNOUNCE)
    {
      return false; /* Uninitialized value */
    }

  /* Note: this uses monotonic clock to track the timeout even when
   *       system clock is adjusted.
   */

  clock_gettime(CLOCK_MONOTONIC, &time_now);
  clock_timespec_subtract(&time_now, &state->last_received_sync, &delta);

  if (timespec_to_ms(&delta) > CONFIG_NETUTILS_PTPD_TIMEOUT_MS)
    {
#ifdef ESP_PTP
      ESP_LOGD(TAG, "Too long time since received packet\n");
#endif // ESP_PTP
      return false; /* Too long time since received packet */
    }

  return true;
}

/* Increment sequence number for packet type, and copy to header */

static void ptp_increment_sequence(FAR uint16_t *sequence_num,
                                   FAR struct ptp_header_s *hdr)
{
  *sequence_num += 1;
  hdr->sequenceid[0] = (uint8_t)(*sequence_num >> 8);
  hdr->sequenceid[1] = (uint8_t)(*sequence_num);
}

/* Get sequence number from received packet */

static uint16_t ptp_get_sequence(FAR const struct ptp_header_s *hdr)
{
  return ((uint16_t)hdr->sequenceid[0] << 8) | hdr->sequenceid[1];
}

/* Get current system timestamp as a timespec
 * TODO: Possibly add support for selecting different clock or using
 *       architecture-specific interface for clock access.
 */

static int ptp_gettime(FAR struct ptp_state_s *state,
                       FAR struct timespec *ts)
{
  UNUSED(state);
#ifdef ESP_PTP
  return esp_eth_clock_gettime(CLOCK_PTP_SYSTEM, ts);
#else
  return clock_gettime(CLOCK_REALTIME, ts);
#endif // ESP_PTP
}

/* Change current system timestamp by jumping */

static int ptp_settime(FAR struct ptp_state_s *state,
                       FAR struct timespec *ts)
{
  UNUSED(state);
#ifdef ESP_PTP
  return esp_eth_clock_settime(CLOCK_PTP_SYSTEM, ts);
#else
  return clock_settime(CLOCK_REALTIME, ts);
#endif // ESP_PTP
}

#ifndef ESP_PTP
/* Smoothly adjust timestamp. */

static int ptp_adjtime(FAR struct ptp_state_s *state, int64_t delta_ns)
{
  struct timeval delta;

  delta.tv_sec = delta_ns / NSEC_PER_SEC;
  delta_ns -= (int64_t)delta.tv_sec * NSEC_PER_SEC;
  delta.tv_usec = delta_ns / NSEC_PER_USEC;
  return adjtime(&delta, NULL);
}
#endif // !ESP_PTP

#ifndef ESP_PTP
/* Get timestamp of latest received packet */

static int ptp_getrxtime(FAR struct ptp_state_s *state,
                         FAR struct msghdr *rxhdr,
                         FAR struct timespec *ts)
{
  /* Get hardware or kernel timestamp if available */

#ifdef CONFIG_NET_TIMESTAMP
  struct cmsghdr *cmsg;

  for_each_cmsghdr(cmsg, rxhdr)
    {
      if (cmsg->cmsg_level == SOL_SOCKET &&
          cmsg->cmsg_type == SO_TIMESTAMP &&
          cmsg->cmsg_len == CMSG_LEN(sizeof(struct timeval)))
        {
          TIMEVAL_TO_TIMESPEC((FAR struct timeval *)CMSG_DATA(cmsg), ts);

          /* Sanity-check the value */

          if (ts->tv_sec > 0 || ts->tv_nsec > 0)
            {
              return OK;
            }
        }
    }

  ptpwarn("CONFIG_NET_TIMESTAMP enabled but did not get packet timestamp\n");
#endif

  /* Fall back to current timestamp */

  return ptp_gettime(state, ts);
}
#endif // !ESP_PTP

/* Initialize PTP client/server state and create sockets */
#ifdef ESP_PTP
static int ptp_initialize_state(FAR struct ptp_state_s *state,
                                FAR const char *interface)
{
  state->ptp_socket = open("/dev/net/tap", 0);
  if (state->ptp_socket < 0)
  {
      ptperr("Failed to create tx socket: %d\n", errno);
      return ERROR;
  }

  // Set Ethernet interface on which to get raw frames
  if (ioctl(state->ptp_socket, L2TAP_S_INTF_DEVICE, interface) < 0)
  {
    ptperr("failed to set network interface at socket: %d\n", errno);
    return ERROR;
  }

  // Set the Ethertype filter (frames with this type will be available through the state->tx_socket)
  uint16_t eth_type_filter = ETH_TYPE_PTP;
  if (ioctl(state->ptp_socket, L2TAP_S_RCV_FILTER, &eth_type_filter) < 0)
  {
    ptperr("failed to set Ethertype filter: %d\n", errno);
    return ERROR;
  }
  // Enable time stamping in driver
  esp_eth_handle_t eth_handle;
  if (ptp_get_esp_eth_handle(state, &eth_handle) < 0)
  {
    ptperr("failed to get socket eth_handle %d\n", errno);
    return ERROR;
  }
  esp_eth_clock_cfg_t clk_cfg = {
    .eth_hndl = eth_handle,
  };
  esp_eth_clock_init(CLOCK_PTP_SYSTEM, &clk_cfg);

  // Enable time stamping in L2TAP
  if(ioctl(state->ptp_socket, L2TAP_S_TIMESTAMP_EN) < 0)
  {
    ptperr("failed to enable time stamping in l2 socket: %d\n", errno);
    return ERROR;
  }

  // get HW address
  esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, &state->intf_hw_addr);

  // Add well-known PTP multicast destination MAC addresses to the filter
  uint8_t dest_addr[ETH_ADDR_LEN];
  SET_MAC_ADDR(dest_addr, 0x01, 0x1B, 0x19, 0x00, 0x00, 0x00);
  esp_eth_ioctl(eth_handle, ETH_CMD_ADD_MAC_FILTER, dest_addr);
  SET_MAC_ADDR(dest_addr, 0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E);
  esp_eth_ioctl(eth_handle, ETH_CMD_ADD_MAC_FILTER, dest_addr);

  state->remote_time_ns_prev = 0;
  state->local_time_ns_prev = 0;

  state->offset_pi.kp = 1;
  state->offset_pi.ki = 10;
  state->offset_pi.drift_acc = 0;

  state->own_identity.header.version = 2;
  state->own_identity.header.domain = CONFIG_NETUTILS_PTPD_DOMAIN;
  state->own_identity.header.sourceidentity[0] = state->intf_hw_addr[0];
  state->own_identity.header.sourceidentity[1] = state->intf_hw_addr[1];
  state->own_identity.header.sourceidentity[2] = state->intf_hw_addr[2];
  state->own_identity.header.sourceidentity[3] = 0xff;
  state->own_identity.header.sourceidentity[4] = 0xfe;
  state->own_identity.header.sourceidentity[5] = state->intf_hw_addr[3];
  state->own_identity.header.sourceidentity[6] = state->intf_hw_addr[4];
  state->own_identity.header.sourceidentity[7] = state->intf_hw_addr[5];
  state->own_identity.header.sourceportindex[0] = 0;
  state->own_identity.header.sourceportindex[1] = 1;
#ifdef CONFIG_NETUTILS_PTPD_SERVER
  state->own_identity.gm_priority1 = CONFIG_NETUTILS_PTPD_PRIORITY1;
  state->own_identity.gm_quality[0] = CONFIG_NETUTILS_PTPD_CLASS;
  state->own_identity.gm_quality[1] = CONFIG_NETUTILS_PTPD_ACCURACY;
  state->own_identity.gm_quality[2] = 0xff; /* No variance estimate */
  state->own_identity.gm_quality[3] = 0xff;
  state->own_identity.gm_priority2 = CONFIG_NETUTILS_PTPD_PRIORITY2;
  memcpy(state->own_identity.gm_identity,
         state->own_identity.header.sourceidentity,
         sizeof(state->own_identity.gm_identity));
  state->own_identity.timesource = CONFIG_NETUTILS_PTPD_CLOCKSOURCE;
#else
  state->own_identity.gm_priority1 = 255; // When daemon is statically configured as slave, set the worst
#endif

  s_state = state;
  return OK;
}
#else
static int ptp_initialize_state(FAR struct ptp_state_s *state,
                                FAR const char *interface)
{
  int ret;
  struct ifreq req;
  struct sockaddr_in bind_addr;

#ifdef CONFIG_NET_TIMESTAMP
  int arg;
#endif

  /* Create sockets */

  state->tx_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (state->tx_socket < 0)
    {
      ptperr("Failed to create tx socket: %d\n", errno);
      return ERROR;
    }

  state->event_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (state->event_socket < 0)
    {
      ptperr("Failed to create event socket: %d\n", errno);
      return ERROR;
    }


  state->info_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (state->info_socket < 0)
    {
      ptperr("Failed to create info socket: %d\n", errno);
      return ERROR;
    }

  /* Get address information of the specified interface for binding socket
   * Only supports IPv4 currently.
   */

  memset(&req, 0, sizeof(req));
  strncpy(req.ifr_name, interface, sizeof(req.ifr_name));

  if (ioctl(state->event_socket, SIOCGIFADDR, (unsigned long)&req) < 0)
    {
      ptperr("Failed to get IP address information for interface %s\n",
             interface);
      return ERROR;
    }

  state->interface_addr = *(struct sockaddr_in *)&req.ifr_ifru.ifru_addr;

  /* Get hardware address to initialize the identity field in header.
   * Clock identity is EUI-64, which we make from EUI-48.
   */

  if (ioctl(state->event_socket, SIOCGIFHWADDR, (unsigned long)&req) < 0)
    {
      ptperr("Failed to get HW address information for interface %s\n",
             interface);
      return ERROR;
    }

  state->own_identity.header.version = 2;
  state->own_identity.header.domain = CONFIG_NETUTILS_PTPD_DOMAIN;
  state->own_identity.header.sourceidentity[0] = req.ifr_hwaddr.sa_data[0];
  state->own_identity.header.sourceidentity[1] = req.ifr_hwaddr.sa_data[1];
  state->own_identity.header.sourceidentity[2] = req.ifr_hwaddr.sa_data[2];
  state->own_identity.header.sourceidentity[3] = 0xff;
  state->own_identity.header.sourceidentity[4] = 0xfe;
  state->own_identity.header.sourceidentity[5] = req.ifr_hwaddr.sa_data[3];
  state->own_identity.header.sourceidentity[6] = req.ifr_hwaddr.sa_data[4];
  state->own_identity.header.sourceidentity[7] = req.ifr_hwaddr.sa_data[5];
  state->own_identity.header.sourceportindex[0] = 0;
  state->own_identity.header.sourceportindex[1] = 1;
  state->own_identity.gm_priority1 = CONFIG_NETUTILS_PTPD_PRIORITY1;
  state->own_identity.gm_quality[0] = CONFIG_NETUTILS_PTPD_CLASS;
  state->own_identity.gm_quality[1] = CONFIG_NETUTILS_PTPD_ACCURACY;
  state->own_identity.gm_quality[2] = 0xff; /* No variance estimate */
  state->own_identity.gm_quality[3] = 0xff;
  state->own_identity.gm_priority2 = CONFIG_NETUTILS_PTPD_PRIORITY2;
  memcpy(state->own_identity.gm_identity,
         state->own_identity.header.sourceidentity,
         sizeof(state->own_identity.gm_identity));
  state->own_identity.timesource = CONFIG_NETUTILS_PTPD_CLOCKSOURCE;

  /* Subscribe to PTP multicast address */

  bind_addr.sin_family = AF_INET;
  bind_addr.sin_addr.s_addr = HTONL(PTP_MULTICAST_ADDR);

  clock_gettime(CLOCK_MONOTONIC, &state->last_received_multicast);

  ret = ipmsfilter(&state->interface_addr.sin_addr,
                   &bind_addr.sin_addr,
                   MCAST_INCLUDE);
  if (ret < 0)
    {
      ptperr("Failed to bind multicast address: %d\n", errno);
      return ERROR;
    }

  /* Bind socket for events */

  bind_addr.sin_port = HTONS(PTP_UDP_PORT_EVENT);
  ret = bind(state->event_socket, (struct sockaddr *)&bind_addr,
             sizeof(bind_addr));
  if (ret < 0)
    {
      ptperr("Failed to bind to udp port %d\n", bind_addr.sin_port);
      return ERROR;
    }

#ifdef CONFIG_NET_TIMESTAMP
  arg = 1;
  ret = setsockopt(state->event_socket, SOL_SOCKET, SO_TIMESTAMP,
                   &arg, sizeof(arg));

  if (ret < 0)
    {
      ptperr("Failed to enable SO_TIMESTAMP: %s\n", strerror(errno));

      /* PTPD can operate without, but with worse accuracy */
    }
#endif

  /* Bind socket for announcements */

  bind_addr.sin_port = HTONS(PTP_UDP_PORT_INFO);
  ret = bind(state->info_socket, (struct sockaddr *)&bind_addr,
             sizeof(bind_addr));
  if (ret < 0)
    {
      ptperr("Failed to bind to udp port %d\n", bind_addr.sin_port);
      return ERROR;
    }

  /* Bind TX socket to interface address (local addr cannot be multicast) */

  bind_addr.sin_addr = state->interface_addr.sin_addr;
  ret = bind(state->tx_socket, (struct sockaddr *)&bind_addr,
             sizeof(bind_addr));
  if (ret < 0)
    {
      ptperr("Failed to bind tx to port %d\n", bind_addr.sin_port);
      return ERROR;
    }

  return OK;
}
#endif // ESP_PTP

/* Unsubscribe multicast and destroy sockets */

static int ptp_destroy_state(FAR struct ptp_state_s *state)
{
#ifdef ESP_PTP
  // Remove well-known PTP multicast destination MAC addresses from the filter
  esp_eth_handle_t eth_handle;
  if (ptp_get_esp_eth_handle(state, &eth_handle) < 0)
  {
    ptperr("failed to get socket eth_handle %d\n", errno);
    return ERROR;
  }
  uint8_t dest_addr[ETH_ADDR_LEN];
  SET_MAC_ADDR(dest_addr, 0x01, 0x1B, 0x19, 0x00, 0x00, 0x00);
  esp_eth_ioctl(eth_handle, ETH_CMD_DEL_MAC_FILTER, dest_addr);
  SET_MAC_ADDR(dest_addr, 0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E);
  esp_eth_ioctl(eth_handle, ETH_CMD_DEL_MAC_FILTER, dest_addr);

  if (state->ptp_socket > 0)
  {
    close(state->ptp_socket);
    state->ptp_socket = -1;
  }
#else
  struct in_addr mcast_addr;

  mcast_addr.s_addr = HTONL(PTP_MULTICAST_ADDR);
  ipmsfilter(&state->interface_addr.sin_addr,
              &mcast_addr,
              MCAST_EXCLUDE);

  if (state->tx_socket > 0)
    {
      close(state->tx_socket);
      state->tx_socket = -1;
    }

  if (state->event_socket > 0)
    {
      close(state->event_socket);
      state->event_socket = -1;
    }

  if (state->info_socket > 0)
    {
      close(state->info_socket);
      state->info_socket = -1;
    }
#endif // ESP_PTP
  return OK;
}

#ifndef ESP_PTP
/* Re-subscribe multicast address.
 * This can become necessary if Ethernet interface gets reset or if external
 * IGMP-compliant Ethernet switch gets plugged in.
 */

static int ptp_check_multicast_status(FAR struct ptp_state_s *state)
{
#if CONFIG_NETUTILS_PTPD_MULTICAST_TIMEOUT_MS > 0
  struct in_addr mcast_addr;
  struct timespec time_now;
  struct timespec delta;

  clock_gettime(CLOCK_MONOTONIC, &time_now);
  clock_timespec_subtract(&time_now, &state->last_received_multicast,
                          &delta);

  if (timespec_to_ms(&delta) > CONFIG_NETUTILS_PTPD_MULTICAST_TIMEOUT_MS)
    {
      /* Remove and re-add the multicast group */

      state->last_received_multicast = time_now;

      mcast_addr.s_addr = HTONL(PTP_MULTICAST_ADDR);
      ipmsfilter(&state->interface_addr.sin_addr,
                 &mcast_addr,
                 MCAST_EXCLUDE);

      return ipmsfilter(&state->interface_addr.sin_addr,
                        &mcast_addr,
                        MCAST_INCLUDE);
    }

#else
  UNUSED(state);
#endif /* CONFIG_NETUTILS_PTPD_MULTICAST_TIMEOUT_MS */

  return OK;
}
#endif // !ESP_PTP

/* Send PTP server announcement packet */

static int ptp_send_announce(FAR struct ptp_state_s *state)
{
  struct ptp_announce_s msg;
#ifndef ESP_PTP
  struct sockaddr_in addr;
#endif // !ESP_PTP
  struct timespec ts;
  int ret;

#ifndef ESP_PTP
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = HTONL(PTP_MULTICAST_ADDR);
  addr.sin_port        = HTONS(PTP_UDP_PORT_INFO);
#endif // !ESP_PTP

  memset(&msg, 0, sizeof(msg));
  msg = state->own_identity;
  msg.header.messagetype = PTP_MSGTYPE_ANNOUNCE;
  msg.header.messagelength[1] = sizeof(msg);

  ptp_increment_sequence(&state->announce_seq, &msg.header);
  ptp_gettime(state, &ts);
  timespec_to_ptp_format(&ts, msg.origintimestamp);

#ifdef ESP_PTP
  ret = ptp_net_send(state, &msg, sizeof(msg), NULL);
#else
  ret = sendto(state->tx_socket, &msg, sizeof(msg), 0,
    (struct sockaddr *)&addr, sizeof(addr));
#endif // ESP_PTP

  if (ret < 0)
    {
      ptperr("sendto failed: %d", errno);
    }
  else
    {
      ptpinfo("Sent announce, seq %ld\n",
              (long)ptp_get_sequence(&msg.header));
    }

  return ret;
}

/* Send PTP server synchronization packet */

static int ptp_send_sync(FAR struct ptp_state_s *state)
{
#ifndef ESP_PTP
  struct msghdr txhdr;
  struct iovec txiov;
#endif // !ESP_PTP
  struct ptp_sync_s msg;
#ifndef ESP_PTP
  struct sockaddr_in addr;
#endif // !ESP_PTP
  struct timespec ts;
#ifndef ESP_PTP
  uint8_t controlbuf[64];
#endif // !ESP_PTP
  int ret;

#ifndef ESP_PTP
  memset(&txhdr, 0, sizeof(txhdr));
  memset(&txiov, 0, sizeof(txiov));

  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = HTONL(PTP_MULTICAST_ADDR);
  addr.sin_port        = HTONS(PTP_UDP_PORT_EVENT);
#endif // !ESP_PTP

  memset(&msg, 0, sizeof(msg));
  msg.header = state->own_identity.header;
  msg.header.messagetype = PTP_MSGTYPE_SYNC;
  msg.header.messagelength[1] = sizeof(msg);

#ifdef CONFIG_NETUTILS_PTPD_TWOSTEP_SYNC
  msg.header.flags[0] = PTP_FLAGS0_TWOSTEP;
#endif

#ifndef ESP_PTP
  txhdr.msg_name = &addr;
  txhdr.msg_namelen = sizeof(addr);
  txhdr.msg_iov = &txiov;
  txhdr.msg_iovlen = 1;
  txhdr.msg_control = controlbuf;
  txhdr.msg_controllen = sizeof(controlbuf);
  txiov.iov_base = &msg;
  txiov.iov_len = sizeof(msg);
#endif //!ESP_PTP

  /* Timestamp and send the sync message */

  ptp_increment_sequence(&state->sync_seq, &msg.header);
  ptp_gettime(state, &ts);
  timespec_to_ptp_format(&ts, msg.origintimestamp);

#ifdef ESP_PTP
  ret = ptp_net_send(state, &msg, sizeof(msg), &ts);
#else
  ret = sendmsg(state->tx_socket, &txhdr, 0);
#endif // ESP_PTP
  if (ret < 0)
    {
      ptperr("sendmsg for sync message failed: %d\n", errno);
      return ret;
    }

#ifdef CONFIG_NETUTILS_PTPD_TWOSTEP_SYNC
#ifndef ESP_PTP
  /* Get timestamp after send completes and send follow-up message
   *
   * TODO: Implement SO_TIMESTAMPING and use the actual tx timestamp here.
   */

  ptp_gettime(state, &ts);
#endif // !ESP_PTP
  timespec_to_ptp_format(&ts, msg.origintimestamp);
  msg.header.messagetype = PTP_MSGTYPE_FOLLOW_UP;
  msg.header.flags[0] = 0;
#ifndef ESP_PTP
  addr.sin_port = HTONS(PTP_UDP_PORT_INFO);

  ret = sendto(state->tx_socket, &msg, sizeof(msg), 0,
               (struct sockaddr *)&addr, sizeof(addr));
#else
  ret = ptp_net_send(state, &msg, sizeof(msg), NULL);
#endif // !ESP_PTP
  if (ret < 0)
    {
      ptperr("sendto for follow-up message failed: %d\n", errno);
      return ret;
    }

  ptpinfo("Sent sync + follow-up, seq %ld\n",
          (long)ptp_get_sequence(&msg.header));
#else
  ptpinfo("Sent sync, seq %ld\n",
          (long)ptp_get_sequence(&msg.header));
#endif /* CONFIG_NETUTILS_PTPD_TWOSTEP_SYNC */

  return OK;
}

/* Send delay request packet to selected source */

static int ptp_send_delay_req(FAR struct ptp_state_s *state)
{
  struct ptp_delay_req_s req;
#ifndef ESP_PTP
  struct sockaddr_in addr;
#endif // !ESP_PTP
  int ret;

#ifndef ESP_PTP
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = HTONL(PTP_MULTICAST_ADDR);
  addr.sin_port        = HTONS(PTP_UDP_PORT_EVENT);
#endif // !ESP_PTP

  memset(&req, 0, sizeof(req));
  req.header = state->own_identity.header;
  req.header.messagetype = PTP_MSGTYPE_DELAY_REQ;
  req.header.messagelength[1] = sizeof(req);
  ptp_increment_sequence(&state->delay_req_seq, &req.header);

  ptp_gettime(state, &state->delayreq_time);
  timespec_to_ptp_format(&state->delayreq_time, req.origintimestamp);

#ifdef ESP_PTP
  ret = ptp_net_send(state, &req, sizeof(req), &state->delayreq_time);
#else
  ret = sendto(state->tx_socket, &req, sizeof(req), 0,
               (FAR struct sockaddr *)&addr, sizeof(addr));
#endif // ESP_PTP

#ifndef ESP_PTP
  /* Get timestamp after send completes.
   * TODO: Implement SO_TIMESTAMPING and use the actual tx timestamp here.
   */

  ptp_gettime(state, &state->delayreq_time);
#endif // !ESP_PTP

  if (ret < 0)
    {
      ptperr("sendto failed: %d", errno);
    }
  else
    {
      clock_gettime(CLOCK_MONOTONIC, &state->last_transmitted_delayreq);
      ptpinfo("Sent delay req, seq %ld\n",
              (long)ptp_get_sequence(&req.header));
    }

  return ret;
}

/* Check if we need to send packets */

static int ptp_periodic_send(FAR struct ptp_state_s *state)
{
#ifdef CONFIG_NETUTILS_PTPD_SERVER
  /* If there is no better master clock on the network,
   * act as the reference source and send server packets.
   */

  if (!state->selected_source_valid)
    {
      struct timespec time_now;
      struct timespec delta;

      clock_gettime(CLOCK_MONOTONIC, &time_now);
      clock_timespec_subtract(&time_now,
        &state->last_transmitted_announce, &delta);
      if (timespec_to_ms(&delta)
          > CONFIG_NETUTILS_PTPD_ANNOUNCE_INTERVAL_MSEC)
        {
          state->last_transmitted_announce = time_now;
          ptp_send_announce(state);
        }

      clock_timespec_subtract(&time_now,
        &state->last_transmitted_sync, &delta);
      if (timespec_to_ms(&delta) > CONFIG_NETUTILS_PTPD_SYNC_INTERVAL_MSEC)
        {
          state->last_transmitted_sync = time_now;
          ptp_send_sync(state);
        }
    }
#endif /* CONFIG_NETUTILS_PTPD_SERVER */

#ifdef CONFIG_NETUTILS_PTPD_SEND_DELAYREQ
  if (state->selected_source_valid && state->can_send_delayreq)
    {
      struct timespec time_now;
      struct timespec delta;

      clock_gettime(CLOCK_MONOTONIC, &time_now);
      clock_timespec_subtract(&time_now,
                              &state->last_transmitted_delayreq, &delta);

      if (timespec_to_ms(&delta) > state->delayreq_interval * MSEC_PER_SEC)
        {
          ptp_send_delay_req(state);
        }
    }
#endif

  return OK;
}

/* Process received PTP announcement */

static int ptp_process_announce(FAR struct ptp_state_s *state,
                                FAR struct ptp_announce_s *msg)
{
  clock_gettime(CLOCK_MONOTONIC, &state->last_received_announce);

  if (is_better_clock(msg, &state->own_identity))
    {
      if (!state->selected_source_valid ||
          is_better_clock(msg, &state->selected_source))
        {
          ptpinfo("Switching to better PTP time source\n");

          state->selected_source = *msg;
          state->last_received_sync = state->last_received_announce;
          state->path_delay_avgcount = 0;
          state->path_delay_ns = 0;
          state->delayreq_time.tv_sec = 0;
        }
    }

  return OK;
}

#ifdef ESP_PTP
static void ptp_lock_local_clock_freq(FAR struct ptp_state_s *state,
                                  FAR struct timespec *remote_timestamp,
                                  FAR struct timespec *local_timestamp)
{
  // Compute how off we are against master
  int64_t offset_ns = timespec_delta_ns(remote_timestamp, local_timestamp);
  offset_ns += state->path_delay_ns;
  // TODO add offset filter

  // Execute PI controller to elimitate the offset
  // compute I component
  state->offset_pi.drift_acc += offset_ns / state->offset_pi.ki;
  // clamp the accumulator to ADJ_FREQ_MAX for sanity
  if (state->offset_pi.drift_acc > ADJ_FREQ_MAX){
    state->offset_pi.drift_acc = ADJ_FREQ_MAX;
  } else if (state->offset_pi.drift_acc < -ADJ_FREQ_MAX) {
    state->offset_pi.drift_acc = -ADJ_FREQ_MAX;
  }
  // compute P component and the whole controller
  int32_t adj = offset_ns / state->offset_pi.kp + state->offset_pi.drift_acc;

  // Compute difference between number of ticks in slave and master over sync period. This is used to lock the frequency with the master.
  // However, it never catch-up the offset by itself, hence also add `adj` at the end
  int64_t remote_time_ns = timespec_to_ns(remote_timestamp);
  int64_t local_time_ns = timespec_to_ns(local_timestamp);
  int64_t remote_delta_ns = remote_time_ns - state->remote_time_ns_prev;
  int64_t local_delta_ns = local_time_ns - state->local_time_ns_prev;
  // clock tick difference between master and slave
  int64_t tick_diff = remote_delta_ns - local_delta_ns;

  // compute how to scale the slave frequency to lock with master frequency and also try to catch-up the offset
  double freq_scale = ((double)(remote_delta_ns /*+ tick_diff*/ + adj)) / (double)local_delta_ns;
  esp_eth_clock_adj_param_t clk_adj_param = {
    .mode = ETH_CLK_ADJ_FREQ_SCALE,
    .freq_scale = freq_scale
  };
  esp_eth_clock_adjtime(CLOCK_PTP_SYSTEM, &clk_adj_param);

  state->remote_time_ns_prev = remote_time_ns;
  state->local_time_ns_prev = local_time_ns;

  ptpinfo("remote_delta_ns %lli, local_delta_ns %lli, tick_diff %lli\n", remote_delta_ns, local_delta_ns, tick_diff);
  ptpinfo("offset_ns %lli, adj %li, drift_acc %li\n", offset_ns, adj, state->offset_pi.drift_acc);

  // Get the path delay only when clock is stable enough. If we were in process of adjustion (speeding/slowing slave),
  // we would get incorrect delay
  int64_t diff = llabs(offset_ns) - llabs(state->last_offset_ns);
  static int cnt = 0;
  if (llabs(diff) < CONFIG_NETUTILS_PTPD_PATH_DELAY_STABILITY_NS) {
    if (cnt <= 3)
      cnt++;
  } else {
    cnt = 0;
  }
  if (cnt > 3)
  {
    state->can_send_delayreq = true;
  }
  state->last_offset_ns = offset_ns;
}

void ptp_clean_after_step(FAR struct ptp_state_s *state)
{
  state->remote_time_ns_prev = 0;
  state->local_time_ns_prev = 0;

  state->offset_pi.drift_acc = 0;
  state->last_offset_ns = 0;
}
#endif // ESP_PTP

/* Update local clock either by smooth adjustment or by jumping.
 * Remote time was remote_timestamp at local_timestamp.
 */

static int ptp_update_local_clock(FAR struct ptp_state_s *state,
                                  FAR struct timespec *remote_timestamp,
                                  FAR struct timespec *local_timestamp)
{
  int ret = OK;
  int64_t delta_ns;
  int64_t absdelta_ns;
  const int64_t adj_limit_ns = CONFIG_NETUTILS_PTPD_SETTIME_THRESHOLD_MS
                               * (int64_t)NSEC_PER_MSEC;

  ptpinfo("Local time: %lld.%09ld, remote time %lld.%09ld\n",
          (long long)local_timestamp->tv_sec,
          (long)local_timestamp->tv_nsec,
          (long long)remote_timestamp->tv_sec,
          (long)remote_timestamp->tv_nsec);

  delta_ns = timespec_delta_ns(remote_timestamp, local_timestamp);
  delta_ns += state->path_delay_ns;
  absdelta_ns = (delta_ns < 0) ? -delta_ns : delta_ns;

  if (absdelta_ns > adj_limit_ns)
    {
      /* Large difference, move by jumping.
       * Account for delay since packet was received.
       */

      struct timespec new_time;
      ptp_gettime(state, &new_time);
      clock_timespec_subtract(&new_time, local_timestamp, &new_time);
      clock_timespec_add(&new_time, remote_timestamp, &new_time);
      ret = ptp_settime(state, &new_time);

      /* Reinitialize drift adjustment parameters */

      state->last_delta_timestamp = new_time;
      state->last_delta_ns = 0;
      state->last_adjtime_ns = 0;
      state->drift_avg_total_ms = 0;
      state->drift_ppb = 0;

#ifdef ESP_PTP
      ptp_clean_after_step(state);
#endif // ESP_PTP

      if (ret == OK)
        {
          ptpinfo("Jumped to timestamp %lld.%09ld s\n",
                  (long long)new_time.tv_sec, (long)new_time.tv_nsec);
        }
      else
        {
          ptperr("ptp_settime() failed: %d\n", errno);
        }
    }
  else
    {
#ifdef ESP_PTP
	  ptp_lock_local_clock_freq(state, remote_timestamp, local_timestamp);
#else
      /* Track drift rate based on two consecutive measurements and
       * the adjustment that was made previously.
       */

      int64_t drift_ppb;
      struct timespec interval;
      int interval_ms;
      int max_avg_period_ms;
      int64_t adjustment_ns;

      clock_timespec_subtract(local_timestamp,
                              &state->last_delta_timestamp,
                              &interval);
      interval_ms = timespec_to_ms(&interval);

      if (interval_ms > 0 && interval_ms < CONFIG_NETUTILS_PTPD_TIMEOUT_MS)
        {
          drift_ppb = (delta_ns - state->last_delta_ns) * MSEC_PER_SEC
                      / interval_ms;
        }
      else
        {
          ptpwarn("Measurement interval out of range: %d ms\n", interval_ms);
          drift_ppb = 0;
          interval_ms = 1;
        }

      /* Account for the adjustment previously made */

      drift_ppb += state->last_adjtime_ns * MSEC_PER_SEC
                  / CONFIG_CLOCK_ADJTIME_PERIOD_MS;

      if (drift_ppb > CONFIG_CLOCK_ADJTIME_SLEWLIMIT_PPM * 1000 ||
          drift_ppb < -CONFIG_CLOCK_ADJTIME_SLEWLIMIT_PPM * 1000)
        {
          ptpwarn("Drift estimate out of range: %lld\n",
                  (long long)drift_ppb);
          drift_ppb = state->drift_ppb;
        }

      /* Take direct average of drift estimate for first measurements,
       * after that update the exponential sliding average.
       * Measurements are weighted according to the interval, because
       * drift estimate is more accurate over longer timespan.
       */

      state->drift_avg_total_ms += interval_ms;
      max_avg_period_ms = CONFIG_NETUTILS_PTPD_DRIFT_AVERAGE_S
                          * MSEC_PER_SEC;
      if (state->drift_avg_total_ms > max_avg_period_ms)
        {
          state->drift_avg_total_ms = max_avg_period_ms;
        }

      state->drift_ppb += (drift_ppb - state->drift_ppb) * interval_ms
                        / state->drift_avg_total_ms;

      /* Compute the value we need to give to adjtime() to match the
       * drift rate.
       */

      adjustment_ns = state->drift_ppb * CONFIG_CLOCK_ADJTIME_PERIOD_MS
                      / MSEC_PER_SEC;

      /* Drift estimation ensures local clock runs at same rate as remote.
       *
       * Adding the current clock offset to adjustment brings the clocks
       * to match. To avoid individual outliers from causing jitter, we
       * take the larger signed value of two previous deltas. This is based
       * on the logic that packets can get delayed in transit, but do not
       * travel backwards in time.
       *
       * Clock offset is applied over ADJTIME_PERIOD. If there is significant
       * noise in measurements, increasing ADJTIME_PERIOD will reduce its
       * effect on the local clock run rate.
       */

      if (state->last_delta_ns > delta_ns)
        {
          adjustment_ns += state->last_delta_ns;
        }
      else
        {
          adjustment_ns += delta_ns;
        }

      /* Apply adjustment and store information for next time */

      state->last_delta_ns = delta_ns;
      state->last_delta_timestamp = *local_timestamp;
      state->last_adjtime_ns = adjustment_ns;

      ptpinfo("Delta: %+lld ns, adjustment %+lld ns, drift rate %+lld ppb\n",
              (long long)delta_ns,
              (long long)state->last_adjtime_ns,
              (long long)state->drift_ppb);

      ret = ptp_adjtime(state, adjustment_ns);

      if (ret != OK)
        {
          ptperr("ptp_adjtime() failed: %d\n", errno);
        }

      /* Check if clock is stable enough for sending delay requests */

      if (absdelta_ns < CONFIG_NETUTILS_PTPD_MAX_PATH_DELAY_NS)
        {
          state->can_send_delayreq = true;
        }
#endif // ESP_PTP
    }

  return ret;
}

/* Process received PTP sync packet */

static int ptp_process_sync(FAR struct ptp_state_s *state,
                            FAR struct ptp_sync_s *msg)
{
  struct timespec remote_time;

  if (memcmp(msg->header.sourceidentity,
             state->selected_source.header.sourceidentity,
             sizeof(msg->header.sourceidentity)) != 0)
    {
      /* This packet wasn't from the currently selected source */
#ifdef ESP_PTP
      ESP_LOGD(TAG, "This packet wasn't from the currently selected source");
#endif // ESP_PTP
      return OK;
    }

  /* Update timeout tracking */

  clock_gettime(CLOCK_MONOTONIC, &state->last_received_sync);

  if (msg->header.flags[0] & PTP_FLAGS0_TWOSTEP)
    {
      /* We need to wait for a follow-up packet before setting the clock. */

      state->twostep_rxtime = state->rxtime;
      state->twostep_packet = *msg;
      ptpinfo("Waiting for follow-up\n");
      return OK;
    }

  /* Update local clock */

  ptp_format_to_timespec(msg->origintimestamp, &remote_time);
  return ptp_update_local_clock(state, &remote_time, &state->rxtime);
}

static int ptp_process_followup(FAR struct ptp_state_s *state,
                                FAR struct ptp_follow_up_s *msg)
{
  struct timespec remote_time;

  if (memcmp(msg->header.sourceidentity,
             state->twostep_packet.header.sourceidentity,
             sizeof(msg->header.sourceidentity)) != 0)
    {
      return OK; /* This packet wasn't from the currently selected source */
    }

  if (ptp_get_sequence(&msg->header)
      != ptp_get_sequence(&state->twostep_packet.header))
    {
      ptpwarn("PTP follow-up packet sequence %ld does not match initial "
              "sync packet sequence %ld, ignoring\n",
        (long)ptp_get_sequence(&msg->header),
        (long)ptp_get_sequence(&state->twostep_packet.header));
      return OK;
    }

  /* Update local clock based on the remote timestamp we received now
   * and the local timestamp of when the sync packet was received.
   */

  ptp_format_to_timespec(msg->origintimestamp, &remote_time);
  return ptp_update_local_clock(state, &remote_time, &state->twostep_rxtime);
}

static int ptp_process_delay_req(FAR struct ptp_state_s *state,
                                 FAR struct ptp_delay_req_s *msg)
{
  struct ptp_delay_resp_s resp;
#ifndef ESP_PTP
  struct sockaddr_in addr;
#endif // !ESP_PTP
  int ret;

  if (state->selected_source_valid)
    {
      /* We are operating as a client, ignore delay requests */

      return OK;
    }

#ifndef ESP_PTP
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = HTONL(PTP_MULTICAST_ADDR);
  addr.sin_port        = HTONS(PTP_UDP_PORT_INFO);
#endif // !ESP_PTP

  memset(&resp, 0, sizeof(resp));
  resp.header = state->own_identity.header;
  resp.header.messagetype = PTP_MSGTYPE_DELAY_RESP;
  resp.header.messagelength[1] = sizeof(resp);
  timespec_to_ptp_format(&state->rxtime, resp.receivetimestamp);
  memcpy(resp.reqidentity, msg->header.sourceidentity,
         sizeof(resp.reqidentity));
  memcpy(resp.reqportindex, msg->header.sourceportindex,
         sizeof(resp.reqportindex));
  memcpy(resp.header.sequenceid, msg->header.sequenceid,
         sizeof(resp.header.sequenceid));
  resp.header.logmessageinterval = CONFIG_NETUTILS_PTPD_DELAYRESP_INTERVAL;

#ifdef ESP_PTP
  ret = ptp_net_send(state, &resp, sizeof(resp), NULL);
#else
  ret = sendto(state->tx_socket, &resp, sizeof(resp), 0,
               (FAR struct sockaddr *)&addr, sizeof(addr));
#endif // ESP_PTP

  if (ret < 0)
    {
      ptperr("sendto failed: %d", errno);
    }
  else
    {
      clock_gettime(CLOCK_MONOTONIC, &state->last_transmitted_delayresp);
      ptpinfo("Sent delay resp, seq %ld\n",
              (long)ptp_get_sequence(&msg->header));
    }

  return ret;
}

static int ptp_process_delay_resp(FAR struct ptp_state_s *state,
                                  FAR struct ptp_delay_resp_s *msg)
{
  int64_t path_delay;
  int64_t sync_delay;
  struct timespec remote_rxtime;
  uint16_t sequence;
  int interval;

  if (!state->selected_source_valid ||
      memcmp(msg->header.sourceidentity,
             state->selected_source.header.sourceidentity,
             sizeof(msg->header.sourceidentity)) != 0 ||
      memcmp(msg->reqidentity,
             state->own_identity.header.sourceidentity,
             sizeof(msg->reqidentity)) != 0)
    {
      return OK; /* This packet wasn't for us */
    }

  sequence = ptp_get_sequence(&msg->header);

  if (sequence != state->delay_req_seq)
    {
      ptpwarn("Ignoring out-of-sequence delay resp (%d vs. expected %d)\n",
              (int)sequence, (int)state->delay_req_seq);
      return OK;
    }

  /* Path delay is calculated as the average between delta for sync
   * message and delta for delay req message.
   * (IEEE-1588 section 11.3: Delay request-response mechanism)
   */

  ptp_format_to_timespec(msg->receivetimestamp, &remote_rxtime);
  path_delay = timespec_delta_ns(&remote_rxtime, &state->delayreq_time);
  sync_delay = state->path_delay_ns - state->last_delta_ns;
  path_delay = (path_delay + sync_delay) / 2;

  if (path_delay >= 0 && path_delay < CONFIG_NETUTILS_PTPD_MAX_PATH_DELAY_NS)
    {
      if (state->path_delay_avgcount <
          CONFIG_NETUTILS_PTPD_DELAYREQ_AVGCOUNT)
        {
          state->path_delay_avgcount++;
        }

      state->path_delay_ns += (path_delay - state->path_delay_ns)
                              / state->path_delay_avgcount;

      ptpinfo("Path delay: %ld ns (avg: %ld ns)\n",
        (long)path_delay, (long)state->path_delay_ns);
    }
  else
    {
      ptpwarn("Path delay out of range: %lld ns\n",
              (long long)path_delay);
    }

  /* Calculate interval until next packet */

  if (msg->header.logmessageinterval <= 12)
    {
      interval = (1 << msg->header.logmessageinterval);
    }
  else
    {
      interval = 4096; /* Refuse to obey excessively long intervals */
    }

  /* Randomize up to 2x nominal delay) */

  state->delayreq_interval = interval + (random() % interval);

  return OK;
}

/* Determine received packet type and process it */

static int ptp_process_rx_packet(FAR struct ptp_state_s *state,
                                 ssize_t length)
{
  if (length < sizeof(struct ptp_header_s))
    {
      ptpwarn("Ignoring invalid PTP packet, length only %d bytes\n",
              (int)length);
      return OK;
    }

  if (state->rxbuf.header.domain != CONFIG_NETUTILS_PTPD_DOMAIN)
    {
      /* Part of different clock domain, ignore */

      return OK;
    }

  clock_gettime(CLOCK_MONOTONIC, &state->last_received_multicast);

  switch (state->rxbuf.header.messagetype & PTP_MSGTYPE_MASK)
  {
#ifdef CONFIG_NETUTILS_PTPD_CLIENT
    case PTP_MSGTYPE_ANNOUNCE:
      ptpinfo("Got announce packet, seq %ld\n",
              (long)ptp_get_sequence(&state->rxbuf.header));
      return ptp_process_announce(state, &state->rxbuf.announce);

    case PTP_MSGTYPE_SYNC:
      ptpinfo("Got sync packet, seq %ld\n",
              (long)ptp_get_sequence(&state->rxbuf.header));
      return ptp_process_sync(state, &state->rxbuf.sync);

    case PTP_MSGTYPE_FOLLOW_UP:
      ptpinfo("Got follow-up packet, seq %ld\n",
              (long)ptp_get_sequence(&state->rxbuf.header));
      return ptp_process_followup(state, &state->rxbuf.follow_up);

    case PTP_MSGTYPE_DELAY_RESP:
      ptpinfo("Got delay-resp, seq %ld\n",
              (long)ptp_get_sequence(&state->rxbuf.header));
      return ptp_process_delay_resp(state, &state->rxbuf.delay_resp);
#endif

#ifdef CONFIG_NETUTILS_PTPD_SERVER
    case PTP_MSGTYPE_DELAY_REQ:
      ptpinfo("Got delay req, seq %ld\n",
              (long)ptp_get_sequence(&state->rxbuf.header));
      return ptp_process_delay_req(state, &state->rxbuf.delay_req);
#endif

    default:
      ptpinfo("Ignoring unknown PTP packet type: 0x%02x\n",
              state->rxbuf.header.messagetype);
      return OK;
  }
}

/* Signal handler for status / stop requests */
#ifndef ESP_PTP
static void ptp_signal_handler(int signo, FAR siginfo_t *siginfo,
                               FAR void *context)
{
  FAR struct ptp_state_s *state = (FAR struct ptp_state_s *)siginfo->si_user;

  if (signo == SIGHUP)
    {
      state->stop = true;
    }
  else if (signo == SIGUSR1 && siginfo->si_value.sival_ptr)
    {
      state->status_req =
        *(FAR struct ptpd_statusreq_s *)siginfo->si_value.sival_ptr;
    }
}

static void ptp_setup_sighandlers(FAR struct ptp_state_s *state)
{
  struct sigaction act;

  act.sa_sigaction = ptp_signal_handler;
  sigfillset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  act.sa_user = state;

  sigaction(SIGHUP, &act, NULL);
  sigaction(SIGUSR1, &act, NULL);
}
#endif // !ESP_PTP

/* Process status information request */

static void ptp_process_statusreq(FAR struct ptp_state_s *state)
{
  FAR struct ptpd_status_s *status;

  if (!state->status_req.dest)
    {
      return; /* No active request */
    }

  status = state->status_req.dest;
  status->clock_source_valid = state->selected_source_valid;

  if (status->clock_source_valid)
    {
      /* Copy relevant parts of announce info to status struct */

      FAR struct ptp_announce_s *s = &state->selected_source;

      memcpy(status->clock_source_info.id,
             s->header.sourceidentity,
             sizeof(status->clock_source_info.id));

      status->clock_source_info.utcoffset =
          (int16_t)(((uint16_t)s->utcoffset[0] << 8) | s->utcoffset[1]);
      status->clock_source_info.priority1 = s->gm_priority1;
      status->clock_source_info.clockclass = s->gm_quality[0];
      status->clock_source_info.accuracy = s->gm_quality[1];
      status->clock_source_info.priority2 = s->gm_priority2;
      status->clock_source_info.variance =
          ((uint16_t)s->gm_quality[2] << 8) | s->gm_quality[3];

      memcpy(status->clock_source_info.gm_id,
             s->gm_identity,
             sizeof(status->clock_source_info.gm_id));

      status->clock_source_info.stepsremoved =
          ((uint16_t)s->stepsremoved[0] << 8) | s->stepsremoved[1];
      status->clock_source_info.timesource = s->timesource;
    }

  /* Copy latest adjustment info */

  status->last_clock_update = state->last_delta_timestamp;
  status->last_delta_ns     = state->last_delta_ns;
  status->last_adjtime_ns   = state->last_adjtime_ns;
  status->drift_ppb         = state->drift_ppb;
  status->path_delay_ns     = state->path_delay_ns;

  /* Copy timestamps */

  status->last_received_multicast    = state->last_received_multicast;
  status->last_received_announce     = state->last_received_announce;
  status->last_received_sync         = state->last_received_sync;
  status->last_transmitted_sync      = state->last_transmitted_sync;
  status->last_transmitted_announce  = state->last_transmitted_announce;
  status->last_transmitted_delayresp = state->last_transmitted_delayresp;
  status->last_transmitted_delayreq  = state->last_transmitted_delayreq;

  /* Post semaphore to inform that we are done */

  if (state->status_req.done)
    {
      sem_post(state->status_req.done);
    }

  state->status_req.done = NULL;
  state->status_req.dest = NULL;
}

/* Main PTPD task */
#ifdef ESP_PTP
static void ptp_daemon(void *task_param)
#else
static int ptp_daemon(int argc, FAR char** argv)
#endif // ESP_PTP
{
  FAR const char *interface = "eth0";
  FAR struct ptp_state_s *state;
#ifdef ESP_PTP
  struct pollfd pollfds[1]; // everything is received over one socket at L2
#else
  struct pollfd pollfds[2];
  struct msghdr rxhdr;
  struct iovec rxiov;
#endif // ESP_PTP
  int ret;

#ifndef ESP_PTP
  memset(&rxhdr, 0, sizeof(rxhdr));
  memset(&rxiov, 0, sizeof(rxiov));
#endif // !ESP_PTP

  state = calloc(1, sizeof(struct ptp_state_s));

#ifdef ESP_PTP
  if (task_param != NULL)
    {
      interface = task_param;
    }
#else
  if (argc > 1)
    {
      interface = argv[1];
    }
#endif // ESP_PTP

  if (ptp_initialize_state(state, interface) != OK)
    {
      ptperr("Failed to initialize PTP state, exiting\n");

      ptp_destroy_state(state);
      free(state);

#ifdef ESP_PTP
      goto err;
#else
      return ERROR;
#endif // ESP_PTP
    }
#ifndef ESP_PTP
  ptp_setup_sighandlers(state);
#endif // !ESP_PTP

  pollfds[0].events = POLLIN;
#ifdef ESP_PTP
  pollfds[0].fd = state->ptp_socket;
#else
  pollfds[0].fd = state->event_socket;
  pollfds[1].events = POLLIN;
  pollfds[1].fd = state->info_socket;
#endif // ESP_PTP

  while (!state->stop)
    {
      state->can_send_delayreq = false;

#ifndef ESP_PTP
      rxhdr.msg_name = NULL;
      rxhdr.msg_namelen = 0;
      rxhdr.msg_iov = &rxiov;
      rxhdr.msg_iovlen = 1;
      rxhdr.msg_control = &state->rxcmsg;
      rxhdr.msg_controllen = sizeof(state->rxcmsg);
      rxhdr.msg_flags = 0;
      rxiov.iov_base = &state->rxbuf;
      rxiov.iov_len = sizeof(state->rxbuf);
#endif // !ESP_PTP

      pollfds[0].revents = 0;
#ifndef ESP_PTP
      pollfds[1].revents = 0;
      ret = poll(pollfds, 2, PTPD_POLL_INTERVAL);
#else
	  ret = poll(pollfds, 1, PTPD_POLL_INTERVAL);
#endif // !ESP_PTP

      if (pollfds[0].revents)
        {
          /* Receive time-critical packet, potentially with cmsg
           * indicating the timestamp.
           */

#ifdef ESP_PTP
          ret = ptp_net_recv(state, &state->rxbuf, sizeof(state->rxbuf), &state->rxtime);
#else
          ret = recvmsg(state->event_socket, &rxhdr, MSG_DONTWAIT);
#endif // ESP_PTP

          if (ret > 0)
            {
#ifndef ESP_PTP
              ptp_getrxtime(state, &rxhdr, &state->rxtime);
#endif
              ptp_process_rx_packet(state, ret);
            }
        }

#ifndef ESP_PTP
      if (pollfds[1].revents)
        {
          /* Receive non-time-critical packet. */

          ret = recv(state->info_socket, &state->rxbuf, sizeof(state->rxbuf),
                    MSG_DONTWAIT);
          if (ret > 0)
            {
              ptp_process_rx_packet(state, ret);
            }
        }

      if (pollfds[0].revents == 0 && pollfds[1].revents == 0)
        {
          /* No packets received, check for multicast timeout */

          ptp_check_multicast_status(state);
        }
#endif // !ESP_PTP
      ptp_periodic_send(state);

      state->selected_source_valid = is_selected_source_valid(state);
      ptp_process_statusreq(state);
    }
  ptp_destroy_state(state);
  free(state);

#ifdef ESP_PTP
err:
  s_state = NULL;
  vTaskDelete(NULL);
#else
  return 0;
#endif // ESP_PTP
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: ptpd_start
 *
 * Description:
 *   Start the PTP daemon and bind it to specified interface.
 *
 * Input Parameters:
 *   interface - Name of the network interface to bind to, e.g. "eth0"
 *
 * Returned Value:
 *   On success, the non-negative task ID of the PTP daemon is returned;
 *   On failure, a negated errno value is returned.
 *
 ****************************************************************************/

int ptpd_start(FAR const char *interface)
{
#ifdef ESP_PTP
  if (s_state == NULL) {
    xTaskCreate(ptp_daemon, "PTPD", CONFIG_NETUTILS_PTPD_STACKSIZE,
              (void *)interface, tskIDLE_PRIORITY + 2, NULL);
    return 1;
  }
  ESP_LOGE(TAG, "Other instance of PTP is already running");
  return -1;
#else
  int pid;
  FAR char *task_argv[] = {
    (FAR char *)interface,
    NULL
  };

  pid = task_create("PTPD", CONFIG_NETUTILS_PTPD_SERVERPRIO,
    CONFIG_NETUTILS_PTPD_STACKSIZE, ptp_daemon, task_argv);

  /* Use kill with signal 0 to check if the process is still alive
   * after initialization.
   */

  usleep(USEC_PER_TICK);
  if (kill(pid, 0) != OK)
    {
      return ERROR;
    }
  else
    {
      return pid;
    }
#endif // ESP_PTP
}

/****************************************************************************
 * Name: ptpd_status
 *
 * Description:
 *   Query status from a running PTP daemon.
 *
 * Input Parameters:
 *   pid     - Process ID previously returned by ptpd_start()
 *   status  - Pointer to storage for status information.
 *
 * Returned Value:
 *   On success, returns OK.
 *   On failure, a negated errno value is returned.
 *
 * Assumptions/Limitations:
 *   Multiple threads with priority less than CONFIG_NETUTILS_PTPD_SERVERPRIO
 *   can request status simultaneously. If higher priority threads request
 *   status simultaneously, some of the requests may timeout.
 *
 ****************************************************************************/

int ptpd_status(int pid, FAR struct ptpd_status_s *status)
{
#ifdef ESP_PTP
  int ret = 0;
  sem_t donesem;
  struct ptpd_statusreq_s req;
  struct timespec timeout;

  /* Fill in the status request */

  memset(status, 0, sizeof(struct ptpd_status_s));
  sem_init(&donesem, 0, 0);
  req.done = &donesem;
  req.dest = status;

  s_state->status_req = req;

  /* Wait for status request to be handled */
  clock_gettime(CLOCK_REALTIME, &timeout); // sem_timedwait uses CLOCK_REALTIME
  timeout.tv_sec += 1;

  if (sem_timedwait(&donesem, &timeout) != 0)
    {
      req.done = NULL;
      req.dest = NULL;
      s_state->status_req = req;
      ret = -errno;
    }
  sem_destroy(&donesem);

  return ret;
#endif
#ifndef CONFIG_BUILD_FLAT

  /* TODO: Use SHM memory to pass the status information if processes
   * do not share the same memory space.
   */

  return -ENOTSUP;

#else

  int ret = OK;
  sem_t donesem;
  struct ptpd_statusreq_s req;
  union sigval val;
  struct timespec timeout;

  /* Fill in the status request */

  memset(status, 0, sizeof(struct ptpd_status_s));
  sem_init(&donesem, 0, 0);
  req.done = &donesem;
  req.dest = status;
  val.sival_ptr = &req;

  if (sigqueue(pid, SIGUSR1, val) != OK)
    {
      return -errno;
    }

  /* Wait for status request to be handled */

  clock_gettime(CLOCK_MONOTONIC, &timeout);
  timeout.tv_sec += 1;
  if (sem_clockwait(&donesem, CLOCK_MONOTONIC, &timeout) != 0)
    {
      ret = -errno;
    }

  return ret;

#endif /* CONFIG_BUILD_FLAT */
}

/****************************************************************************
 * Name: ptpd_stop
 *
 * Description:
 *   Stop PTP daemon
 *
 * Input Parameters:
 *   pid     - Process ID previously returned by ptpd_start()
 *
 * Returned Value:
 *   On success, returns OK.
 *   On failure, a negated errno value is returned.
 *
 ****************************************************************************/

int ptpd_stop(int pid)
{
#ifdef ESP_PTP
  s_state->stop = true;
  return OK;
#else
  if (kill(pid, SIGHUP) == OK)
    {
      return OK;
    }
  else
    {
      return -errno;
    }
#endif
}
