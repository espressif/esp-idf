/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __APP_SPP_MSG_PRS_H__
#define __APP_SPP_MSG_PRS_H__

typedef enum {
    SPP_MSG_PRS_ERR_OK = 0,          // a complete message is finished
    SPP_MSG_PRS_ERR_IN_PROGRESS,     // message parsing is in progress
    SPP_MSG_PRS_ERR_HDR_UNDETECTED,  // header not detected
    SPP_MSG_PRS_ERR_HDR_SYNC_FAILED, // failed to sync header
    SPP_MSG_PRS_ERR_BUF_OVERFLOW,    // exceeds the buffer size: SPP_MSG_LEN_MAX
} spp_msg_prs_err_t;

typedef enum {
    SPP_MSG_PRS_IDLE = 0,
    SPP_MSG_PRS_HDR,
    SPP_MSG_PRS_PAYL,
} spp_msg_prs_state_t;

typedef void (*spp_msg_callback)(char *buf, int len);

#define SPP_MSG_LEN_MAX        (128)

typedef struct {
    spp_msg_prs_state_t state;
    char buf[SPP_MSG_LEN_MAX + 1];
    int cnt;
    int h_idx;
    int t_idx;
    spp_msg_callback callback;
} spp_msg_prs_cb_t;

void spp_msg_parser_reset_state(spp_msg_prs_cb_t *prs);

void spp_msg_parser_register_callback(spp_msg_prs_cb_t *prs, spp_msg_callback cb);

spp_msg_prs_err_t spp_msg_parse(char c, spp_msg_prs_cb_t *prs);

void spp_msg_show_usage(void);

#endif /* __APP_SPP_MSG_PRS_H__*/
