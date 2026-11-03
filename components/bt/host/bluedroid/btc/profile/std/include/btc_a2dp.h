/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 *
 *  Filename:      btc_a2dp.h
 *
 *  Description:   Common definitions for A2DP
 *
 *******************************************************************************/

#ifndef __BTC_A2DP_H__
#define __BTC_A2DP_H__

#include <stdint.h>
#include "common/bt_target.h"
#include "bta/bta_api.h"
#include "btc_av_api.h"
#include "esp_a2dp_api.h"

#if BTC_AV_INCLUDED

/*******************************************************************************
 **  Constants
 *******************************************************************************/
#define BTC_AV_SUCCESS                             (0)
/**
 * AV (Audio Video source) Errors
 */
#define BTC_ERROR_SRV_AV_NOT_ENABLED               700     /* AV is not enabled */
#define BTC_ERROR_SRV_AV_FEEDING_NOT_SUPPORTED     701     /* Requested Feeding not supported */
#define BTC_ERROR_SRV_AV_BUSY                      702     /* Another operation ongoing */
#define BTC_ERROR_SRV_AV_NOT_OPENED                703     /* No AV link opened */
#define BTC_ERROR_SRV_AV_NOT_STARTED               704     /* AV is not started */
#define BTC_ERROR_SRV_AV_CP_NOT_SUPPORTED          705     /* Content protection is not supported by all headsets */

/* Transcoding definition for TxTranscoding and RxTranscoding */
#define BTC_MEDIA_TRSCD_OFF                        0
#define BTC_MEDIA_TRSCD_PCM_2_SBC                  1       /* Tx */


/*******************************************************************************
 **  Data types
 *******************************************************************************/
typedef int tBTC_AV_STATUS;

/*******************************************************************************
 **  Public functions
 *******************************************************************************/

void btc_a2dp_on_init(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_on_idle
 **
 ** Description      Process 'idle' request from BTC AV state machine during
 **                  initialization
 **
 *******************************************************************************/
void btc_a2dp_on_idle(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_on_started
 **
 ** Description      Process 'start' request from BTC AV state machine to prepare
 **                  for A2DP streaming
 **
 ** Return           TRUE if an ACK for the local command is sent
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_on_started(tBTA_AV_START *p_av, BOOLEAN pending_start);

/*******************************************************************************
 **
 ** Function         btc_a2dp_on_stopped
 **
 ** Description      Process 'stop' request from BTC AV state machine to stop
 **                  A2DP streaming
 **
 *******************************************************************************/
void btc_a2dp_on_stopped(tBTA_AV_SUSPEND *p_av);

/*******************************************************************************
 **
 ** Function         btc_a2dp_on_suspended
 **
 ** Description      Process 'stop' request from BTC AV state machine to suspend
 **                  A2DP streaming
 **
 *******************************************************************************/
void btc_a2dp_on_suspended(tBTA_AV_SUSPEND *p_av);

#if BTC_A2DP_AUDIO_STATS
/*******************************************************************************
 **
 ** Function         btc_a2dp_audio_stats_init
 **
 ** Description      Prepare the audio glitch counters. They are only updated
 **                  on the BTC task.
 **
 *******************************************************************************/
void btc_a2dp_audio_stats_init(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_audio_stats_deinit
 **
 ** Description      Clear the audio glitch counters. Called when A2DP is shut
 **                  down.
 **
 *******************************************************************************/
void btc_a2dp_audio_stats_deinit(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_audio_stats_reset
 **
 ** Description      Clear the packet counters of conn_hdl and begin counting.
 **                  Called when that link becomes STARTED.
 **
 *******************************************************************************/
void btc_a2dp_audio_stats_reset(esp_a2d_conn_hdl_t conn_hdl);

/*******************************************************************************
 **
 ** Function         btc_a2dp_audio_stats_stop
 **
 ** Description      Stop counting for conn_hdl when streaming stops or the
 **                  link closes. The counters stay readable until the next
 **                  STARTED.
 **
 *******************************************************************************/
void btc_a2dp_audio_stats_stop(esp_a2d_conn_hdl_t conn_hdl);

/*******************************************************************************
 **
 ** Function         btc_a2dp_audio_stats_get
 **
 ** Description      Copy the glitch counters. This call does not clear them.
 **
 ** Return           ESP_OK on success, otherwise an esp_err_t error code
 **
 *******************************************************************************/
esp_err_t btc_a2dp_audio_stats_get(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_stats_t *stats);

/*******************************************************************************
 **
 ** Function         btc_a2dp_audio_stats_on_lost
 **
 ** Description      Record one glitch of pkts packets on conn_hdl. Source:
 **                  packets dropped from the TX queue. Sink: packets missing
 **                  from a forward sequence gap. Ignored unless that stream
 **                  is playing.
 **
 *******************************************************************************/
void btc_a2dp_audio_stats_on_lost(esp_a2d_conn_hdl_t conn_hdl, uint32_t pkts);

/*******************************************************************************
 **
 ** Function         btc_a2dp_audio_stats_on_handled
 **
 ** Description      Count one packet in handled_pkts. Source: one packet
 **                  placed on the TX queue. Sink: one media packet received
 **                  at BTC. Ignored unless that stream is playing.
 **
 *******************************************************************************/
void btc_a2dp_audio_stats_on_handled(esp_a2d_conn_hdl_t conn_hdl);
#else
#define btc_a2dp_audio_stats_init()
#define btc_a2dp_audio_stats_deinit()
#define btc_a2dp_audio_stats_reset(conn_hdl)          ((void)(conn_hdl))
#define btc_a2dp_audio_stats_stop(conn_hdl)           ((void)(conn_hdl))
#define btc_a2dp_audio_stats_on_lost(conn_hdl, pkts)  ((void)(conn_hdl), (void)(pkts))
#define btc_a2dp_audio_stats_on_handled(conn_hdl)     ((void)(conn_hdl))
#endif

#endif /* #if BTC_AV_INCLUDED */

#endif /* __BTC_A2DP_H__ */
